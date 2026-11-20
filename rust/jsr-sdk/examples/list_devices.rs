//! Lists the plugins and the pulser-receivers connected to this computer.
//!
//!     cargo run --example list_devices
//!
//! Without a device attached it lists the plugins and reports that no
//! pulser-receiver was found.
use jsr_sdk::{Manager, Result};
use std::thread::sleep;
use std::time::{Duration, Instant};

/// The SDK's simulator plugin; this example looks for real devices only.
const SIMULATOR_PLUGIN: &str = "JSRDotNET_DiagLib";

fn main() -> Result<()> {
    println!("JSR-SDK {}", jsr_sdk::library_version()?);

    let manager = Manager::new()?;
    // The plugins are the DLLs next to JSR-SDK.dll.
    manager.load_plugins_from_binary_dir()?;

    // Discovery only searches *managed* plugins, and none are by default.
    println!("Plugins:");
    for plugin in manager.get_plugin_names()? {
        println!("  {plugin}");
        if plugin != SIMULATOR_PLUGIN {
            manager.add_managed_plugin(&plugin)?;
        }
    }

    // Discovery runs in the background; give it a few seconds.
    manager.set_discovery_enable(true)?;
    let deadline = Instant::now() + Duration::from_secs(10);
    let mut found = Vec::new();
    while found.is_empty() && Instant::now() < deadline {
        sleep(Duration::from_millis(500));
        found = manager.get_pulser_receivers()?;
    }

    if found.is_empty() {
        println!("No pulser-receiver found.");
    }
    for id in &found {
        let instrument = &id.instrument_id;
        println!(
            "Found {} (serial {}), channel {}, through {}",
            instrument.model_name,
            instrument.serial_num,
            id.pulser_receiver_index,
            instrument.plugin_name
        );
    }
    Ok(())
}
