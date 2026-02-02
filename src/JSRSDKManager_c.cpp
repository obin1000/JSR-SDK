#include "JSR-SDK/JSRSDKManager.h"
#include "JSR-SDK/JSRSDKManager.hpp"
#include "JSR-SDK/marshals/EventConverters.h"

#include <cstring>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <mutex>


// ============================================================================
// Helper Functions
// ============================================================================

static int copyStringToBuffer(const std::string &s, char *buf, int bufSize) {
  int required = static_cast<int>(s.size()) + 1;
  if (buf == nullptr || bufSize <= 0)
    return required;
  int toCopy = std::min(bufSize - 1, static_cast<int>(s.size()));
  memcpy(buf, s.c_str(), toCopy);
  buf[toCopy] = '\0';
  return required;
}

static int copyJSRStringToBuffer(const JSRString &s, char *buf, int bufSize) {
  return copyStringToBuffer(std::string(s.data), buf, bufSize);
}

// ============================================================================
// Callback Management - Thread-Safe, Copy-Based Design
// ============================================================================

static std::mutex g_cbMutex;

struct CallbackHolder {
  JSR_StatusChangeCallback statusCb = nullptr;
  void *statusUser = nullptr;
  JSR_NotifyCallback notifyCb = nullptr;
  void *notifyUser = nullptr;
};

static std::unordered_map<JSRSDKManagerHandle, CallbackHolder> g_callbacks;

// ============================================================================
// Manager Lifecycle
// ============================================================================

JSRSDKManagerHandle JSR_CreateManager() {
  JSRSDKManager *m = CreateJSRSDKManager();
  return reinterpret_cast<JSRSDKManagerHandle>(m);
}

void JSR_DestroyManager(JSRSDKManagerHandle mgr) {
  if (!mgr) return;
  
  // First, remove callbacks to prevent any pending callbacks from firing
  JSR_RemoveStatusChangeCallback(mgr);
  JSR_RemoveNotifyCallback(mgr);
  
  // Clean up callback holder
  {
    std::lock_guard<std::mutex> lk(g_cbMutex);
    g_callbacks.erase(mgr);
  }
  
  // Finally, destroy the manager
  JSRSDKManager *m = reinterpret_cast<JSRSDKManager *>(mgr);
  DestroyJSRSDKManager(m);
}

// ============================================================================
// Callbacks - FIXED: Copy events to prevent dangling references
// ============================================================================

int JSR_SetStatusChangeCallback(JSRSDKManagerHandle mgr, JSR_StatusChangeCallback cb, void *user_data) {
  if (!mgr) return -1;
  
  try {
    auto mcpp = reinterpret_cast<JSRSDKManager *>(mgr);
    
    if (cb) {
      // Store callback info
      {
        std::lock_guard<std::mutex> lk(g_cbMutex);
        auto &h = g_callbacks[mgr];
        h.statusCb = cb;
        h.statusUser = user_data;
      }
      
      // Create wrapper that COPIES the event to avoid dangling references
      StatusChangeCallback wrapper = [mgr](const StatusChangedEvent &evt) {
        // CRITICAL: Make a COPY of the event on our stack
        // This prevents dangling references if the original is destroyed
        StatusChangedEvent evtCopy = evt;
        
        // Now safely access the callback
        JSR_StatusChangeCallback callback = nullptr;
        void* userData = nullptr;
        
        {
          std::lock_guard<std::mutex> lk(g_cbMutex);
          auto it = g_callbacks.find(mgr);
          if (it == g_callbacks.end() || !it->second.statusCb) return;
          
          callback = it->second.statusCb;
          userData = it->second.statusUser;
        }
        
        // Call user callback with pointer to OUR copy
        // This is safe because evtCopy is on our stack
        if (callback) {
          callback(&evtCopy, userData);
        }
      };
      
      mcpp->replaceStatusChangeEventHandler(wrapper);
    } else {
      // Clear callback
      {
        std::lock_guard<std::mutex> lk(g_cbMutex);
        auto it = g_callbacks.find(mgr);
        if (it != g_callbacks.end()) {
          it->second.statusCb = nullptr;
          it->second.statusUser = nullptr;
        }
      }
      mcpp->removeStatusChangeEventHandler();
    }
    return 0;
  } catch(...) {
    return -1;
  }
}

