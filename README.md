# JSR SDK
This project provides a wrapper around the JSRdotNETSDK that allows you to use it in other programming languages than C# through a C++/CLI wrapper.

### ⚠️⚠️ This project is **not** developed, endorsed, maintained, or otherwise associated with **JSR/BYK/Imaginant**⚠️⚠️


## Usage
Select a release from the github, and copy the download url and hash. This adds the library, and provides headers. It needs CMake 3.21 or newer.
```CMake
include(FetchContent)

set(JSR_SDK_VERSION "v<version>")
FetchContent_Declare(
    JSR-SDK
    URL      "https://github.com/obin1000/JSR-SDK/releases/download/${JSR_SDK_VERSION}/JSR-SDK-${JSR_SDK_VERSION}.zip"
    URL_HASH SHA256=<hash>
)

FetchContent_MakeAvailable(JSR-SDK)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE JSR-SDK)
# Copy JSR-SDK.dll, JSRDotNETSDK.dll and the plugin DLLs next to my_app.
jsr_sdk_copy_runtime(my_app)
```

#### Or with find_package
Unpack the release zip and point CMake at it, for example with `-DCMAKE_PREFIX_PATH=C:/libs/JSR-SDK-v<version>`:
```CMake
find_package(JSR-SDK 0.14 CONFIG REQUIRED)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE JSR-SDK::JSR-SDK)
jsr_sdk_copy_runtime(my_app)
```
A version request accepts releases with the same major version that aren't older. Before 1.0 the minor version has to match too, because a 0.x minor release may break the API. Set the `JSR_SDK_INCLUDE_*` plugin options before `find_package()` to change them.

### Supported compilers
| Compiler | `jsr_sdk.hpp` (C++) | `jsr_sdk.h` (C) | `JSRSDKManager.hpp` (old C++ interface) |
|---|---|---|---|
| MSVC, x64 and x86 | ✅ | ✅ | ✅ |
| clang-cl | ✅ | ✅ | ✅ |
| MinGW-w64 GCC, x64 and x86 | ✅ | ✅ | ❌ |
| MinGW clang | ✅ | ✅ | ❌ |

CI builds and runs a small program with each of these on every pull request. MinGW builds link `lib/<arch>/dynamic/libJSR-SDK.dll.a`, which the release zip includes. Since 1.0 the SDK is only a DLL: releases before it also had a static library (`JSR_SDK_USE_DYNAMIC_LIB OFF`), and setting that option now only gives a warning.

### C++
Include `JSR-SDK/jsr_sdk.hpp` and create a `jsr::Manager`. It frees the manager when it goes out of scope, returns `std::string` and `std::vector`, and throws `jsr::Error` (with `code()` and `what()`) when a call fails. It needs C++17.
```cpp
#include "JSR-SDK/jsr_sdk.hpp"

#include <iostream>

int main() {
  try {
    jsr::Manager manager;
    manager.loadPluginsFromBinaryDir();

    for (const auto &plugin : manager.GetPluginNames()) {
      std::cout << "Found plugin: " << plugin << "\n";
    }

    manager.replaceNotifyEventHandler([](const jsr::NotifyEvent &event) {
      // May run on a library thread: keep it short and thread-safe.
      std::cout << "Notify: " << event.model << " " << event.serial << "\n";
    });
  } catch (const jsr::Error &e) {
    std::cerr << "JSR-SDK error " << e.code() << ": " << e.what() << "\n";
    return 1;
  }
  return 0;
}
```

