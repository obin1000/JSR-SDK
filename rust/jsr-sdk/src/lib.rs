//! Rust API for [JSR-SDK](https://github.com/obin1000/JSR-SDK), which drives
//! JSR (Imaginant) pulser-receivers through the closed-source JSRDotNETSDK.
//!
//! ```no_run
//! use jsr_sdk::Manager;
//!
//! fn main() -> jsr_sdk::Result<()> {
//!     let manager = Manager::new()?;
//!     manager.load_plugins_from_binary_dir()?;
//!     // Discovery only searches managed plugins.
//!     for plugin in manager.get_plugin_names()? {
//!         manager.add_managed_plugin(&plugin)?;
//!     }
//!     manager.set_discovery_enable(true)?;
//!     std::thread::sleep(std::time::Duration::from_secs(5));
//!     for id in manager.get_pulser_receivers()? {
//!         println!("{} {}", id.instrument_id.model_name, id.instrument_id.serial_num);
//!     }
//!     Ok(())
//! }
//! ```
//!
//! # How it maps to the SDK
//! [`Manager`] has one method per SDK method, named after the C function in
//! snake_case (`JSR_GetGainIndex` is [`Manager::get_gain_index`]). Every
//! method returns a [`Result`]; an [`Error`] carries the SDK's error code and
//! message. Strings and lists come back as `String` and `Vec`, and the SDK's
//! enums are Rust enums.
//!
//! # Threads and callbacks
//! - A [`Manager`] can move to another thread (`Send`) but can't be shared
//!   (`!Sync`): the SDK expects one thread at a time per manager.
//! - Callbacks run on the SDK's threads, and can run while one of your calls
//!   is in progress: a setter raises its status event before it returns.
//!   Keep them short. Callbacks of one kind never overlap.
//! - Once [`Manager::set_status_callback`] or a `remove_*` method returns,
//!   the old callback isn't running and won't be called again.
//! - A panic in a callback is caught and dropped, as the C++ wrapper drops
//!   exceptions: it can't unwind into the SDK.
//!
//! # The DLLs
//! `JSR-SDK.dll`, `JSRDotNETSDK.dll` and the plugin DLLs must be next to the
//! program. The `jsr-sdk-sys` build script copies them next to Cargo's build
//! output, so `cargo run` and `cargo test` work; ship them with your program.
#![allow(clippy::new_without_default)]

use jsr_sdk_sys as sys;
use std::ffi::{c_char, c_void, CStr, CString};
use std::fmt;
use std::panic::{catch_unwind, AssertUnwindSafe};
use std::ptr;

mod generated;
pub use generated::*;

/// The API version this crate was built for; see [`api_version`].
pub const API_VERSION: i32 = sys::JSR_API_VERSION;

/// The result of an SDK call.
pub type Result<T> = std::result::Result<T, Error>;

/// A failed SDK call: its error code and the SDK's message.
#[derive(Clone, Debug, PartialEq, Eq)]
pub struct Error {
    code: ErrorCode,
    message: String,
}

impl Error {
    /// The SDK's error code (`JSR_ERROR_*`).
    pub fn code(&self) -> ErrorCode {
        self.code
    }

    /// What went wrong, from the SDK.
    pub fn message(&self) -> &str {
        &self.message
    }

    fn new(code: ErrorCode, message: impl Into<String>) -> Self {
        Self {
            code,
            message: message.into(),
        }
    }
}

impl fmt::Display for Error {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{} ({:?})", self.message, self.code)
    }
}

impl std::error::Error for Error {}

/// The API version of the loaded `JSR-SDK.dll`. [`Manager::new`] checks
/// that it is [`API_VERSION`].
pub fn api_version() -> Result<i32> {
    let mut version = 0;
    check(unsafe { sys::JSR_GetApiVersion(&mut version) })?;
    Ok(version)
}

/// The release of the loaded `JSR-SDK.dll`, such as `1.0.0`. For messages
/// and logs; [`api_version`] decides compatibility.
pub fn library_version() -> Result<String> {
    read_string(|buffer, size, required| unsafe {
        sys::JSR_GetLibraryVersion(buffer, size, required)
    })
}

// --- Helpers for the generated methods ------------------------------------------

