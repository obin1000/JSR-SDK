# JSR SDK
This project provides a wrapper around the JSRdotNETSDK that allows you to use it in other programming languages than C# through a C++/CLI wrapper.



## Usage
Select a release from the github, and copy the download url and hash. This adds the library, and provides headers.
```CMake
include(FetchContent)

# Tell the JSR-SDK to use the dynamic library instead of the static one.
SET(JSR_SDK_USE_DYNAMIC_LIB ON)
FetchContent_Declare(
    JSR-SDK
    URL      "https://github.com/obin1000/JSR-SDK/releases/download/v<version>/JSR-SDK-v<version>.zip"
    URL_HASH SHA256=<hash>
)

FetchContent_MakeAvailable(JSR-SDK)
```
In C++, include the header and create the JSR Manager object.
``` cpp
#include "JSR-SDK/JSRSDKManager.h"

 int hello_world() {
    try {
      std::cout << "Hello, World!\n";
      auto bridge = CreateJSRSDKManager();
      if (!bridge) {
        std::cerr << "Failed to create JSRBridge\n";
        return 1;
      }

      auto plugins = bridge->GetPluginNames();
      for (const auto &plugin : plugins) {
        std::cout << "Found Plugin: " << plugin << "\n";
      }

      std::cout << "Done! Loaded " << bridge->GetNumberOfPlugins() << " plugins\n";
      DestroyJSRSDKManager(bridge);

    } catch (const std::exception &e) {
      return 1;
    }
    return 0;
}
```