int JSR_RemoveStatusChangeCallback(JSRSDKManagerHandle mgr) {
  return JSR_SetStatusChangeCallback(mgr, nullptr, nullptr);
}

int JSR_SetNotifyCallback(JSRSDKManagerHandle mgr, JSR_NotifyCallback cb, void *user_data) {
  if (!mgr) return -1;
  
  try {
    auto mcpp = reinterpret_cast<JSRSDKManager *>(mgr);
    
    if (cb) {
      // Store callback info
      {
        std::lock_guard<std::mutex> lk(g_cbMutex);
        auto &h = g_callbacks[mgr];
        h.notifyCb = cb;
        h.notifyUser = user_data;
      }
      
      // Create wrapper that COPIES the event to avoid dangling references
      NotifyCallback wrapper = [mgr](const NotifyEvent &evt) {
        // CRITICAL: Make a COPY of the event (~12KB on stack)
        // This is safe because:
        // 1. Stack can handle 12KB easily (default is 1MB+)
        // 2. Prevents dangling references
        // 3. No dynamic allocation needed
        NotifyEvent evtCopy = evt;
        
        // Now safely access the callback
        JSR_NotifyCallback callback = nullptr;
        void* userData = nullptr;
        
        {
          std::lock_guard<std::mutex> lk(g_cbMutex);
          auto it = g_callbacks.find(mgr);
          if (it == g_callbacks.end() || !it->second.notifyCb) return;
          
          callback = it->second.notifyCb;
          userData = it->second.notifyUser;
        }
        
        // Call user callback with pointer to OUR copy
        if (callback) {
          callback(&evtCopy, userData);
        }
      };
      
      mcpp->replaceNotifyEventHandler(wrapper);
    } else {
      // Clear callback
      {
        std::lock_guard<std::mutex> lk(g_cbMutex);
        auto it = g_callbacks.find(mgr);
        if (it != g_callbacks.end()) {
          it->second.notifyCb = nullptr;
          it->second.notifyUser = nullptr;
        }
      }
      mcpp->removeNotifyEventHandler();
    }
    return 0;
  } catch(...) {
    return -1;
  }
}

int JSR_RemoveNotifyCallback(JSRSDKManagerHandle mgr) {
  return JSR_SetNotifyCallback(mgr, nullptr, nullptr);
}

// ============================================================================
// Vector<String> Helpers - Simple Pattern
// ============================================================================

int JSR_GetPluginNamesCount(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return static_cast<int>(m->GetPluginNames().size());
  } catch(...) {
    return -1;
  }
}

int JSR_GetPluginName(JSRSDKManagerHandle mgr, int index, char *outBuf, int bufSize) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    auto v = m->GetPluginNames();
    if (index < 0 || index >= static_cast<int>(v.size()))
      return -2;
    return copyStringToBuffer(v[index], outBuf, bufSize);
  } catch(...) {
    return -1;
  }
}

int JSR_GetManagedPluginNamesCount(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return static_cast<int>(m->GetManagedPluginNames().size());
  } catch(...) {
    return -1;
  }
}

int JSR_GetManagedPluginName(JSRSDKManagerHandle mgr, int index, char *outBuf, int bufSize) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    auto v = m->GetManagedPluginNames();
    if (index < 0 || index >= static_cast<int>(v.size()))
      return -2;
    return copyStringToBuffer(v[index], outBuf, bufSize);
  } catch(...) {
    return -1;
  }
}

int JSR_GetCustomSettingsCount(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return static_cast<int>(m->GetCustomSettings().size());
  } catch(...) {
    return -1;
  }
}

int JSR_GetCustomSetting(JSRSDKManagerHandle mgr, int index, char *outBuf, int bufSize) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    auto v = m->GetCustomSettings();
    if (index < 0 || index >= static_cast<int>(v.size()))
      return -2;
    return copyStringToBuffer(v[index], outBuf, bufSize);
  } catch(...) {
    return -1;
  }
}

