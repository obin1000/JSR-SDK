//! Calls the C API through the raw bindings: the DLL loads, the functions
//! link by name, and results and errors come back as jsr_sdk.h describes.
use core::ffi::{c_char, CStr};
use jsr_sdk_sys::*;
use std::ptr;

fn last_error() -> String {
    let mut buffer = [0 as c_char; 1024];
    unsafe {
        JSR_GetLastErrorMessage(buffer.as_mut_ptr(), buffer.len());
        CStr::from_ptr(buffer.as_ptr())
            .to_string_lossy()
            .into_owned()
    }
}

#[test]
fn api_version_matches_the_bindings() {
    let mut version = 0;
    assert_eq!(unsafe { JSR_GetApiVersion(&mut version) }, JSR_OK);
    assert_eq!(version, JSR_API_VERSION);
}

#[test]
fn library_version_is_a_string_result() {
    let mut required = 0;
    let rc = unsafe { JSR_GetLibraryVersion(ptr::null_mut(), 0, &mut required) };
    assert_eq!(rc, JSR_OK);
    assert!(required > 1);

    let mut buffer = vec![0 as c_char; required];
    let rc = unsafe { JSR_GetLibraryVersion(buffer.as_mut_ptr(), buffer.len(), ptr::null_mut()) };
    assert_eq!(rc, JSR_OK);
    let version = unsafe { CStr::from_ptr(buffer.as_ptr()) }.to_str().unwrap();
    assert_eq!(version.len(), required - 1);
}

#[test]
fn manager_loads_plugins() {
    unsafe {
        let mut manager = ptr::null_mut();
        assert_eq!(JSR_Create(&mut manager), JSR_OK, "{}", last_error());
        assert!(!manager.is_null());
        assert_eq!(
            JSR_LoadPluginsFromBinaryDir(manager),
            JSR_OK,
            "{}",
            last_error()
        );

        let mut count = 0;
        let rc = JSR_GetPluginNames(manager, ptr::null_mut(), 0, 0, &mut count);
        assert_eq!(rc, JSR_OK, "{}", last_error());
        assert!(count > 0, "no plugins next to the test program");

        assert_eq!(JSR_Destroy(manager), JSR_OK);
    }
}

#[test]
fn failure_returns_a_code_and_a_message() {
    assert_eq!(
        unsafe { JSR_GetApiVersion(ptr::null_mut()) },
        JSR_ERROR_NULL_POINTER
    );
    assert!(last_error().contains("version"), "{}", last_error());
}

#[test]
#[cfg(target_pointer_width = "64")]
fn struct_sizes_match_the_c_api() {
    use core::mem::size_of;
    // From jsr_sdk.h on x64: 256-byte strings, int32_t, size_t and double.
    assert_eq!(size_of::<JSR_InstrumentID>(), 4 * 256);
    assert_eq!(size_of::<JSR_PulserReceiverID>(), 4 * 256 + 4);
    assert_eq!(size_of::<JSR_LibMetadata>(), 34080);
}