/// Turns a return code into a Result, with the thread's last error message.
pub(crate) fn check(rc: i32) -> Result<()> {
    if rc == sys::JSR_OK {
        return Ok(());
    }
    let mut buffer = vec![0 as c_char; 1024];
    let needed = unsafe { sys::JSR_GetLastErrorMessage(buffer.as_mut_ptr(), buffer.len()) };
    if needed > buffer.len() {
        buffer.resize(needed, 0);
        unsafe { sys::JSR_GetLastErrorMessage(buffer.as_mut_ptr(), buffer.len()) };
    }
    let message = unsafe { CStr::from_ptr(buffer.as_ptr()) }
        .to_string_lossy()
        .into_owned();
    Err(Error::new(ErrorCode::from(rc), message))
}

pub(crate) fn c_string(value: &str, name: &str) -> Result<CString> {
    CString::new(value).map_err(|_| {
        Error::new(
            ErrorCode::InvalidArgument,
            format!("{name} contains a NUL character"),
        )
    })
}

/// A string result: asks for the size, then reads, again if it grew.
pub(crate) fn read_string(
    mut fill: impl FnMut(*mut c_char, usize, *mut usize) -> i32,
) -> Result<String> {
    let mut required = 0;
    check(fill(ptr::null_mut(), 0, &mut required))?;
    loop {
        let mut buffer = vec![0 as c_char; required.max(1)];
        let mut needed = 0;
        check(fill(buffer.as_mut_ptr(), buffer.len(), &mut needed))?;
        if needed <= buffer.len() {
            return Ok(text(&buffer));
        }
        required = needed;
    }
}

/// Length of each string buffer of a string list, as in jsr_sdk.hpp.
const LIST_STRING_LENGTH: usize = 1024;

/// A string list result: asks for the count, then reads, again if it grew.
pub(crate) fn read_strings(
    mut fill: impl FnMut(*mut *mut c_char, usize, usize, *mut usize) -> i32,
) -> Result<Vec<String>> {
    let mut count = 0;
    check(fill(ptr::null_mut(), 0, LIST_STRING_LENGTH, &mut count))?;
    loop {
        let mut storage = vec![0 as c_char; count * LIST_STRING_LENGTH];
        let mut buffers: Vec<*mut c_char> = storage
            .chunks_exact_mut(LIST_STRING_LENGTH)
            .map(|chunk| chunk.as_mut_ptr())
            .collect();
        let pointer = if buffers.is_empty() {
            ptr::null_mut()
        } else {
            buffers.as_mut_ptr()
        };
        let mut available = 0;
        check(fill(
            pointer,
            buffers.len(),
            LIST_STRING_LENGTH,
            &mut available,
        ))?;
        if available <= count {
            return Ok(storage
                .chunks_exact(LIST_STRING_LENGTH)
                .take(available)
                .map(text)
                .collect());
        }
        count = available;
    }
}

/// A list result: asks for the count, then reads, again if it grew.
pub(crate) fn read_list<T: Copy>(
    zero: T,
    mut fill: impl FnMut(*mut T, usize, *mut usize) -> i32,
) -> Result<Vec<T>> {
    let mut count = 0;
    check(fill(ptr::null_mut(), 0, &mut count))?;
    loop {
        let mut items = vec![zero; count];
        let pointer = if items.is_empty() {
            ptr::null_mut()
        } else {
            items.as_mut_ptr()
        };
        let mut available = 0;
        check(fill(pointer, items.len(), &mut available))?;
        if available <= count {
            items.truncate(available);
            return Ok(items);
        }
        count = available;
    }
}

/// A NUL-terminated C string in a buffer, as UTF-8 (invalid bytes replaced).
fn text(chars: &[c_char]) -> String {
    let bytes: Vec<u8> = chars
        .iter()
        .take_while(|&&c| c != 0)
        .map(|&c| c as u8)
        .collect();
    String::from_utf8_lossy(&bytes).into_owned()
}

/// Copies text into a fixed C buffer, truncated so the NUL fits.
fn copy_text(dest: &mut [c_char], value: &str) {
    let length = value.len().min(dest.len() - 1);
    for (d, &s) in dest.iter_mut().zip(&value.as_bytes()[..length]) {
        *d = s as c_char;
    }
    dest[length] = 0;
}

/// A JSR_ERROR_* field of an event: None for JSR_OK.
fn optional_error(code: i32) -> Option<ErrorCode> {
    (code != sys::JSR_OK).then(|| ErrorCode::from(code))
}

// --- Structs ----------------------------------------------------------------------

/// Identifies an instrument (`JSR_InstrumentID`).
#[derive(Clone, Debug, Default, PartialEq, Eq, Hash)]
pub struct InstrumentId {
    /// Such as "JSR-DPR300".
    pub model_name: String,
    /// Such as "JB0104".
    pub serial_num: String,
    /// Such as "COM3"; can be empty.
    pub port: String,
    /// The plugin that manages the device.
    pub plugin_name: String,
}