// ============================================================================
// Plugin-Specific Vector<String> Functions
// ============================================================================

int JSR_GetOpenOptionNamesCount(JSRSDKManagerHandle mgr, const char *plugin) {
  if (!mgr || !plugin) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return static_cast<int>(m->GetOpenOptionNames(std::string(plugin)).size());
  } catch(...) {
    return -1;
  }
}

int JSR_GetOpenOptionName(JSRSDKManagerHandle mgr, const char *plugin, int index, char *outBuf, int bufSize) {
  if (!mgr || !plugin) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    auto v = m->GetOpenOptionNames(std::string(plugin));
    if (index < 0 || index >= static_cast<int>(v.size()))
      return -2;
    return copyStringToBuffer(v[index], outBuf, bufSize);
  } catch(...) {
    return -1;
  }
}

int JSR_GetPortsToExcludeCount(JSRSDKManagerHandle mgr, const char *plugin) {
  if (!mgr || !plugin) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return static_cast<int>(m->GetPortsToExclude(std::string(plugin)).size());
  } catch(...) {
    return -1;
  }
}

int JSR_GetPortToExclude(JSRSDKManagerHandle mgr, const char *plugin, int index, char *outBuf, int bufSize) {
  if (!mgr || !plugin) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    auto v = m->GetPortsToExclude(std::string(plugin));
    if (index < 0 || index >= static_cast<int>(v.size()))
      return -2;
    return copyStringToBuffer(v[index], outBuf, bufSize);
  } catch(...) {
    return -1;
  }
}

int JSR_GetPortsToIncludeCount(JSRSDKManagerHandle mgr, const char *plugin) {
  if (!mgr || !plugin) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return static_cast<int>(m->GetPortsToInclude(std::string(plugin)).size());
  } catch(...) {
    return -1;
  }
}

int JSR_GetPortToInclude(JSRSDKManagerHandle mgr, const char *plugin, int index, char *outBuf, int bufSize) {
  if (!mgr || !plugin) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    auto v = m->GetPortsToInclude(std::string(plugin));
    if (index < 0 || index >= static_cast<int>(v.size()))
      return -2;
    return copyStringToBuffer(v[index], outBuf, bufSize);
  } catch(...) {
    return -1;
  }
}

// ============================================================================
// Instruments (Complex Struct)
// ============================================================================

int JSR_GetInstrumentsCount(JSRSDKManagerHandle mgr, const char *plugin) {
  if (!mgr || !plugin) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return static_cast<int>(m->GetInstruments(std::string(plugin)).size());
  } catch(...) {
    return -1;
  }
}

int JSR_GetInstrumentModelName(JSRSDKManagerHandle mgr, const char *plugin, int index, char *outBuf, int bufSize) {
  if (!mgr || !plugin) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    auto v = m->GetInstruments(std::string(plugin));
    if (index < 0 || index >= static_cast<int>(v.size()))
      return -2;
    return copyJSRStringToBuffer(v[index].ModelName, outBuf, bufSize);
  } catch(...) {
    return -1;
  }
}

int JSR_GetInstrumentSerialNum(JSRSDKManagerHandle mgr, const char *plugin, int index, char *outBuf, int bufSize) {
  if (!mgr || !plugin) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    auto v = m->GetInstruments(std::string(plugin));
    if (index < 0 || index >= static_cast<int>(v.size()))
      return -2;
    return copyJSRStringToBuffer(v[index].SerialNum, outBuf, bufSize);
  } catch(...) {
    return -1;
  }
}

int JSR_GetInstrumentPort(JSRSDKManagerHandle mgr, const char *plugin, int index, char *outBuf, int bufSize) {
  if (!mgr || !plugin) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    auto v = m->GetInstruments(std::string(plugin));
    if (index < 0 || index >= static_cast<int>(v.size()))
      return -2;
    return copyJSRStringToBuffer(v[index].Port, outBuf, bufSize);
  } catch(...) {
    return -1;
  }
}

