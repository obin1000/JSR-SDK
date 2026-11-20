//! The safe API against a pulser-receiver the SDK's diagnostics plugin
//! simulates ("SIM 00", channel 0), so no hardware is needed.
//!
//! Discovery takes seconds, so the tests share one manager; the Mutex runs
//! them one at a time, as the SDK requires.
use jsr_sdk::{ErrorCode, Manager, NotifyType, ReceiverMode, StatusChangedEvent};
use std::sync::atomic::{AtomicUsize, Ordering};
use std::sync::{Arc, Mutex, MutexGuard, OnceLock};
use std::thread::sleep;
use std::time::{Duration, Instant};

const PLUGIN: &str = "JSRDotNET_DiagLib";
const MODEL: &str = "SIM";
const SERIAL: &str = "00";
const TIMEOUT: Duration = Duration::from_secs(20);

/// Polls until condition() is true or the timeout passes.
fn wait_for(mut condition: impl FnMut() -> bool) -> bool {
    let deadline = Instant::now() + TIMEOUT;
    while !condition() {
        if Instant::now() > deadline {
            return false;
        }
        sleep(Duration::from_millis(50));
    }
    true
}

/// Loads the simulator plugin, discovers and selects "SIM 00" channel 0.
fn select_simulator() -> Manager {
    let manager = Manager::new().unwrap();
    manager.load_plugins_from_binary_dir().unwrap();
    manager.add_managed_plugin(PLUGIN).unwrap();
    manager
        .add_plugin_open_option(PLUGIN, "SHOW_SIMULATOR_WINDOWS", "NO")
        .unwrap();
    manager
        .add_plugin_open_option(PLUGIN, "CONNECT_ON_OPEN", "YES")
        .unwrap();
    manager.set_discovery_enable(true).unwrap();
    assert!(
        wait_for(|| manager
            .set_current_pulser_receiver_by_model(MODEL, SERIAL, 0)
            .is_ok()),
        "the simulated pulser-receiver wasn't found"
    );
    manager
}

fn simulator() -> MutexGuard<'static, Manager> {
    static MANAGER: OnceLock<Mutex<Manager>> = OnceLock::new();
    let guard = MANAGER
        .get_or_init(|| Mutex::new(select_simulator()))
        .lock()
        // A failed test must not fail the others.
        .unwrap_or_else(|poisoned| poisoned.into_inner());
    guard
        .set_current_pulser_receiver_by_model(MODEL, SERIAL, 0)
        .unwrap();
    guard
}

/// Removes the callbacks when a test ends, also when it fails.
struct Callbacks<'a>(MutexGuard<'a, Manager>);

impl Drop for Callbacks<'_> {
    fn drop(&mut self) {
        let _ = self.0.remove_status_callback();
        let _ = self.0.remove_notify_callback();
    }
}

fn is_gain_index(event: &StatusChangedEvent, value: &str) -> bool {
    event.pulser_property == "GainIndex" && event.new_value == value
}

#[test]
fn discovers_and_selects_the_simulator() {
    let manager = simulator();
    let found = manager.get_pulser_receivers().unwrap();
    assert!(found.iter().any(|id| id.instrument_id.model_name == MODEL));
    assert!(manager.get_is_pulser_receiver_selected().unwrap());
    let id = manager.get_id().unwrap();
    assert_eq!(id.instrument_id.serial_num, SERIAL);
    assert_eq!(id.pulser_receiver_index, 0);
    assert!(!manager.get_info().unwrap().is_empty());
}

#[test]
fn gain_index_round_trips() {
    let manager = simulator();
    let gains = manager.get_gain_values().unwrap();
    assert!(gains.len() > 6);
    manager.set_gain_index(5).unwrap();
    assert_eq!(manager.get_gain_index().unwrap(), 5);
    assert_eq!(manager.get_gain().unwrap(), gains[5]);
    manager.set_gain_index(6).unwrap();
    assert_eq!(manager.get_gain_index().unwrap(), 6);
}

