# JSR-SDK examples

Small, commented programs that show how to use JSR-SDK from C and C++. Each one builds against a release zip with `find_package(JSR-SDK)` and runs without hardware, so you can try them before a pulser-receiver is connected. CI builds and runs them with MSVC (x64 and x86), clang-cl, MinGW-w64 GCC (x64 and x86) and MinGW clang.

| Program | API | What it shows |
| --- | --- | --- |
| [`c/list_devices.c`](c/list_devices.c) | C, `jsr_sdk.h` | Checking the API version, loading plugins, managing them for discovery, and listing the pulser-receivers found. Checks every return code and prints `JSR_GetLastErrorMessage()`. |
| [`c/simulator.c`](c/simulator.c) | C, `jsr_sdk.h` | Selecting a simulated pulser-receiver, reading and changing its gain index and receiver mode, and printing the status events this raises. |
| [`cpp/list_devices.cpp`](cpp/list_devices.cpp) | C++17, `jsr_sdk.hpp` | The same as `list_devices.c` with `jsr::Manager`, which throws `jsr::Error` instead of returning error codes. |
| [`cpp/simulator.cpp`](cpp/simulator.cpp) | C++17, `jsr_sdk.hpp` | The same as `simulator.c` with `jsr::Manager`. |
| [`cpp/events.cpp`](cpp/events.cpp) | C++17, `jsr_sdk.hpp` | Status and notify callbacks that share state through a mutex, and replacing and removing them. |

## Building and running

You need CMake 3.21 or newer, a compiler from the list above, and a JSR-SDK release.

1. Download `JSR-SDK-<version>.zip` from the [releases](https://github.com/obin1000/JSR-SDK/releases) and unpack it, for example to `C:/libs/JSR-SDK-<version>`. It contains `include/`, `lib/` and `cmake/`.
2. Configure this folder with `CMAKE_PREFIX_PATH` pointing at the unpacked release, then build:

   ```powershell
   cmake -S examples -B build -DCMAKE_PREFIX_PATH=C:/libs/JSR-SDK-<version>
   ```

   ```powershell
   cmake --build build --config Release
   ```

   Add `-A Win32` to the first command for a 32-bit build with Visual Studio. With MinGW, or from a Developer PowerShell with Ninja, add `-G Ninja -DCMAKE_BUILD_TYPE=Release` instead.
3. Run a program from `build/Release` (`build` with Ninja), for example:

   ```powershell
   .\build\Release\cpp_simulator.exe
   ```

   Or run all of them with CTest:

   ```powershell
   ctest --test-dir build -C Release --output-on-failure
   ```

The build copies `JSR-SDK.dll`, `JSRDotNETSDK.dll` and the plugin DLLs next to each program (`jsr_sdk_copy_runtime()` in [CMakeLists.txt](CMakeLists.txt)). The programs load the plugins from there, so keep those files together when you copy a program elsewhere. The machine needs .NET Framework 4.8 and the Microsoft Visual C++ runtime.

To build against your own build of this repository instead of a release, point `CMAKE_PREFIX_PATH` at the repository root after building `JSR-SDK.vcxproj` (configuration `dynamic`).

## What to know

### Discovery only searches managed plugins

`JSR_LoadPluginsFromBinaryDir()` (`loadPluginsFromBinaryDir()`) loads the plugins, but discovery only searches the plugins you add with `JSR_AddManagedPlugin()` (`addManagedPlugin()`), and none are managed by default. Add them before `JSR_SetDiscoveryEnable()`, or nothing is found. `list_devices` manages every loaded plugin except the simulator, `JSRDotNET_DiagLib`.

Discovery runs in the background: poll `JSR_GetPulserReceivers()` (`getPulserReceivers()`), or watch for notify events, until your device shows up. The `list_devices` programs wait up to 20 seconds and exit 0 with a message when they find nothing.

### The simulator

The diagnostics plugin `JSRDotNET_DiagLib` simulates pulser-receivers, so a program can be developed and tested without hardware. The `simulator` and `events` programs manage only that plugin, set the open options `SHOW_SIMULATOR_WINDOWS=NO` (no simulator windows) and `CONNECT_ON_OPEN=YES`, and select model `SIM`, serial `00`, channel 0. Code that drives real hardware is the same; only the plugin, model and serial number differ.

### Errors

Every C function returns `JSR_OK` (0) or a negative `JSR_ERROR_*` code. After a failure, `JSR_GetLastErrorMessage()` returns the SDK's message for the last failed call on the same thread. `jsr::Manager` turns these into a `jsr::Error` exception, with the code in `code()` and the message in `what()`.

### Callbacks

- Status and notify callbacks run on threads of the SDK, or inside your own call: a setter such as `setGainIndex()` raises its status event before it returns. Guard state they share with the rest of the program with a mutex, as `events.cpp` does.
- Callbacks must return quickly and must not throw. `jsr::Manager` catches and drops an exception, and the event with it. With the C API, the library only catches C++ exceptions from MSVC and clang-cl code, so catch them inside the callback, and never `longjmp` out of one.
- Replacing or removing a callback waits for a running one to return, and after that the old one is never called again, so its data may then be freed. Don't hold a lock that a callback takes while you replace or remove callbacks.
- Keep what a callback uses alive until the callback is removed. `JSR_Destroy()` and the `jsr::Manager` destructor remove the callbacks.

See the comments in [`jsr_sdk.h`](../include/JSR-SDK/jsr_sdk.h) for the full rules.