int JSR_GetInstrumentPluginName(JSRSDKManagerHandle mgr, const char *plugin, int index, char *outBuf, int bufSize) {
  if (!mgr || !plugin) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    auto v = m->GetInstruments(std::string(plugin));
    if (index < 0 || index >= static_cast<int>(v.size()))
      return -2;
    return copyJSRStringToBuffer(v[index].PluginName, outBuf, bufSize);
  } catch(...) {
    return -1;
  }
}

// ============================================================================
// PulserReceiver Info
// ============================================================================

int JSR_GetPulserReceiverInfoCount(JSRSDKManagerHandle mgr, const char *model, const char *serial, int idxPR) {
  if (!mgr || !model || !serial) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return static_cast<int>(m->GetPulserReceiverInfo(std::string(model), std::string(serial), idxPR).size());
  } catch(...) {
    return -1;
  }
}

int JSR_GetPulserReceiverInfoItem(JSRSDKManagerHandle mgr, const char *model, const char *serial, int idxPR, int index, char *outBuf, int bufSize) {
  if (!mgr || !model || !serial) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    auto v = m->GetPulserReceiverInfo(std::string(model), std::string(serial), idxPR);
    if (index < 0 || index >= static_cast<int>(v.size()))
      return -2;
    return copyStringToBuffer(v[index], outBuf, bufSize);
  } catch(...) {
    return -1;
  }
}

// ============================================================================
// PulserReceivers List
// ============================================================================

int JSR_GetPulserReceiversCount(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return static_cast<int>(m->GetPulserReceivers().size());
  } catch(...) {
    return -1;
  }
}

int JSR_GetPulserReceiverInstrumentModelName(JSRSDKManagerHandle mgr, int index, char *outBuf, int bufSize) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    auto v = m->GetPulserReceivers();
    if (index < 0 || index >= static_cast<int>(v.size()))
      return -2;
    return copyJSRStringToBuffer(v[index].InstrumentId.ModelName, outBuf, bufSize);
  } catch(...) {
    return -1;
  }
}

int JSR_GetPulserReceiverInstrumentSerialNum(JSRSDKManagerHandle mgr, int index, char *outBuf, int bufSize) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    auto v = m->GetPulserReceivers();
    if (index < 0 || index >= static_cast<int>(v.size()))
      return -2;
    return copyJSRStringToBuffer(v[index].InstrumentId.SerialNum, outBuf, bufSize);
  } catch(...) {
    return -1;
  }
}

int JSR_GetPulserReceiverIndex(JSRSDKManagerHandle mgr, int index) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    auto v = m->GetPulserReceivers();
    if (index < 0 || index >= static_cast<int>(v.size()))
      return -2;
    return v[index].PulserReceiverIndex;
  } catch(...) {
    return -1;
  }
}

// ============================================================================
// Vector<Double> Functions
// ============================================================================

int JSR_GetPulserMaxPRFsCount(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return static_cast<int>(m->getPulserMaxPRFs().size());
  } catch(...) {
    return -1;
  }
}

double JSR_GetPulserMaxPRF(JSRSDKManagerHandle mgr, int index) {
  if (!mgr) return -1.0;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    auto v = m->getPulserMaxPRFs();
    if (index < 0 || index >= static_cast<int>(v.size()))
      return -1.0;
    return v[index];
  } catch(...) {
    return -1.0;
  }
}

int JSR_GetPulseRepetitionFrequencyValuesCount(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return static_cast<int>(m->getPulseRepetitionFrequencyValues().size());
  } catch(...) {
    return -1;
  }
}

double JSR_GetPulseRepetitionFrequencyValue(JSRSDKManagerHandle mgr, int index) {
  if (!mgr) return -1.0;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    auto v = m->getPulseRepetitionFrequencyValues();
    if (index < 0 || index >= static_cast<int>(v.size()))
      return -1.0;
    return v[index];
  } catch(...) {
    return -1.0;
  }
}

// ============================================================================
// Binary Data (OEM Data)
// ============================================================================

int JSR_GetReceiverOEMDataSize(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return static_cast<int>(m->getReceiverOEMData().size());
  } catch(...) {
    return -1;
  }
}

