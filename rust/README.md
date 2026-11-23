# JSR-SDK for Rust
Rust crates for [JSR-SDK](https://github.com/obin1000/JSR-SDK), which drives JSR (Imaginant) pulser-receivers through the closed-source JSRDotNETSDK.

| Crate | What it is |
|---|---|
| `jsr-sdk` | The API to use: a `Manager` with every SDK method, `Result` errors, Rust enums and closure callbacks. |
| `jsr-sdk-sys` | Raw bindings to the C API (`jsr_sdk.h`), generated from the header. `jsr-sdk` builds on it. |

### ⚠️ Not developed, endorsed, maintained, or otherwise associated with JSR/BYK/Imaginant. See the [disclaimer](../README.md#disclaimer).

## Requirements
- **Windows**, x64 or x86 (MSVC or GNU target).
- **.NET Framework 4.8** and the **Microsoft Visual C++ runtime**, as for every JSR-SDK program.
- **Rust 1.71** or newer.

## Using it
The crates aren't on crates.io yet; use them from this repository:
```toml
[dependencies]
jsr-sdk = { git = "https://github.com/obin1000/JSR-SDK" }
```

```rust
use jsr_sdk::Manager;

fn main() -> jsr_sdk::Result<()> {
    let mut manager = Manager::new()?;
    manager.load_plugins_from_binary_dir()?;

    // Discovery only searches managed plugins, and none are by default.
    for plugin in manager.get_plugin_names()? {
        if plugin != "JSRDotNET_DiagLib" {
            manager.add_managed_plugin(&plugin)?;
        }
    }

    // Runs on an SDK thread.
    manager.set_status_callback(|event| {
        println!("{} is now {}", event.pulser_property, event.new_value);
    })?;

    manager.set_discovery_enable(true)?;
    std::thread::sleep(std::time::Duration::from_secs(5));
    if let Some(id) = manager.get_pulser_receivers()?.first() {
        manager.set_current_pulser_receiver_by_id(id)?;
        println!("{} gains", manager.get_gain_values()?.len());
    }
    Ok(())
}
```

## The DLLs
The SDK is a set of DLLs: `JSR-SDK.dll`, the .NET SDK `JSRDotNETSDK.dll`, and the plugins that talk to the devices. Rust code can't be compiled into them, so the build script of `jsr-sdk-sys` gets them from a JSR-SDK release.

1. **Where they come from.** By default, the build script downloads the release it pins from this repository's GitHub releases. It checks the zip's SHA-256 and unpacks it with Windows' own `curl` and `tar`. To build without network access, or with your own build of the library, set `JSR_SDK_DIR` to an unpacked release or to a JSR-SDK checkout with the DLL built.
2. **While you develop.** The build script puts the DLLs where Cargo finds them: on `PATH` for `cargo run` and `cargo test`, and next to your programs and examples in `target/<profile>`.
3. **When you ship.** Put the DLLs next to your `.exe`, or in a folder on `PATH`. The build script exports the unpacked release's folder to your own build script as `DEP_JSR_SDK_ROOT`. The DLLs are in its `lib/<x64|Win32>/dynamic/` and `lib/plugins/`.

## How it maps to the SDK
- **Methods:** `Manager` has one method per SDK method, named after the C function in snake_case. `JSR_GetGainIndex` is `get_gain_index`, and `JSR_SetCurrentPulserReceiverByModel` is `set_current_pulser_receiver_by_model`. The [C API documentation](../include/JSR-SDK/jsr_sdk.h) describes each one.
- **Errors:** every method returns `jsr_sdk::Result<T>`. An `Error` has the SDK's `code()` (an `ErrorCode`) and `message()`.
- **Types:**
  - strings and lists come back as `String` and `Vec`;
  - IDs and events are plain Rust structs;
  - the SDK's enums are Rust enums, such as `ReceiverMode::Thru`. Each enum has an `Unrecognized(i32)` variant for values a newer DLL may add.
- **Versions:** `Manager::new()` checks that the loaded DLL implements the API version the crate was built for. `jsr_sdk::library_version()` tells you which release it is.

## Threads and callbacks
- **One thread at a time:** a `Manager` can move to another thread (`Send`) but not be shared (`!Sync`). The SDK allows one thread at a time per manager. To use it from several threads, put it in a `Mutex`.
- **Callbacks run on the SDK's threads:** so they must be `Send`, and whatever they share must be thread-safe (`Arc` with an atomic or a `Mutex`).
- **Keep callbacks short:** a setter such as `set_gain_index` raises its status event before it returns.
- **Callbacks of one kind never overlap.**
- **Replacing is safe:** once `set_status_callback` or `remove_status_callback` returns, the old callback isn't running and won't be called again. The same holds for the notify callback.
- **Panics are dropped:** a panic in a callback is caught and dropped, because it can't unwind into the SDK.

## Examples
`cargo run --example <name>` in this folder:

| Example | Shows | Hardware |
|---|---|---|
| `list_devices` | Plugins, managed plugins and discovery | Optional: lists what it finds |
| `simulator` | Selecting a device, reading and changing settings, status events, errors | No: the SDK's simulator |
| `events` | Notify and status callbacks with shared state; replacing and removing them | No: the SDK's simulator |
| `configure_device` | Opening a device by serial (`-- JC0100`), its info and capabilities, changing a setting and restoring it | Yes |

## Tests
- **No hardware needed:** `cargo test` runs the tests against the SDK's simulated pulser-receiver. In this repository, set `JSR_SDK_DIR` to the repository root after building the DLL, to test against your build.
- **With a device attached:** `cargo test --test hardware -- --ignored`. The tests accept the lab's devices; set `JSR_TEST_DEVICE_SERIALS` to your devices' serial numbers to use others.
- **Generated code:** the bindings and the per-method part of `jsr-sdk` are generated by `tools/gen_c_api` (`python tools/gen_c_api/generate.py`). Edit the generator, not the generated files.
