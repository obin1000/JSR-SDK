//! Opens a real pulser-receiver, prints what it is and supports, changes a
//! setting and puts it back.
//!
//!     cargo run --example configure_device            (the first device found)
//!     cargo run --example configure_device -- JC0100  (a device by serial)
//!
//! It only changes settings the device reports as supported, and restores
//! them before it ends.
use jsr_sdk::{Manager, PulserReceiverId, ReceiverMode, Result};
use std::thread::sleep;
use std::time::{Duration, Instant};

const SIMULATOR_PLUGIN: &str = "JSRDotNET_DiagLib";

/// Discovers devices for up to 20 seconds and returns the first one with
/// the wanted serial number, or the first one at all.
fn find_device(manager: &Manager, serial: Option<&str>) -> Result<Option<PulserReceiverId>> {
    for plugin in manager.get_plugin_names()? {
        if plugin != SIMULATOR_PLUGIN {
            manager.add_managed_plugin(&plugin)?;
        }
    }
    manager.set_discovery_enable(true)?;
    let deadline = Instant::now() + Duration::from_secs(20);
    while Instant::now() < deadline {
        let found = manager
            .get_pulser_receivers()?
            .into_iter()
            .find(|id| serial.map_or(true, |s| id.instrument_id.serial_num == s));
        if found.is_some() {
            return Ok(found);
        }
        sleep(Duration::from_millis(500));
    }
    Ok(None)
}

fn main() -> Result<()> {
    let serial = std::env::args().nth(1);
    let manager = Manager::new()?;
    manager.load_plugins_from_binary_dir()?;

    let Some(id) = find_device(&manager, serial.as_deref())? else {
        println!(
            "No pulser-receiver found{}.",
            serial
                .map(|s| format!(" with serial {s}"))
                .unwrap_or_default()
        );
        return Ok(());
    };
    manager.set_current_pulser_receiver_by_id(&id)?;
    println!(
        "Opened {} {} through {}",
        id.instrument_id.model_name, id.instrument_id.serial_num, id.instrument_id.plugin_name
    );

    // What it is.
    for line in manager.get_info()? {
        if !line.is_empty() {
            println!("  {line}");
        }
    }

    // What it supports.
    let gain = manager.get_gain_index_supported()?;
    let thru = manager.get_receiver_mode_thru_supported()?;
    let echo = manager.get_receiver_mode_echo_supported()?;
    println!("Gain index supported: {gain}; receiver modes: thru {thru}, echo {echo}");
    if manager.get_hv_supply_supported()? {
        println!(
            "HV supply: {} V ({} to {} V)",
            manager.get_hv_supply()?,
            manager.get_hv_supply_min()?,
            manager.get_hv_supply_max()?
        );
    }

    // Change a setting, then put it back.
    if thru && echo {
        let original = manager.get_receiver_mode()?;
        let other = if original == ReceiverMode::Echo {
            ReceiverMode::Thru
        } else {
            ReceiverMode::Echo
        };
        manager.set_receiver_mode(other)?;
        println!(
            "Receiver mode {original:?} -> {:?}",
            manager.get_receiver_mode()?
        );
        manager.set_receiver_mode(original)?;
        println!(
            "Receiver mode restored to {:?}",
            manager.get_receiver_mode()?
        );
    } else if gain {
        let original = manager.get_gain_index()?;
        let other = if original == 0 { 1 } else { 0 };
        manager.set_gain_index(other)?;
        println!("Gain {} dB -> {} dB", original, manager.get_gain()?);
        manager.set_gain_index(original)?;
        println!("Gain index restored to {original}");
    }
    Ok(())
}
