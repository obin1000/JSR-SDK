#include "JSR-SDK/JSRSDKManager.h"
#include <msclr/marshal_cppstd.h>
#include <vcclr.h>  // Needed for gcroot

using namespace System;
using namespace JSRDotNETSDK;
using namespace msclr::interop;

class JSRSDKManagerWrapper : public JSRSDKManager {
public:
    JSRSDKManagerWrapper() {
        m_manager = gcnew JSRDotNETManager("");
    }

    ~JSRSDKManagerWrapper() override {
        if (static_cast<JSRDotNETManager^>(m_manager) != nullptr) {
            m_manager->Shutdown();
            m_manager = nullptr;
        }
    }
    int LoadPlugins(const std::string& pluginPath) override {
        String^ path = marshal_as<String^>(pluginPath);
        Console::WriteLine("Reading plugins from: " + path);
        try {
            m_manager->LoadPlugins(path);
        }
        catch (System::Exception^ exception) {
            std::string msg = msclr::interop::marshal_as<std::string>(exception->Message);
            throw msg;
        }
        return GetNumberOfPlugins();
    }

    int GetNumberOfPlugins() override {
		return m_manager->GetPluginNames()->Length;
    }

    std::vector<std::string> GetPluginNames() override {
        std::vector<std::string> result;
        auto names = m_manager->GetPluginNames();
        for each (String ^ name in names) {
            result.push_back(marshal_as<std::string>(name));
        }
        return result;
    }

private:
    gcroot<JSRDotNETManager^> m_manager;  // This is the correct way inside a native class
};


extern "C" {

    __declspec(dllexport) JSRSDKManager* CreateJSRSDKManager() {
        try {
            return new JSRSDKManagerWrapper();
        }
        catch (System::Exception^ exception) {
            std::string msg = msclr::interop::marshal_as<std::string>(exception->Message);
            throw "Failed creating JSR Bridge object: " + msg;
        }
    }

    __declspec(dllexport) void DestroyJSRSDKManager(JSRSDKManager* bridge) {
        delete bridge;
    }
}
