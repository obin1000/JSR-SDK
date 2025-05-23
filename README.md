# JSR SDK
This project provides a wrapper around the JSRdotNETSDK that allows you to use it in other programming languages than C# through a C++/CLI wrapper.

### ⚠️⚠️ This project is **not** developed, endorsed, maintained, or otherwise associated with **JSR/BYK/Imaginant**⚠️⚠️


## Usage
Select a release from the github, and copy the download url and hash. This adds the library, and provides headers.
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


## Disclaimer

This repository (“**JSR-SDK**”) is an **independent, community-driven wrapper** for the closed-source **JSRDotNETSDK**. It is **not** developed, endorsed, maintained, or otherwise associated with **JSR** or any of its subsidiaries or affiliates.

* **No Affiliation or Endorsement** – “JSR”, “JSRDotNETSDK”, and all related trademarks, service marks, and logos are the exclusive property of JSR. Their use here is **solely for identification purposes** and does **not** imply endorsement by, or any formal relationship with, JSR.  
* **No Distribution of Proprietary Code** – This repository does bundle (portion) of the proprietary JSRDotNETSDK. However, using this library means you must comply with JSR its license and EULA.  
* **License Independence** – The Project’s source code is released under the license stated in `LICENSE`. **Nothing** in this repository alters, overrides, or substitutes the license terms that apply to the JSRDotNETSDK itself.  
* **Reverse Engineering** – The Project neither encourages nor facilitates reverse engineering, de-obfuscation, or circumvention of the JSRDotNETSDK or any copy-protection mechanisms.  
* **Liability** – The Project is provided **“AS IS”** without warranty of any kind. The authors and maintainers assume **no liability** for any damages, losses, or legal issues arising from its use. Use the Project **at your own risk** and ensure you have the legal right to interface with the JSRDotNETSDK.

By using this Project you acknowledge that you have read, understood, and agree to all of the above terms.
