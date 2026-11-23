//! Raw bindings to the C API of [JSR-SDK](https://github.com/obin1000/JSR-SDK)
//! (`jsr_sdk.h`), which drives JSR (Imaginant) pulser-receivers through the
//! closed-source JSRDotNETSDK.
//!
//! Most programs should use the safe `jsr-sdk` crate instead. These bindings
//! are generated from `jsr_sdk.h`, so the names, values and layouts are those
//! of the C API, and its documentation applies:
//!
//! - Every function returns `JSR_OK` (0) or a negative `JSR_ERROR_*` code;
//!   [`JSR_GetLastErrorMessage`] explains the last failure on the thread.
//! - Strings and lists go into buffers the caller owns; pass a null buffer to
//!   ask for the size.
//! - A manager is used from one thread at a time. Callbacks run on SDK
//!   threads, must return quickly and must not unwind.
//!
//! # The DLLs
//! `JSR-SDK.dll` is linked by name (`raw-dylib`), so no import library is
//! needed. At run time it, `JSRDotNETSDK.dll` and the plugin DLLs must be
//! next to the program or in a folder on `PATH`. The build script gets them
//! from a JSR-SDK release and puts them where `cargo run` and `cargo test`
//! find them; see the crate's README.
#![cfg_attr(not(windows), allow(unused))]
#![allow(non_camel_case_types, non_upper_case_globals, non_snake_case)]
#![no_std]

#[cfg(not(windows))]
compile_error!("JSR-SDK is Windows-only.");

mod bindings;
pub use bindings::*;

/// The JSR-SDK release whose DLLs the build script used, such as `1.0.0`,
/// or `local` when they came from `JSR_SDK_DIR`.
pub const SDK_RELEASE: &str = env!("JSR_SDK_RELEASE");