int JSR_CopyReceiverOEMData(JSRSDKManagerHandle mgr, unsigned char *dest, int destSize) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    auto v = m->getReceiverOEMData();
    int required = static_cast<int>(v.size());
    if (dest == nullptr || destSize <= 0)
      return required;
    int toCopy = std::min(destSize, required);
    memcpy(dest, v.data(), toCopy);
    return toCopy;
  } catch(...) {
    return -1;
  }
}

int JSR_GetPulserOEMDataSize(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return static_cast<int>(m->getPulserOEMData().size());
  } catch(...) {
    return -1;
  }
}

int JSR_CopyPulserOEMData(JSRSDKManagerHandle mgr, unsigned char *dest, int destSize) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    auto v = m->getPulserOEMData();
    int required = static_cast<int>(v.size());
    if (dest == nullptr || destSize <= 0)
      return required;
    int toCopy = std::min(destSize, required);
    memcpy(dest, v.data(), toCopy);
    return toCopy;
  } catch(...) {
    return -1;
  }
}

// ============================================================================
// Action Functions (Void Return)
// ============================================================================

int JSR_AddManagedPlugin(JSRSDKManagerHandle mgr, const char *pluginName) {
  if (!mgr || !pluginName) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    m->AddManagedPlugin(std::string(pluginName));
    return 0;
  } catch(...) {
    return -1;
  }
}

int JSR_AddPluginOpenOption(JSRSDKManagerHandle mgr, const char *pluginName, const char *optionName, const char *optionValue) {
  if (!mgr || !pluginName || !optionName || !optionValue) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    m->AddPluginOpenOption(std::string(pluginName), std::string(optionName), std::string(optionValue));
    return 0;
  } catch(...) {
    return -1;
  }
}

int JSR_AddPluginType(JSRSDKManagerHandle mgr, const char *pluginType) {
  if (!mgr || !pluginType) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    m->AddPluginType(std::string(pluginType));
    return 0;
  } catch(...) {
    return -1;
  }
}

int JSR_ForceDetach(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    m->ForceDetach();
    return 0;
  } catch(...) {
    return -1;
  }
}

int JSR_LoadPlugins(JSRSDKManagerHandle mgr, const char *pluginPath) {
  if (!mgr || !pluginPath) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    m->LoadPlugins(std::string(pluginPath));
    return 0;
  } catch(...) {
    return -1;
  }
}

int JSR_SetDiscoveryEnable(JSRSDKManagerHandle mgr, int enable) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    m->SetDiscoveryEnable(enable != 0);
    return 0;
  } catch(...) {
    return -1;
  }
}

int JSR_SetCurrentPulserReceiverByModel(JSRSDKManagerHandle mgr, const char *model, const char *serial, int idxPR) {
  if (!mgr || !model || !serial) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    m->SetCurrentPulserReceiver(std::string(model), std::string(serial), idxPR);
    return 0;
  } catch(...) {
    return -1;
  }
}

int JSR_SetPulserPropertyValue(JSRSDKManagerHandle mgr, const char *propName, const char *value) {
  if (!mgr || !propName || !value) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    m->SetPulserPropertyValue(std::string(propName), std::string(value));
    return 0;
  } catch(...) {
    return -1;
  }
}

int JSR_SetPulserPropertyValueWithRole(JSRSDKManagerHandle mgr, const char *propName, int role, const char *value) {
  if (!mgr || !propName || !value) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    m->SetPulserPropertyValue(std::string(propName), static_cast<C_PULSER_PROPERTY_ROLE>(role), std::string(value));
    return 0;
  } catch(...) {
    return -1;
  }
}

int JSR_Shutdown(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    m->Shutdown();
    return 0;
  } catch(...) {
    return -1;
  }
}

// ============================================================================
// String Getters/Setters
// ============================================================================

int JSR_GetPluginPath(JSRSDKManagerHandle mgr, char *outBuf, int bufSize) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return copyStringToBuffer(m->getPluginPath(), outBuf, bufSize);
  } catch(...) {
    return -1;
  }
}

