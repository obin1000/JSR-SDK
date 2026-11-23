//! Tests against a real pulser-receiver. CI skips them (`#[ignore]`); run
//! them with a device attached:
//!
//!     cargo test --test hardware -- --ignored
//!
//! They accept the lab's devices (JB0104, JB0118, JC0100). Set
//! `JSR_TEST_DEVICE_SERIALS` to a comma-separated list of serial numbers to
//! use others, as for the C and C++ integration tests (test/common/TestDevices.h).
use jsr_sdk::{Manager, PulserReceiverId, ReceiverMode, Result};
use std::sync::atomic::{AtomicUsize, Ordering};
use std::sync::{Arc, Mutex, MutexGuard};
use std::thread::sleep;
use std::time::{Duration, Instant};

const DEFAULT_SERIALS: &str = "JB0104,JB0118,JC0100";
const SIMULATOR_PLUGIN: &str = "JSRDotNET_DiagLib";

/// One test at a time: they all open the same device.
fn device_lock() -> MutexGuard<'static, ()> {
    static LOCK: Mutex<()> = Mutex::new(());
    LOCK.lock().unwrap_or_else(|poisoned| poisoned.into_inner())
}

fn accepted_serials() -> Vec<String> {
    let list = std::env::var("JSR_TEST_DEVICE_SERIALS").unwrap_or_default();
    let serials: Vec<String> = list
        .split(',')
        .map(str::trim)
        .filter(|s| !s.is_empty())
        .map(String::from)
        .collect();
    if serials.is_empty() {
        DEFAULT_SERIALS.split(',').map(String::from).collect()
    } else {
        serials
    }
}

/// A manager with every hardware plugin managed (discovery only searches
/// managed plugins) and an accepted device selected.
fn open_device() -> (Manager, PulserReceiverId) {
    let manager = Manager::new().unwrap();
    manager.load_plugins_from_binary_dir().unwrap();
    for plugin in manager.get_plugin_names().unwrap() {
        if plugin != SIMULATOR_PLUGIN {
            manager.add_managed_plugin(&plugin).unwrap();
        }
    }
    manager.set_discovery_enable(true).unwrap();
    let accepted = accepted_serials();
    let deadline = Instant::now() + Duration::from_secs(20);
    while Instant::now() < deadline {
        let found = manager
            .get_pulser_receivers()
            .unwrap()
            .into_iter()
            .find(|id| {
                id.instrument_id.model_name != "SIM"
                    && accepted.contains(&id.instrument_id.serial_num)
            });
        if let Some(id) = found {
            manager.set_current_pulser_receiver_by_id(&id).unwrap();
            return (manager, id);
        }
        sleep(Duration::from_millis(500));
    }
    panic!(
        "No test device connected: the hardware tests need one of {}. Set \
         JSR_TEST_DEVICE_SERIALS to the serial numbers of your devices to use others.",
        accepted.join(", ")
    );
}

/// Changes one setting of the device.
type Change = Box<dyn Fn(&Manager) -> Result<()>>;

/// A setting to switch back and forth so the device raises status events:
/// the receiver mode, or else the gain index. Returns (set other, restore).
fn toggle(manager: &Manager) -> Result<Option<(Change, Change)>> {
    if manager.get_receiver_mode_thru_supported()? && manager.get_receiver_mode_echo_supported()? {
        let original = manager.get_receiver_mode()?;
        let other = if original == ReceiverMode::Echo {
            ReceiverMode::Thru
        } else {
            ReceiverMode::Echo
        };
        return Ok(Some((
            Box::new(move |m: &Manager| m.set_receiver_mode(other)),
            Box::new(move |m: &Manager| m.set_receiver_mode(original)),
        )));
    }
    if manager.get_gain_index_supported()? {
        let original = manager.get_gain_index()?;
        let other = if original == 0 { 1 } else { 0 };
        return Ok(Some((
            Box::new(move |m: &Manager| m.set_gain_index(other)),
            Box::new(move |m: &Manager| m.set_gain_index(original)),
        )));
    }
    Ok(None)
}

#[test]
#[ignore = "needs a real pulser-receiver"]
fn opens_a_test_device() {
    let _lock = device_lock();
    let (manager, id) = open_device();
    println!(
        "opened {} {}",
        id.instrument_id.model_name, id.instrument_id.serial_num
    );
    assert!(manager.get_is_pulser_receiver_selected().unwrap());
    assert_eq!(
        manager.get_id().unwrap().instrument_id.serial_num,
        id.instrument_id.serial_num
    );
    assert!(manager
        .get_info()
        .unwrap()
        .iter()
        .any(|line| !line.is_empty()));
}

