# JSR SDK
This project provides a wrapper around the JSRdotNETSDK that allows you to use it in other programming languages than C# through a C++/CLI wrapper.

### ⚠️⚠️ This project is **not** developed, endorsed, maintained, or otherwise associated with **JSR/BYK/Imaginant**⚠️⚠️


## Usage
Select a release from the github, and copy the download url and hash. This adds the library, and provides headers. It needs CMake 3.21 or newer.
```CMake
include(FetchContent)

# Tell the JSR-SDK to use the dynamic library instead of the static one.
SET(JSR_SDK_USE_DYNAMIC_LIB ON)
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

### Supported compilers
| Compiler | `jsr_sdk.hpp` (C++) | `jsr_sdk.h` (C) | `JSRSDKManager.hpp` (old C++ interface) |
|---|---|---|---|
| MSVC, x64 and x86 | ✅ | ✅ | ✅ |
| clang-cl | ✅ | ✅ | ✅ |
| MinGW-w64 GCC, x64 and x86 | ✅ | ✅ | ❌ |
| MinGW clang | ✅ | ✅ | ❌ |

CI builds and runs a small program with each of these on every pull request. MinGW builds need the dynamic library (`JSR_SDK_USE_DYNAMIC_LIB ON`); they link `lib/<arch>/dynamic/libJSR-SDK.dll.a`, which the release zip includes.

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

### Threading and events
These rules hold for every interface (`jsr_sdk.h`, `jsr_sdk.hpp` and `JSRSDKManager.hpp`).
- **One thread at a time per manager.** A manager's calls are not synchronized: use it from one thread, or serialize the calls yourself. Separate managers are independent.
- **Events can arrive inside your own calls.** The SDK raises the event for a change it makes inside the call that made it. For example, `setGainIndex()` calls the status-change callback on your thread before it returns.
- **Other events arrive on library threads.** Discovery events, for example, come from threads owned by the .NET runtime, not the thread that registered the callback.
- **Callbacks must be thread-safe, return quickly and not throw.** Exceptions a callback throws are dropped: the SDK raised the event inside one of its own calls, and the exception would otherwise make that call fail.
- **Callbacks of one kind never overlap.** Status callbacks run one at a time, and so do notify callbacks.
- **Replacing or removing a callback waits for a running one.** Once the call returns, the old callback isn't running and won't be called again, so you may free whatever it uses. Because of that wait, a callback must not block on a thread that is replacing callbacks.


## Disclaimer

This repository (“**JSR-SDK**”) is an **independent, community-driven wrapper** for the closed-source **JSRDotNETSDK**. It is **not** developed, endorsed, maintained, or otherwise associated with **JSR** or any of its subsidiaries or affiliates.

* **No Affiliation or Endorsement** – “JSR”, “JSRDotNETSDK”, and all related trademarks, service marks, and logos are the exclusive property of JSR. Their use here is **solely for identification purposes** and does **not** imply endorsement by, or any formal relationship with, JSR.  
* **No Distribution of Proprietary Code** – This repository does bundle (portion) of the proprietary JSRDotNETSDK. However, using this library means you must comply with JSR its license and EULA.  
* **License Independence** – The Project’s source code is released under the license stated in `LICENSE`. **Nothing** in this repository alters, overrides, or substitutes the license terms that apply to the JSRDotNETSDK itself.  
* **Reverse Engineering** – The Project neither encourages nor facilitates reverse engineering, de-obfuscation, or circumvention of the JSRDotNETSDK or any copy-protection mechanisms.  
* **Liability** – The Project is provided **“AS IS”** without warranty of any kind. The authors and maintainers assume **no liability** for any damages, losses, or legal issues arising from its use. Use the Project **at your own risk** and ensure you have the legal right to interface with the JSRDotNETSDK.

By using this Project you acknowledge that you have read, understood, and agree to all of the above terms.