int JSR_SetPluginPath(JSRSDKManagerHandle mgr, const char *path) {
  if (!mgr || !path) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    m->setPluginPath(std::string(path));
    return 0;
  } catch(...) {
    return -1;
  }
}

int JSR_GetUnitModelName(JSRSDKManagerHandle mgr, char *outBuf, int bufSize) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return copyStringToBuffer(m->getUnitModelName(), outBuf, bufSize);
  } catch(...) {
    return -1;
  }
}

int JSR_GetUnitSerialNum(JSRSDKManagerHandle mgr, char *outBuf, int bufSize) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return copyStringToBuffer(m->getUnitSerialNum(), outBuf, bufSize);
  } catch(...) {
    return -1;
  }
}

int JSR_GetPulserModelName(JSRSDKManagerHandle mgr, char *outBuf, int bufSize) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return copyStringToBuffer(m->getPulserModelName(), outBuf, bufSize);
  } catch(...) {
    return -1;
  }
}

int JSR_GetPulserSerialNum(JSRSDKManagerHandle mgr, char *outBuf, int bufSize) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return copyStringToBuffer(m->getPulserSerialNum(), outBuf, bufSize);
  } catch(...) {
    return -1;
  }
}

int JSR_GetReceiverModelName(JSRSDKManagerHandle mgr, char *outBuf, int bufSize) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return copyStringToBuffer(m->getReceiverModelName(), outBuf, bufSize);
  } catch(...) {
    return -1;
  }
}

int JSR_GetReceiverSerialNum(JSRSDKManagerHandle mgr, char *outBuf, int bufSize) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return copyStringToBuffer(m->getReceiverSerialNum(), outBuf, bufSize);
  } catch(...) {
    return -1;
  }
}

int JSR_GetManagerState(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return static_cast<int>(m->getManagerState());
  } catch(...) {
    return -1;
  }
}

// ============================================================================
// Device Control & Configuration
// ============================================================================

int JSR_LoadPluginsFromBinaryDir(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    m->loadPluginsFromBinaryDir();
    return 0;
  } catch(...) {
    return -1;
  }
}

int JSR_AddPortToExclude(JSRSDKManagerHandle mgr, const char *plugin, const char *port) {
  if (!mgr || !plugin || !port) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    m->AddPortToExclude(std::string(plugin), std::string(port));
    return 0;
  } catch(...) {
    return -1;
  }
}

int JSR_AddPortToInclude(JSRSDKManagerHandle mgr, const char *plugin, const char *port) {
  if (!mgr || !plugin || !port) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    m->AddPortToInclude(std::string(plugin), std::string(port));
    return 0;
  } catch(...) {
    return -1;
  }
}

int JSR_SetCurrentPulserReceiverByID(JSRSDKManagerHandle mgr, const char *modelName, const char *serialNum, int prIndex) {
  if (!mgr || !modelName || !serialNum) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    PulserReceiverID prID;
    PulserReceiverID_Init(&prID);
    JSRString_Set(&prID.InstrumentId.ModelName, modelName);
    JSRString_Set(&prID.InstrumentId.SerialNum, serialNum);
    prID.PulserReceiverIndex = prIndex;
    m->SetCurrentPulserReceiver(prID);
    return 0;
  } catch(...) {
    return -1;
  }
}

int JSR_SetCurrentPulserReceiver(JSRSDKManagerHandle mgr, const PulserReceiverID *prID) {
  if (!mgr || !prID) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    m->SetCurrentPulserReceiver(*prID);
    return 0;
  } catch(...) {
    return -1;
  }
}

// ============================================================================
// Trigger Configuration
// ============================================================================

int JSR_SetTriggerEnable(JSRSDKManagerHandle mgr, int enable) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    m->setTriggerEnable(enable != 0);
    return 0;
  } catch(...) {
    return -1;
  }
}

int JSR_GetTriggerEnable(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return m->getTriggerEnable() ? 1 : 0;
  } catch(...) {
    return -1;
  }
}