impl InstrumentId {
    fn from_c(c: &sys::JSR_InstrumentID) -> Self {
        Self {
            model_name: text(&c.ModelName),
            serial_num: text(&c.SerialNum),
            port: text(&c.Port),
            plugin_name: text(&c.PluginName),
        }
    }

    fn to_c(&self) -> sys::JSR_InstrumentID {
        let mut c = sys::JSR_InstrumentID {
            ModelName: [0; sys::JSR_STRING_LENGTH],
            SerialNum: [0; sys::JSR_STRING_LENGTH],
            Port: [0; sys::JSR_STRING_LENGTH],
            PluginName: [0; sys::JSR_STRING_LENGTH],
        };
        copy_text(&mut c.ModelName, &self.model_name);
        copy_text(&mut c.SerialNum, &self.serial_num);
        copy_text(&mut c.Port, &self.port);
        copy_text(&mut c.PluginName, &self.plugin_name);
        c
    }
}

/// Identifies one pulser-receiver channel (`JSR_PulserReceiverID`).
#[derive(Clone, Debug, Default, PartialEq, Eq, Hash)]
pub struct PulserReceiverId {
    pub instrument_id: InstrumentId,
    /// Zero-based channel index.
    pub pulser_receiver_index: i32,
}

impl PulserReceiverId {
    fn from_c(c: &sys::JSR_PulserReceiverID) -> Self {
        Self {
            instrument_id: InstrumentId::from_c(&c.InstrumentId),
            pulser_receiver_index: c.PulserReceiverIndex,
        }
    }

    pub(crate) fn to_c(&self) -> sys::JSR_PulserReceiverID {
        sys::JSR_PulserReceiverID {
            InstrumentId: self.instrument_id.to_c(),
            PulserReceiverIndex: self.pulser_receiver_index,
        }
    }
}

/// Describes a plugin library (`JSR_LibMetadata`).
#[derive(Clone, Debug, Default, PartialEq, Eq)]
pub struct LibMetadata {
    pub name: String,
    pub guid: String,
    pub friendly_name: String,
    pub version: String,
    pub interface_version: i32,
    pub connection_types: Vec<ConnectionType>,
    pub supported_models: Vec<String>,
    pub open_options: Vec<String>,
}

impl LibMetadata {
    fn from_c(c: &sys::JSR_LibMetadata) -> Self {
        let count = |n: usize| n.min(sys::JSR_MAX_ARRAY_SIZE);
        Self {
            name: text(&c.Name),
            guid: text(&c.GUID),
            friendly_name: text(&c.FriendlyName),
            version: text(&c.Version),
            interface_version: c.InterfaceVersion,
            connection_types: c.ConnectionType[..count(c.ConnectionTypeCount)]
                .iter()
                .map(|&t| ConnectionType::from(t))
                .collect(),
            supported_models: c.SupportedModels[..count(c.SupportedModelsCount)]
                .iter()
                .map(|s| text(s))
                .collect(),
            open_options: c.OpenOptions[..count(c.OpenOptionsCount)]
                .iter()
                .map(|s| text(s))
                .collect(),
        }
    }
}

/// A status change of the current pulser-receiver
/// (`JSR_StatusChangedEvent`).
#[derive(Clone, Debug, PartialEq)]
pub struct StatusChangedEvent {
    /// The property that changed, such as "GainIndex".
    pub pulser_property: String,
    /// Its new value, as text.
    pub new_value: String,
    pub data_type: PulserPropertyDataType,
    pub pulser_state: PulserReceiverState,
    pub change_type: StatusChange,
    pub pulser_receiver_id: PulserReceiverId,
    pub error_message: String,
    /// None when the change succeeded.
    pub error_code: Option<ErrorCode>,
    /// The exception the SDK attached to the event, if any.
    pub exception_error_code: Option<ErrorCode>,
    pub exception_message: String,
    pub exception_inner_message: String,
}

impl StatusChangedEvent {
    fn from_c(c: &sys::JSR_StatusChangedEvent) -> Self {
        Self {
            pulser_property: text(&c.pulserProperty),
            new_value: text(&c.newValue),
            data_type: c.dataType.into(),
            pulser_state: c.pulserState.into(),
            change_type: c.changeType.into(),
            pulser_receiver_id: PulserReceiverId::from_c(&c.pulserReceiverId),
            error_message: text(&c.errorMessage),
            error_code: optional_error(c.errorCode),
            exception_error_code: optional_error(c.exceptionErrorCode),
            exception_message: text(&c.exceptionMessage),
            exception_inner_message: text(&c.exceptionInnerMessage),
        }
    }
}

