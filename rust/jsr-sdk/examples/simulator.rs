//! Drives a pulser-receiver that the SDK's diagnostics plugin simulates, so
//! it needs no hardware: select it, read its settings, change them and watch
//! the status events.
//!
//!     cargo run --example simulator
use jsr_sdk::{Manager, ReceiverMode, Result};
use std::thread::sleep;
use std::time::{Duration, Instant};

const PLUGIN: &str = "JSRDotNET_DiagLib";

fn main() -> Result<()> {
    let mut manager = Manager::new()?;
    manager.load_plugins_from_binary_dir()?;

    // Manage only the simulator plugin, without its windows, and let it
    // connect to its simulated devices as soon as it opens.
    manager.add_managed_plugin(PLUGIN)?;
    manager.add_plugin_open_option(PLUGIN, "SHOW_SIMULATOR_WINDOWS", "NO")?;
    manager.add_plugin_open_option(PLUGIN, "CONNECT_ON_OPEN", "YES")?;

    // Status events arrive on an SDK thread; printing is all this one does.
    manager.set_status_callback(|event| {
        println!(
            "  event: {} is now {}",
            event.pulser_property, event.new_value
        );
    })?;

    // Discover, then select model "SIM", serial "00", channel 0.
    manager.set_discovery_enable(true)?;
    let deadline = Instant::now() + Duration::from_secs(20);
    while let Err(error) = manager.set_current_pulser_receiver_by_model("SIM", "00", 0) {
        if Instant::now() > deadline {
            return Err(error);
        }
        sleep(Duration::from_millis(100));
    }
    let id = manager.get_id()?;
    println!(
        "Selected {} {}, channel {}",
        id.instrument_id.model_name, id.instrument_id.serial_num, id.pulser_receiver_index
    );

    // Gain: an index into a list of values in dB.
    let gains = manager.get_gain_values()?;
    println!(
        "{} gains, from {} to {} dB",
        gains.len(),
        gains.first().unwrap_or(&0.0),
        gains.last().unwrap_or(&0.0)
    );
    println!(
        "Gain index {} = {} dB",
        manager.get_gain_index()?,
        manager.get_gain()?
    );
    println!("Setting gain index 5:");
    manager.set_gain_index(5)?;
    println!("Gain is now {} dB", manager.get_gain()?);

    // Receiver mode: an enum.
    println!("Receiver mode: {:?}", manager.get_receiver_mode()?);
    println!("Setting receiver mode Thru:");
    manager.set_receiver_mode(ReceiverMode::Thru)?;
    println!("Receiver mode: {:?}", manager.get_receiver_mode()?);

    // A setting the device rejects comes back as an error, not a panic.
    if let Err(error) = manager.set_gain_index(10_000) {
        println!("Gain index 10000 was rejected: {error}");
    }

    // Dropping the manager removes the callback and frees the manager.
    Ok(())
}