#[test]
#[ignore = "needs a real pulser-receiver"]
fn a_setting_round_trips_and_raises_a_status_event() {
    let _lock = device_lock();
    let (mut manager, _) = open_device();
    let Some((set_other, restore)) = toggle(&manager).unwrap() else {
        println!("skipped: neither receiver mode nor gain index can be changed");
        return;
    };
    let events = Arc::new(AtomicUsize::new(0));
    let count = Arc::clone(&events);
    manager
        .set_status_callback(move |_| {
            count.fetch_add(1, Ordering::SeqCst);
        })
        .unwrap();
    set_other(&manager).unwrap();
    restore(&manager).unwrap();
    let deadline = Instant::now() + Duration::from_secs(5);
    while events.load(Ordering::SeqCst) == 0 && Instant::now() < deadline {
        sleep(Duration::from_millis(100));
    }
    assert!(events.load(Ordering::SeqCst) > 0, "no status event");
}

/// #13: a dropped manager releases the device, so the next one opens it.
#[test]
#[ignore = "needs a real pulser-receiver"]
fn a_recreated_manager_reopens_the_device() {
    let _lock = device_lock();
    for round in 1..=2 {
        let (manager, id) = open_device();
        println!("round {round}: opened {}", id.instrument_id.serial_num);
        assert!(!manager.get_info().unwrap().is_empty());
        drop(manager);
    }
}

/// #14: replacing callbacks while the device raises events, from SDK
/// threads. A replaced callback must never run again.
#[test]
#[ignore = "needs a real pulser-receiver"]
fn replacing_callbacks_while_the_device_raises_events_is_safe() {
    let _lock = device_lock();
    let (mut manager, _) = open_device();
    let current = Arc::new(AtomicUsize::new(0));
    let calls = Arc::new(AtomicUsize::new(0));
    let stale = Arc::new(AtomicUsize::new(0));
    let changes = toggle(&manager).unwrap();

    let end = Instant::now() + Duration::from_secs(10);
    let mut generation = 0;
    let mut next_change = Instant::now();
    let mut next_discovery = Instant::now();
    while Instant::now() < end {
        generation += 1;
        current.store(generation, Ordering::SeqCst);
        // Each callback knows its generation; a call from an older one is stale.
        let (cur, cnt, stl) = (Arc::clone(&current), Arc::clone(&calls), Arc::clone(&stale));
        manager
            .set_status_callback(move |_| {
                cnt.fetch_add(1, Ordering::SeqCst);
                if cur.load(Ordering::SeqCst) != generation {
                    stl.fetch_add(1, Ordering::SeqCst);
                }
            })
            .unwrap();
        let (cur, cnt, stl) = (Arc::clone(&current), Arc::clone(&calls), Arc::clone(&stale));
        manager
            .set_notify_callback(move |_| {
                cnt.fetch_add(1, Ordering::SeqCst);
                if cur.load(Ordering::SeqCst) != generation {
                    stl.fetch_add(1, Ordering::SeqCst);
                }
            })
            .unwrap();
        if let Some((set_other, restore)) = &changes {
            if Instant::now() >= next_change {
                if generation % 2 == 0 {
                    set_other(&manager)
                } else {
                    restore(&manager)
                }
                .unwrap();
                next_change = Instant::now() + Duration::from_millis(20);
            }
        }
        if Instant::now() >= next_discovery {
            manager.set_discovery_enable(false).unwrap();
            manager.set_discovery_enable(true).unwrap();
            next_discovery = Instant::now() + Duration::from_millis(500);
        }
    }
    manager.remove_status_callback().unwrap();
    manager.remove_notify_callback().unwrap();
    if let Some((_, restore)) = &changes {
        restore(&manager).unwrap();
    }
    println!(
        "{generation} replacements, {} calls",
        calls.load(Ordering::SeqCst)
    );
    assert_eq!(
        stale.load(Ordering::SeqCst),
        0,
        "a replaced callback ran again (#14)"
    );
    assert!(
        calls.load(Ordering::SeqCst) > 0,
        "the device raised no events"
    );
}