/// A notification from the manager, such as a discovered device
/// (`JSR_NotifyEvent`).
#[derive(Clone, Debug, PartialEq)]
pub struct NotifyEvent {
    pub model: String,
    pub serial: String,
    pub plugin_name: String,
    pub pr_index: i32,
    pub pulser_receiver_id: PulserReceiverId,
    pub instrument_id: InstrumentId,
    pub state: PulserReceiverState,
    pub notify_type: NotifyType,
    pub discover_state: DiscoveryStateFlags,
    pub was_selected: bool,
    pub property_name: String,
    pub new_value: String,
    pub data_type: PulserPropertyDataType,
    pub max_frequency: f64,
    pub info: Vec<String>,
    pub exception_type_info: String,
    pub error_msg: String,
    pub error_text: String,
}

impl NotifyEvent {
    fn from_c(c: &sys::JSR_NotifyEvent) -> Self {
        let info_count = c.infoCount.min(sys::JSR_MAX_INFO_COUNT);
        Self {
            model: text(&c.model),
            serial: text(&c.serial),
            plugin_name: text(&c.pluginName),
            pr_index: c.prIndex,
            pulser_receiver_id: PulserReceiverId::from_c(&c.pulserReceiverId),
            instrument_id: InstrumentId::from_c(&c.instrumentId),
            state: c.state.into(),
            notify_type: c.notifyType.into(),
            discover_state: DiscoveryStateFlags(c.discoverState),
            was_selected: c.wasSelected != 0,
            property_name: text(&c.propertyName),
            new_value: text(&c.newValue),
            data_type: c.dataType.into(),
            max_frequency: c.maxFrequency,
            info: c.info[..info_count].iter().map(|s| text(s)).collect(),
            exception_type_info: text(&c.exceptionTypeInfo),
            error_msg: text(&c.errorMsg),
            error_text: text(&c.errorText),
        }
    }
}

// --- Manager ----------------------------------------------------------------------

type StatusCallback = Box<dyn FnMut(&StatusChangedEvent) + Send>;
type NotifyCallback = Box<dyn FnMut(&NotifyEvent) + Send>;

/// Owns a manager of the SDK, and frees it when dropped.
///
/// Its methods are generated, one per SDK method; see the crate
/// documentation for the rules on threads and callbacks.
pub struct Manager {
    handle: *mut sys::JSR_Manager,
    // Boxed twice so the C side gets a thin pointer that stays put.
    status: Option<Box<StatusCallback>>,
    notify: Option<Box<NotifyCallback>>,
}

// The SDK allows a manager on any thread, one at a time; it isn't Sync.
unsafe impl Send for Manager {}

impl Manager {
    /// Creates a manager. The first one starts the .NET runtime.
    ///
    /// Fails with [`ErrorCode::VersionMismatch`] when the loaded
    /// `JSR-SDK.dll` implements another API version than this crate.
    pub fn new() -> Result<Self> {
        let loaded = api_version()?;
        if loaded != API_VERSION {
            let release = library_version().unwrap_or_default();
            return Err(Error::new(
                ErrorCode::VersionMismatch,
                format!(
                    "JSR-SDK.dll {release} implements API version {loaded}, but \
                     this crate was built for API version {API_VERSION}"
                ),
            ));
        }
        let mut handle = ptr::null_mut();
        check(unsafe { sys::JSR_Create(&mut handle) })?;
        Ok(Self {
            handle,
            status: None,
            notify: None,
        })
    }

    /// The `JSR_Manager` handle, for calling `jsr_sdk_sys` directly. It
    /// stays owned by this Manager.
    pub fn as_ptr(&self) -> *mut sys::JSR_Manager {
        self.handle
    }

    /// Sets the status-change callback, replacing any previous one.
    pub fn set_status_callback<F>(&mut self, callback: F) -> Result<()>
    where
        F: FnMut(&StatusChangedEvent) + Send + 'static,
    {
        let mut boxed: Box<StatusCallback> = Box::new(Box::new(callback));
        let user = &mut *boxed as *mut StatusCallback as *mut c_void;
        check(unsafe {
            sys::JSR_SetStatusChangeCallback(self.handle, Some(status_trampoline), user)
        })?;
        // The old callback isn't running any more, so it can go.
        self.status = Some(boxed);
        Ok(())
    }