### C
`JSR-SDK/jsr_sdk.h` is the C API underneath `jsr_sdk.hpp`, for C programs or other languages. Every function returns `JSR_OK` (0) or a negative `JSR_ERROR_*` code, and `JSR_GetLastErrorMessage()` explains the last failure on the thread. Results go into buffers you provide; the header describes the conventions.
```c
#include "JSR-SDK/jsr_sdk.h"

#include <stdio.h>

int main(void) {
  JSR_Manager *manager = NULL;
  char names[8][JSR_STRING_LENGTH];
  char *buffers[8];
  size_t count = 0;
  size_t i;

  if (JSR_Create(&manager) != JSR_OK ||
      JSR_LoadPluginsFromBinaryDir(manager) != JSR_OK) {
    char message[512];
    JSR_GetLastErrorMessage(message, sizeof message);
    fprintf(stderr, "JSR-SDK error: %s\n", message);
    JSR_Destroy(manager);
    return 1;
  }

  for (i = 0; i < 8; ++i) {
    buffers[i] = names[i];
  }
  JSR_GetPluginNames(manager, buffers, 8, JSR_STRING_LENGTH, &count);
  for (i = 0; i < count && i < 8; ++i) {
    printf("Found plugin: %s\n", names[i]);
  }

  JSR_Destroy(manager);
  return 0;
}
```

### The old C++ interface
`JSR-SDK/JSRSDKManager.hpp` (`CreateJSRSDKManager()`) is superseded by `jsr_sdk.hpp`, and stays for existing users. It is an abstract C++ class, so it only works with MSVC and clang-cl; other compilers get a compile error. `jsr::Manager` uses the same method names, so moving over is mostly a matter of replacing `CreateJSRSDKManager()` with a `jsr::Manager` and dropping the buffer arguments.

### Run-time requirements
- .NET Framework 4.8 and the Microsoft Visual C++ runtime (x64 or x86).
- `JSR-SDK.dll`, `JSRDotNETSDK.dll` and the plugin DLLs next to the program; `jsr_sdk_copy_runtime(<target>)` copies them after each build.

### Versions
A program loads whichever `JSR-SDK.dll` sits next to it, so the DLL can come from another release than the headers the program was built with.
- `JSR_API_VERSION` in `jsr_sdk.h` is the API version of the headers, and `JSR_GetApiVersion()` returns the one of the loaded DLL. `jsr::Manager` compares them and throws a `jsr::Error` with `JSR_ERROR_VERSION_MISMATCH` when they differ. C programs should make the same check before `JSR_Create()`.
- `JSR_GetLibraryVersion()` (`jsr::libraryVersion()`) returns the release, such as `0.13.0`, for messages and logs.
- The API version only goes up for an incompatible change. New functions and enum values keep it, so a newer DLL runs programs built for an older release with the same API version.
- Releases before 0.13.0 don't have these functions. A program built with 0.13.0 or later won't start with such a DLL: Windows reports that `JSR_GetApiVersion` is missing.
- [CHANGELOG.md](CHANGELOG.md) lists what changed in each release, breaking changes first.

### Threading and events
These rules hold for every interface (`jsr_sdk.h`, `jsr_sdk.hpp` and `JSRSDKManager.hpp`).
- **One thread at a time per manager.** A manager's calls are not synchronized: use it from one thread, or serialize the calls yourself. Separate managers are independent.
- **Events can arrive inside your own calls.** The SDK raises the event for a change it makes inside the call that made it. For example, `setGainIndex()` calls the status-change callback before it returns. That call can come from another thread while yours is still inside `setGainIndex()`.
- **Other events arrive on library threads.** Discovery events, for example, come from threads owned by the .NET runtime, not the thread that registered the callback.
- **Callbacks must be thread-safe, return quickly and not throw.** Exceptions a callback throws are dropped: the SDK raised the event inside one of its own calls, and the exception would otherwise make that call fail. `jsr::Manager` drops them with every compiler. A C callback passed to `jsr_sdk.h` directly can only rely on that when it is built with MSVC or clang-cl: the library can't catch the exceptions of MinGW compilers, so such a callback must catch its own.
- **A crash ends the process.** An access violation or another structured exception, in a callback or inside the library, is not caught or turned into an error code. After a crash memory may be corrupt, so carrying on would hide the bug and keep driving the pulser-receiver in an unknown state. It also lets page heap, Application Verifier and AddressSanitizer report memory errors where they happen.
- **Callbacks of one kind never overlap.** Status callbacks run one at a time, and so do notify callbacks.
- **Replacing or removing a callback waits for a running one.** Once the call returns, the old callback isn't running and won't be called again, so you may free whatever it uses. Because of that wait, a callback must not block on a thread that is replacing callbacks.