int JSR_SetTriggerSource(JSRSDKManagerHandle mgr, int source) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    m->setTriggerSource(static_cast<C_TRIGGER_SOURCE>(source));
    return 0;
  } catch(...) {
    return -1;
  }
}

int JSR_GetTriggerSource(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return static_cast<int>(m->getTriggerSource());
  } catch(...) {
    return -1;
  }
}

int JSR_SetTriggerImpedance(JSRSDKManagerHandle mgr, int impedance) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    m->setTriggerImpedance(static_cast<C_TRIGGER_IMPEDANCE>(impedance));
    return 0;
  } catch(...) {
    return -1;
  }
}

int JSR_GetTriggerImpedance(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return static_cast<int>(m->getTriggerImpedance());
  } catch(...) {
    return -1;
  }
}

// ============================================================================
// Receiver Configuration
// ============================================================================

int JSR_SetReceiverMode(JSRSDKManagerHandle mgr, int mode) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    m->setReceiverMode(static_cast<C_RECEIVER_MODE>(mode));
    return 0;
  } catch(...) {
    return -1;
  }
}

int JSR_GetReceiverMode(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return static_cast<int>(m->getReceiverMode());
  } catch(...) {
    return -1;
  }
}

int JSR_SetGain(JSRSDKManagerHandle mgr, double gain) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    m->setGain(gain);
    return 0;
  } catch(...) {
    return -1;
  }
}

double JSR_GetGain(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1.0;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return m->getGain();
  } catch(...) {
    return -1.0;
  }
}

double JSR_GetGainMin(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1.0;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return m->getGainMin();
  } catch(...) {
    return -1.0;
  }
}

double JSR_GetGainMax(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1.0;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return m->getGainMax();
  } catch(...) {
    return -1.0;
  }
}

// ============================================================================
// Pulse Repetition Frequency Configuration
// ============================================================================

int JSR_SetPulseRepetitionFrequency(JSRSDKManagerHandle mgr, double frequency) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    m->setPulseRepetitionFrequency(frequency);
    return 0;
  } catch(...) {
    return -1;
  }
}

double JSR_GetPulseRepetitionFrequency(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1.0;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return m->getPulseRepetitionFrequency();
  } catch(...) {
    return -1.0;
  }
}

double JSR_GetPulseRepetitionFrequencyMin(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1.0;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return m->getPulseRepetitionFrequencyMin();
  } catch(...) {
    return -1.0;
  }
}

double JSR_GetPulseRepetitionFrequencyMax(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1.0;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return m->getPulseRepetitionFrequencyMax();
  } catch(...) {
    return -1.0;
  }
}

// ============================================================================
// Filter Configuration
// ============================================================================

int JSR_SetLowPassFilterIndex(JSRSDKManagerHandle mgr, int index) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    m->setLowPassFilterIndex(index);
    return 0;
  } catch(...) {
    return -1;
  }
}

int JSR_GetLowPassFilterIndex(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return m->getLowPassFilterIndex();
  } catch(...) {
    return -1;
  }
}

int JSR_GetLowPassFilterIndexMax(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return m->getLowPassFilterIndexMax();
  } catch(...) {
    return -1;
  }
}

int JSR_SetHighPassFilterIndex(JSRSDKManagerHandle mgr, int index) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    m->setHighPassFilterIndex(index);
    return 0;
  } catch(...) {
    return -1;
  }
}

int JSR_GetHighPassFilterIndex(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return m->getHighPassFilterIndex();
  } catch(...) {
    return -1;
  }
}

int JSR_GetHighPassFilterIndexMax(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return m->getHighPassFilterIndexMax();
  } catch(...) {
    return -1;
  }
}

// ============================================================================
// Damping Configuration
// ============================================================================

int JSR_SetDampingIndex(JSRSDKManagerHandle mgr, int index) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    m->setDampingIndex(index);
    return 0;
  } catch(...) {
    return -1;
  }
}

int JSR_GetDampingIndex(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return m->getDampingIndex();
  } catch(...) {
    return -1;
  }
}

int JSR_GetDampingIndexMax(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    return m->getDampingIndexMax();
  } catch(...) {
    return -1;
  }
}