    /// Removes the status-change callback.
    pub fn remove_status_callback(&mut self) -> Result<()> {
        check(unsafe { sys::JSR_SetStatusChangeCallback(self.handle, None, ptr::null_mut()) })?;
        self.status = None;
        Ok(())
    }

    /// Sets the notify callback, replacing any previous one.
    pub fn set_notify_callback<F>(&mut self, callback: F) -> Result<()>
    where
        F: FnMut(&NotifyEvent) + Send + 'static,
    {
        let mut boxed: Box<NotifyCallback> = Box::new(Box::new(callback));
        let user = &mut *boxed as *mut NotifyCallback as *mut c_void;
        check(unsafe { sys::JSR_SetNotifyCallback(self.handle, Some(notify_trampoline), user) })?;
        self.notify = Some(boxed);
        Ok(())
    }

    /// Removes the notify callback.
    pub fn remove_notify_callback(&mut self) -> Result<()> {
        check(unsafe { sys::JSR_SetNotifyCallback(self.handle, None, ptr::null_mut()) })?;
        self.notify = None;
        Ok(())
    }

    /// Retrieves the instruments a plugin found.
    ///
    /// Calls `JSR_GetInstruments`.
    pub fn get_instruments(&self, plugin_name: &str) -> Result<Vec<InstrumentId>> {
        let plugin = c_string(plugin_name, "plugin_name")?;
        let zero = InstrumentId::default().to_c();
        let items = read_list(zero, |buffer, count, available| unsafe {
            sys::JSR_GetInstruments(self.handle, plugin.as_ptr(), buffer, count, available)
        })?;
        Ok(items.iter().map(InstrumentId::from_c).collect())
    }

    /// Retrieves all pulser-receivers the SDK detected. Discovery only
    /// searches managed plugins; see [`Manager::add_managed_plugin`].
    ///
    /// Calls `JSR_GetPulserReceivers`.
    pub fn get_pulser_receivers(&self) -> Result<Vec<PulserReceiverId>> {
        let zero = PulserReceiverId::default().to_c();
        let items = read_list(zero, |buffer, count, available| unsafe {
            sys::JSR_GetPulserReceivers(self.handle, buffer, count, available)
        })?;
        Ok(items.iter().map(PulserReceiverId::from_c).collect())
    }

    /// Retrieves the metadata of a plugin library.
    ///
    /// Calls `JSR_GetPluginLibraryMetadata`.
    pub fn get_plugin_library_metadata(&self, plugin_name: &str) -> Result<LibMetadata> {
        let plugin = c_string(plugin_name, "plugin_name")?;
        // About 34 KB: on the heap. All-zero is a valid JSR_LibMetadata.
        let mut metadata: Box<sys::JSR_LibMetadata> = Box::new(unsafe { std::mem::zeroed() });
        check(unsafe {
            sys::JSR_GetPluginLibraryMetadata(self.handle, plugin.as_ptr(), &mut *metadata)
        })?;
        Ok(LibMetadata::from_c(&metadata))
    }

    /// Retrieves the ID of the current pulser-receiver.
    ///
    /// Calls `JSR_GetId`.
    pub fn get_id(&self) -> Result<PulserReceiverId> {
        let mut id = PulserReceiverId::default().to_c();
        check(unsafe { sys::JSR_GetId(self.handle, &mut id) })?;
        Ok(PulserReceiverId::from_c(&id))
    }
}

impl Drop for Manager {
    fn drop(&mut self) {
        // Also removes the callbacks; the boxes are freed afterwards.
        unsafe { sys::JSR_Destroy(self.handle) };
    }
}

impl fmt::Debug for Manager {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_struct("Manager")
            .field("handle", &self.handle)
            .finish()
    }
}

// Called by the SDK on its threads. Callbacks of one kind never overlap, so
// the &mut is exclusive. A panic is caught: it must not unwind into the SDK.
unsafe extern "C" fn status_trampoline(
    event: *const sys::JSR_StatusChangedEvent,
    user: *mut c_void,
) {
    if event.is_null() || user.is_null() {
        return;
    }
    let callback = &mut *(user as *mut StatusCallback);
    let event = StatusChangedEvent::from_c(&*event);
    let _ = catch_unwind(AssertUnwindSafe(|| callback(&event)));
}

unsafe extern "C" fn notify_trampoline(event: *const sys::JSR_NotifyEvent, user: *mut c_void) {
    if event.is_null() || user.is_null() {
        return;
    }
    let callback = &mut *(user as *mut NotifyCallback);
    let event = NotifyEvent::from_c(&*event);
    let _ = catch_unwind(AssertUnwindSafe(|| callback(&event)));
}