## Development
The library is a C++/CLI project for Visual Studio (`JSR-SDK.vcxproj`); building it needs MSVC and .NET Framework 4.8. The tests are a CMake project in `test/` that runs against the built library, and fetches googletest itself.

```
msbuild JSR-SDK.vcxproj -p:Configuration=dynamic -p:Platform=x64
cmake -S test -B build/test-x64 -A x64
cmake --build build/test-x64 --config Release
ctest --test-dir build/test-x64 -C Release --output-on-failure
```
Use `-p:Platform=Win32` and `-A Win32` for x86.
- `clr_tests` (`test/clr/`) tests the DLL's internals and compares the C API with the C++ interface. It is C++/CLI, so it needs MSVC and a Visual Studio generator.
- `capi_tests` (`test/capi/`) tests the C API and `jsr::Manager`. It is plain C++17; CI also builds and runs it with every supported compiler, through `ci/consumer`.
- Both run against simulated pulser-receivers from the SDK's diagnostics plugin, so no hardware is needed.

#### Hardware tests
The `*IntegrationTest` suites need a real pulser-receiver, so CI skips them. Run them by hand with a device attached:
```
build/test-x64/Release/clr_tests.exe --gtest_filter=*IntegrationTest.*
build/test-x64/capi/Release/capi_tests.exe --gtest_filter=*IntegrationTest.*
```
- They accept the lab's devices by default. To use others, set `JSR_TEST_DEVICE_SERIALS` to their serial numbers, separated by commas (`set JSR_TEST_DEVICE_SERIALS=JB0104,JC0100`).
- The **Hardware tests** workflow runs them too: start it from the Actions tab. It builds and tests the library with CI first. Then, on a self-hosted runner labelled `jsr-hardware` with a device attached, it runs both programs built with MSVC, and `capi_tests` built with MinGW GCC against the release layout. [hardware.yml](.github/workflows/hardware.yml) lists what the runner needs.

Code that is generated from `tools/gen_c_api` (the per-method parts of the C API and the parity tests) is regenerated with `python tools/gen_c_api/generate.py`; see [its README](tools/gen_c_api/README.md).

The C and C++ code follows `.clang-format`, and CI checks it. Format with `python tools/format.py`, or check with `python tools/format.py --check`. CI uses clang-format 23.1.4 (`pip install clang-format==23.1.4`); other versions may format a few lines differently.


## Disclaimer

This repository (“**JSR-SDK**”) is an **independent, community-driven wrapper** for the closed-source **JSRDotNETSDK**. It is **not** developed, endorsed, maintained, or otherwise associated with **JSR** or any of its subsidiaries or affiliates.

* **No Affiliation or Endorsement** – “JSR”, “JSRDotNETSDK”, and all related trademarks, service marks, and logos are the exclusive property of JSR. Their use here is **solely for identification purposes** and does **not** imply endorsement by, or any formal relationship with, JSR.  
* **No Distribution of Proprietary Code** – This repository does bundle (portion) of the proprietary JSRDotNETSDK. However, using this library means you must comply with JSR its license and EULA.  
* **License Independence** – The Project’s source code is released under the license stated in `LICENSE`. **Nothing** in this repository alters, overrides, or substitutes the license terms that apply to the JSRDotNETSDK itself.  
* **Reverse Engineering** – The Project neither encourages nor facilitates reverse engineering, de-obfuscation, or circumvention of the JSRDotNETSDK or any copy-protection mechanisms.  
* **Liability** – The Project is provided **“AS IS”** without warranty of any kind. The authors and maintainers assume **no liability** for any damages, losses, or legal issues arising from its use. Use the Project **at your own risk** and ensure you have the legal right to interface with the JSRDotNETSDK.

By using this Project you acknowledge that you have read, understood, and agree to all of the above terms.
