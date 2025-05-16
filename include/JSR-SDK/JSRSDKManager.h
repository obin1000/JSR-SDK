#pragma once

#include <string>
#include <vector>

// This abstract class is the interface to unmanaged C++.
// The extension will contain C++/CLI code, which the unmanaged C++ codo will know nothing about.
class JSRSDKManager {
public:
	// Closes the connection to the devices and releases all resources.
    virtual ~JSRSDKManager() = default;
	// Load the plugins from the specified path.
    virtual int LoadPlugins(const std::string& pluginPath) = 0;
	// Gives the number of loaded plugins.
    virtual int GetNumberOfPlugins() = 0;
	// Gives the names of the loaded plugins.
    virtual std::vector<std::string> GetPluginNames() = 0;
};

extern "C" {
    // Factory function to create the manager
    __declspec(dllexport) JSRSDKManager* CreateJSRSDKManager();

    // Explicitly destroy the manager
    __declspec(dllexport) void DestroyJSRSDKManager(JSRSDKManager* bridge);
}
