//! Callbacks that share state with the rest of the program: count the notify
//! events of a discovery run, collect status events, then replace and remove
//! the callbacks. Uses the SDK's simulated pulser-receiver, so it needs no
//! hardware.
//!
//!     cargo run --example events
//!
//! Callbacks run on the SDK's threads, so what they share must be thread-safe
//! (Arc with an atomic or a Mutex here). Keep them short: the SDK waits for a
//! status callback before the setter that raised it returns.
use jsr_sdk::{Manager, NotifyType, Result};
use std::sync::atomic::{AtomicUsize, Ordering};
use std::sync::{Arc, Mutex};
use std::thread::sleep;
use std::time::{Duration, Instant};

const PLUGIN: &str = "JSRDotNET_DiagLib";

fn main() -> Result<()> {
    let mut manager = Manager::new()?;
    manager.load_plugins_from_binary_dir()?;
    manager.add_managed_plugin(PLUGIN)?;
    manager.add_plugin_open_option(PLUGIN, "SHOW_SIMULATOR_WINDOWS", "NO")?;
    manager.add_plugin_open_option(PLUGIN, "CONNECT_ON_OPEN", "YES")?;

    // Notify events: the manager's news, such as a discovered device.
    let discovered = Arc::new(AtomicUsize::new(0));
    let counter = Arc::clone(&discovered);
    manager.set_notify_callback(move |event| {
        if event.notify_type == NotifyType::PulserRcvrDiscovered {
            counter.fetch_add(1, Ordering::SeqCst);
            println!("  discovered {} {}", event.model, event.serial);
        }
    })?;

    manager.set_discovery_enable(true)?;
    let deadline = Instant::now() + Duration::from_secs(20);
    while let Err(error) = manager.set_current_pulser_receiver_by_model("SIM", "00", 0) {
        if Instant::now() > deadline {
            return Err(error);
        }
        sleep(Duration::from_millis(100));
    }
    // The discovery events arrive on another thread, possibly after the
    // selection above succeeded: wait a moment for them.
    sleep(Duration::from_millis(500));
    println!(
        "{} channels discovered (each simulated instrument has two)",
        discovered.load(Ordering::SeqCst)
    );

    // Status events: changes of the selected pulser-receiver.
    let changes = Arc::new(Mutex::new(Vec::new()));
    let log = Arc::clone(&changes);
    manager.set_status_callback(move |event| {
        log.lock()
            .unwrap()
            .push(format!("{} = {}", event.pulser_property, event.new_value));
    })?;
    manager.set_gain_index(3)?;
    manager.set_gain_index(4)?;
    println!("Status events: {:?}", changes.lock().unwrap());

    // Replacing a callback: once this returns, the old one is neither running
    // nor called again, so whatever it captured can go.
    manager.set_status_callback(|event| {
        println!(
            "  new callback: {} = {}",
            event.pulser_property, event.new_value
        );
    })?;
    manager.set_gain_index(5)?;

    // Removing them.
    manager.remove_status_callback()?;
    manager.remove_notify_callback()?;
    manager.set_gain_index(6)?;
    println!("No callback printed the last change.");
    Ok(())
}
