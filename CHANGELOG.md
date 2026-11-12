# Changelog
Notable changes per release. Changes that can break a build or change behaviour come first in each release. Releases before 0.12.0 are on the [releases page](https://github.com/obin1000/JSR-SDK/releases).

## 1.0.0-rc.1 (2026-11-12)
1.0 makes the C API the SDK's only binary interface. From 1.0 on, the 1.x releases keep `jsr_sdk.h` and `jsr_sdk.hpp` compatible: they only add things. A program built with any 1.x release runs with the `JSR-SDK.dll` of any later 1.x release. `JSR_API_VERSION` stays 1, so programs built with 0.13 against `jsr_sdk.h` or `jsr_sdk.hpp` run with the 1.0 DLL too. The README's "Moving from JSRSDKManager.hpp" section shows how to move over.

### Changes that can break your build
- **The old C++ interface is gone** (#48). `JSR-SDK/JSRSDKManager.hpp`, `CreateJSRSDKManager()` and its `C_*` enums and C++ structs were removed; use `jsr_sdk.hpp` (or `jsr_sdk.h`). `include/JSR-SDK/` now holds only those two headers, and `JSR-SDK.dll` exports only the `JSR_*` functions.
- **The static library is gone** (#47). The SDK is only a DLL. `JSR_SDK_USE_DYNAMIC_LIB OFF` is still accepted, with a warning.

### Behaviour changes
- **A crash ends the process** (#36). An access violation or another structured exception, in a callback or inside the library, is no longer caught. Before, a crash in a callback was dropped, and a crash inside a call returned `JSR_ERROR_UNKNOWN`. Carrying on after a crash hid the bug and kept driving the device with possibly corrupt memory.
- **MinGW C callbacks must catch their own exceptions** (#36). The library can only catch the C++ exceptions of MSVC and clang-cl. An exception thrown by a C callback built with MinGW x64 now makes the SDK call that raised the event fail; it used to be caught as if it were a crash. `jsr::Manager` still drops callback exceptions with every compiler.

### Changed
- **`jsr::Manager` methods are camelCase** (#45): `getPluginNames()`, `setDiscoveryEnable()`, `getPulserReceivers()`, `shutdown()` and so on. The .NET SDK's PascalCase names still work through 1.x, with a deprecation warning that names the replacement.

### Added
- **`find_package(JSR-SDK)`** for an unpacked release, with the target `JSR-SDK::JSR-SDK` and a version check (#42). `add_subdirectory()` and `FetchContent` work as before.
- **Configurable hardware tests** (#19). `JSR_TEST_DEVICE_SERIALS` selects the devices the integration tests accept, and the manual **Hardware tests** workflow runs them on a self-hosted runner. It includes `capi_tests` built with MinGW GCC against the release layout.
- **Hardware checks for #13 and #14** (#38): a device must reopen after its manager was destroyed, and callbacks can be replaced safely while a device raises events.

### Fixed
- The docs said `setGainIndex()` calls the status-change callback on your thread. It calls it before it returns, but from another thread (#38).

### Development
- CI runs the simulator tests under full page heap, which turns a use-after-free into a crash where it happens (#24, #41).
- CI checks the formatting of the C and C++ code; `python tools/format.py` formats it (#21).
- `clr_tests` compiles the DLL's implementation itself, so the parity test still compares the C API with it (#48).

## 0.13.0 (2026-11-12)
### Changes that can break your build
- **The internal headers are gone from `include/`** (#27). `JSRSDKWrapper.hpp` and `marshals/*` were C++/CLI internals of the DLL. Use `jsr_sdk.hpp`, `jsr_sdk.h` or `JSRSDKManager.hpp`.
- **Invalid enum values are rejected** (#31). A value outside an enum used to fall back to a real setting; it now fails with `JSR_ERROR_INVALID_ARGUMENT` (`jsr::Error` in C++). Combined flag values are converted bit by bit instead of becoming `NONE`.
- **`C_POWER_LIMIT::UNKNOWN` was added** (#31). Switches over `C_POWER_LIMIT` may need a new case.

### Added
- **Run-time version check** (#20, #37). `JSR_API_VERSION`, `JSR_GetApiVersion()` and `JSR_GetLibraryVersion()` (`jsr::apiVersion()`, `jsr::libraryVersion()`). `jsr::Manager` throws `JSR_ERROR_VERSION_MISMATCH` when the loaded `JSR-SDK.dll` implements another API version than its headers. A program built with 0.13.0 or later needs a `JSR-SDK.dll` of 0.13.0 or later.
- **`jsr_sdk_copy_runtime()` and `$<TARGET_RUNTIME_DLLS>` work with every compiler** (#33).

### Fixed
- `getId()` without a selected device, and event values that depended on the machine's regional settings (#31).
- Misleading comments in the public C++ headers (#32).

### Development
- The C API generator is in `tools/gen_c_api`, and CI checks the generated code (#27).
- The C API tests run with all 7 supported compilers, and the unit tests on x86 too (#34).
- Regression tests for the shutdown hang (#13) and the callback race (#14) (#35).

## 0.12.0 (2026-11-11)
### Added
- **A C API (`jsr_sdk.h`) and a header-only C++ wrapper (`jsr_sdk.hpp`, `jsr::Manager`)**, so MinGW-w64 GCC, MinGW clang and other compilers can use the SDK (#10, #12). The old `JSRSDKManager.hpp` interface stays for MSVC and clang-cl.
- **SDK error codes are kept** in the exceptions of the old interface, and returned by the C API (#11).
- **Status-change callbacks get the event's data**; they used to receive an empty event (#11).

### Fixed
- Destroying a manager now shuts the .NET manager down, so its threads no longer keep a .NET program from exiting (#13).
- Callbacks can be replaced safely while events arrive, and exceptions from callbacks no longer make SDK calls fail (#14).

### Development
- CI on every pull request: the library for x64 and x86, unit tests, an export check, and programs built against the release layout with MSVC, clang-cl, MinGW GCC and MinGW clang (#11, #12). A release is exactly what CI built and tested.