#[test]
fn receiver_mode_round_trips() {
    let manager = simulator();
    let original = manager.get_receiver_mode().unwrap();
    for mode in [ReceiverMode::Thru, ReceiverMode::Echo] {
        manager.set_receiver_mode(mode).unwrap();
        assert_eq!(manager.get_receiver_mode().unwrap(), mode);
    }
    manager.set_receiver_mode(original).unwrap();
}

#[test]
fn invalid_enum_value_is_rejected() {
    let manager = simulator();
    let original = manager.get_receiver_mode().unwrap();
    let error = manager
        .set_receiver_mode(ReceiverMode::Unrecognized(7))
        .unwrap_err();
    assert_eq!(error.code(), ErrorCode::InvalidArgument);
    assert_eq!(manager.get_receiver_mode().unwrap(), original);
}

#[test]
fn status_events_carry_the_new_value() {
    let mut callbacks = Callbacks(simulator());
    let events = Arc::new(Mutex::new(Vec::<StatusChangedEvent>::new()));
    let log = Arc::clone(&events);
    callbacks
        .0
        .set_status_callback(move |event| log.lock().unwrap().push(event.clone()))
        .unwrap();

    callbacks.0.set_gain_index(5).unwrap();
    callbacks.0.set_gain_index(6).unwrap();
    assert!(wait_for(|| events
        .lock()
        .unwrap()
        .iter()
        .any(|e| is_gain_index(e, "6"))));
    let events = events.lock().unwrap();
    let event = events.iter().find(|e| is_gain_index(e, "6")).unwrap();
    assert_eq!(event.pulser_receiver_id.instrument_id.model_name, MODEL);
    assert_eq!(event.error_code, None);
}

#[test]
fn replaced_callback_is_not_called_again() {
    let mut callbacks = Callbacks(simulator());
    let first = Arc::new(AtomicUsize::new(0));
    let second = Arc::new(AtomicUsize::new(0));
    let count = Arc::clone(&first);
    callbacks
        .0
        .set_status_callback(move |_| {
            count.fetch_add(1, Ordering::SeqCst);
        })
        .unwrap();
    callbacks.0.set_gain_index(5).unwrap();
    assert!(wait_for(|| first.load(Ordering::SeqCst) > 0));

    let count = Arc::clone(&second);
    callbacks
        .0
        .set_status_callback(move |_| {
            count.fetch_add(1, Ordering::SeqCst);
        })
        .unwrap();
    let before = first.load(Ordering::SeqCst);
    callbacks.0.set_gain_index(6).unwrap();
    assert!(wait_for(|| second.load(Ordering::SeqCst) > 0));
    assert_eq!(first.load(Ordering::SeqCst), before);

    callbacks.0.remove_status_callback().unwrap();
    let removed = second.load(Ordering::SeqCst);
    callbacks.0.set_gain_index(5).unwrap();
    sleep(Duration::from_millis(500));
    assert_eq!(second.load(Ordering::SeqCst), removed);
}

#[test]
fn panicking_callback_does_not_stop_events() {
    let mut callbacks = Callbacks(simulator());
    let calls = Arc::new(AtomicUsize::new(0));
    let count = Arc::clone(&calls);
    callbacks
        .0
        .set_status_callback(move |_| {
            count.fetch_add(1, Ordering::SeqCst);
            panic!("callback failure (expected by the test)");
        })
        .unwrap();
    callbacks.0.set_gain_index(5).unwrap();
    callbacks.0.set_gain_index(6).unwrap();
    assert!(wait_for(|| calls.load(Ordering::SeqCst) >= 2));
}

#[test]
fn notify_events_arrive_during_discovery() {
    let mut callbacks = Callbacks(simulator());
    let types = Arc::new(Mutex::new(Vec::new()));
    let log = Arc::clone(&types);
    callbacks
        .0
        .set_notify_callback(move |event| log.lock().unwrap().push(event.notify_type))
        .unwrap();
    callbacks.0.set_discovery_enable(false).unwrap();
    callbacks.0.set_discovery_enable(true).unwrap();
    assert!(wait_for(|| types
        .lock()
        .unwrap()
        .contains(&NotifyType::DiscoveryStarted)));
}
