#include "JSR-SDK/JSRSDKManager.hpp"
#include "JSR-SDK/JSRSDKWrapper.hpp"
#include "JSR-SDK/marshals/MarshalTypes.h"

#include <msclr/marshal.h>
#include <msclr/marshal_cppstd.h>
#include <stdexcept>
#include <vcclr.h>

using namespace System;
using namespace JSRDotNETSDK;
using namespace msclr::interop;
using namespace System::Reflection;
using namespace System::IO;
using namespace System::Collections::Generic;

class JSRSDKManagerAdapter : public JSRSDKManager {
private:
  gcroot<JSRSDKWrapper ^> m_manager;

  std::unique_ptr<StatusChangeCallback> m_statusHolder;
  std::unique_ptr<NotifyCallback> m_notifyHolder;

public:
  JSRSDKManagerAdapter() {
    try {
      m_manager = gcnew JSRSDKWrapper();
    } catch (System::Exception ^ exception) {
      std::string msg =
          msclr::interop::marshal_as<std::string>(exception->Message);
      throw std::runtime_error("Failed creating JSR wrapper object: " + msg);
    }
  }

  ~JSRSDKManagerAdapter() override {
    removeStatusChangeEventHandler();
    removeNotifyEventHandler();
  }

  // === Custom functions added ===
  void loadPluginsFromBinaryDir() override {
    String ^ binaryDir = "";
    try {
      String ^ executablePath = Assembly::GetExecutingAssembly()->Location;
      binaryDir = Path::GetDirectoryName(executablePath);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("Failed finding the binary directory: " + msg);
    }
    try {
      m_manager->dotNETManager->LoadPlugins(binaryDir);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("Failed loading plugins from binary dir: " +
                               msg);
    }
  }

  // === Event handlers used for callbacks ===
  void
  replaceStatusChangeEventHandler(const StatusChangeCallback &cb) override {
    removeStatusChangeEventHandler();
    m_statusHolder = std::make_unique<StatusChangeCallback>(cb);
    m_manager->SetStatusChangeCallback(m_statusHolder.get());
  }

  void removeStatusChangeEventHandler() override {
    if (m_statusHolder) {
      m_manager->SetStatusChangeCallback(nullptr);
      m_statusHolder.reset();
    }
  }

  void replaceNotifyEventHandler(const NotifyCallback &cb) override {
    removeNotifyEventHandler();
    m_notifyHolder = std::make_unique<NotifyCallback>(cb);
    m_manager->SetNotifyCallback(m_notifyHolder.get());
  }

  void removeNotifyEventHandler() override {
    if (m_notifyHolder) {
      m_manager->SetNotifyCallback(nullptr);
      m_notifyHolder.reset();
    }
  }

  // === InstrumentOpenCriteria  ===
  void AddPortToExclude(const char *plugin, const char *port) override {
    try {
      m_manager->dotNETManager->GetPluginOpenOptions(gcnew String(plugin))
          ->AddPortToExclude(gcnew String(port));
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSRdotNETSDK produced Exception: " + msg);
    }
  }

  size_t GetPortsToExclude(const char *plugin, char **buffer,
                           size_t bufferCount, size_t stringLength) override {
    try {
      return copyStringList(
          m_manager->dotNETManager->GetPluginOpenOptions(gcnew String(plugin))
              ->PortsToExclude,
          buffer, bufferCount, stringLength);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSRdotNETSDK produced Exception: " + msg);
    }
  }

  void AddPortToInclude(const char *plugin, const char *port) override {
    try {
      m_manager->dotNETManager->GetPluginOpenOptions(gcnew String(plugin))
          ->AddPortToInclude(gcnew String(port));
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSRdotNETSDK produced Exception: " + msg);
    }
  }

  size_t GetPortsToInclude(const char *plugin, char **buffer,
                           size_t bufferCount, size_t stringLength) override {
    try {
      return copyStringList(
          m_manager->dotNETManager->GetPluginOpenOptions(gcnew String(plugin))
              ->PortsToInclude,
          buffer, bufferCount, stringLength);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSRdotNETSDK produced Exception: " + msg);
    }
  }

  bool AddOpenOption(const char *plugin, const char *openOptionName,
                     const char *openOptionValue) override {
    try {
      return m_manager->dotNETManager
          ->GetPluginOpenOptions(gcnew String(plugin))
          ->AddOpenOption(gcnew String(openOptionName),
                          gcnew String(openOptionValue));
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSRdotNETSDK produced Exception: " + msg);
    }
  }

  size_t GetOpenOption(const char *plugin, const char *openOptionName,
                       char *buffer, size_t bufferSize) override {
    try {
      System::String ^ result =
          m_manager->dotNETManager->GetPluginOpenOptions(gcnew String(plugin))
              ->GetOpenOption(gcnew String(openOptionName));
      return copyManagedStringToBuffer(result, buffer, bufferSize);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSRdotNETSDK produced Exception: " + msg);
    }
  }

  size_t GetOpenOptionNames(const char *plugin, char **buffer,
                            size_t bufferCount, size_t stringLength) override {
    try {
      return copyStringList(
          m_manager->dotNETManager->GetPluginOpenOptions(gcnew String(plugin))
              ->GetOpenOptionNames(),
          buffer, bufferCount, stringLength);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSRdotNETSDK produced Exception: " + msg);
    }
  }

  bool RemoveOpenOption(const char *plugin,
                        const char *openOptionName) override {
    try {
      return m_manager->dotNETManager
          ->GetPluginOpenOptions(gcnew String(plugin))
          ->RemoveOpenOption(gcnew String(openOptionName));
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSRdotNETSDK produced Exception: " + msg);
    }
  }

  // === Functions provided by SDK ===
  void AddManagedPlugin(const char *pluginName) override {
    try {
      m_manager->dotNETManager->AddManagedPlugin(gcnew String(pluginName));
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void AddPluginOpenOption(const char *strPluginName, const char *optionName,
                           const char *optionValue) override {
    try {
      m_manager->dotNETManager->AddPluginOpenOption(gcnew String(strPluginName),
                                                    gcnew String(optionName),
                                                    gcnew String(optionValue));
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void AddPluginType(const char *pluginType) override {
    try {
      m_manager->dotNETManager->AddPluginType(gcnew String(pluginType));
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void ForceDetach() override {
    try {
      m_manager->dotNETManager->ForceDetach();
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  size_t GetCustomSettings(char **buffer, size_t bufferCount,
                           size_t stringLength) override {
    try {
      return copyStringList(m_manager->dotNETManager->GetCustomSettings(),
                            buffer, bufferCount, stringLength);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  size_t GetInstruments(const char *pluginName, InstrumentID *buffer,
                        size_t bufferCount) override {
    try {
      auto instruments =
          m_manager->dotNETManager->GetInstruments(gcnew String(pluginName));

      size_t count = static_cast<size_t>(instruments->Length);

      if (buffer == nullptr || bufferCount == 0) {
        return count;
      }

      size_t copyCount = (count < bufferCount) ? count : bufferCount;
      for (size_t i = 0; i < copyCount; ++i) {
        buffer[i] = instrumentFromManaged(instruments[static_cast<int>(i)]);
      }

      return count;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  size_t GetManagedPluginNames(char **buffer, size_t bufferCount,
                               size_t stringLength) override {
    try {
      return copyStringList(m_manager->dotNETManager->GetManagedPluginNames(),
                            buffer, bufferCount, stringLength);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool GetPluginLibraryMetadata(const char *strPluginName,
                                JSRLibMetadata *metadata) override {
    try {
      if (metadata == nullptr) {
        return false;
      }

      auto managedMetadata = m_manager->dotNETManager->GetPluginLibraryMetadata(
          gcnew String(strPluginName));
      *metadata = libMetadataFromManaged(managedMetadata);
      return true;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  size_t GetPluginNames(char **buffer, size_t bufferCount,
                        size_t stringLength) override {
    try {
      return copyStringList(m_manager->dotNETManager->GetPluginNames(), buffer,
                            bufferCount, stringLength);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int GetPulserPropertyAttributes(const char *settingName) override {
    try {
      return m_manager->dotNETManager->GetPulserPropertyAttributes(
          gcnew String(settingName));
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  C_PROPERTY_UNITS GetPulserPropertyUnits(const char *settingName) override {
    try {
      return propertyUnitsFromManaged(
          m_manager->dotNETManager->GetPulserPropertyUnits(
              gcnew String(settingName)));
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  size_t GetPulserPropertyUnitsAsString(const char *settingName, bool useShort,
                                        char *buffer,
                                        size_t bufferSize) override {
    try {
      System::String ^ result =
          m_manager->dotNETManager->GetPulserPropertyUnitsAsString(
              gcnew String(settingName), useShort);
      return copyManagedStringToBuffer(result, buffer, bufferSize);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  size_t GetPulserReceiverInfo(const char *model, const char *serialNum,
                               int idxPR, char **buffer, size_t bufferCount,
                               size_t stringLength) override {
    try {
      return copyStringList(
          m_manager->dotNETManager->GetPulserReceiverInfo(
              gcnew String(model), gcnew String(serialNum), idxPR),
          buffer, bufferCount, stringLength);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  size_t GetPulserReceiverInfo(PulserReceiverID id, char **buffer,
                               size_t bufferCount,
                               size_t stringLength) override {
    return GetPulserReceiverInfo(
        id.InstrumentId.ModelName, id.InstrumentId.SerialNum,
        id.PulserReceiverIndex, buffer, bufferCount, stringLength);
  }

  size_t GetPulserReceivers(PulserReceiverID *buffer,
                            size_t bufferCount) override {
    try {
      auto managedReceivers =
          m_manager->dotNETManager->GetPulserReceivers(nullptr);
      size_t count = static_cast<size_t>(managedReceivers->Length);

      if (buffer == nullptr || bufferCount == 0) {
        return count;
      }

      size_t copyCount = (count < bufferCount) ? count : bufferCount;
      for (size_t i = 0; i < copyCount; ++i) {
        buffer[i] = pulsereceiverFromManaged(managedReceivers[static_cast<int>(i)]);
      }

      return count;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool IsPulserSettingSupported(const char *settingName) override {
    try {
      return m_manager->dotNETManager->IsPulserSettingSupported(
          gcnew String(settingName));
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void LoadPlugins(const char *pluginPath) override {
    try {
      m_manager->dotNETManager->LoadPlugins(gcnew String(pluginPath));
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void NotifyThreadProc() override {
    try {
      m_manager->dotNETManager->NotifyThreadProc();
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void RemoveAllOpenOptions(const char *strPluginName) override {
    try {
      m_manager->dotNETManager->RemoveAllOpenOptions(
          gcnew String(strPluginName));
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void RemoveManagedPlugin(const char *pluginName) override {
    try {
      m_manager->dotNETManager->RemoveManagedPlugin(gcnew String(pluginName));
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void RequestThreadProc() override {
    try {
      m_manager->dotNETManager->RequestThreadProc();
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void SetCurrentPulserReceiver(PulserReceiverID prID) override {
    SetCurrentPulserReceiver(prID.InstrumentId.ModelName,
                             prID.InstrumentId.SerialNum,
                             prID.PulserReceiverIndex);
  }

  void SetCurrentPulserReceiver(const char *model, const char *serialNum,
                                int idxPR) override {
    try {
      m_manager->dotNETManager->SetCurrentPulserReceiver(
          gcnew String(model), gcnew String(serialNum), idxPR);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void SetDiscoveryEnable(bool bEnable) override {
    try {
      m_manager->dotNETManager->SetDiscoveryEnable(bEnable);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK enable produced Exception: " +
                               msg);
    }
  }

  void SetPulserPropertyValue(const char *strProp, const char *value) override {
    try {
      m_manager->dotNETManager->SetPulserPropertyValue(gcnew String(strProp),
                                                       gcnew String(value));
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void SetPulserPropertyValue(const char *settingName,
                              C_PULSER_PROPERTY_ROLE role,
                              const char *value) override {
    try {
      m_manager->dotNETManager->SetPulserPropertyValue(
          gcnew String(settingName), pulserPropertyRoleToManaged(role),
          gcnew String(value));
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void Shutdown() override {
    try {
      m_manager->dotNETManager->Shutdown();
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  // === Getters and setters for variables in the manager ===
  bool getPulseRepetitionFrequencyIndexSupported() override {
    try {
      return m_manager->dotNETManager->PulseRepetitionFrequencyIndexSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  double getHVSupplyMax() override {
    try {
      return m_manager->dotNETManager->HVSupplyMax;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  double getHVSupply() override {
    try {
      return m_manager->dotNETManager->HVSupply;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setHVSupply(double hvSupply) override {
    try {
      m_manager->dotNETManager->HVSupply = hvSupply;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getHVSupplySupported() override {
    try {
      return m_manager->dotNETManager->HVSupplySupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getPulseRepetitionFrequencyNumerator() override {
    try {
      return m_manager->dotNETManager->PulseRepetitionFrequencyNumerator;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  size_t getPulseRepetitionFrequencyValues(double *buffer,
                                           size_t bufferCount) override {
    try {
      return copyDoubleArray(
          m_manager->dotNETManager->PulseRepetitionFrequencyValues, buffer,
          bufferCount);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getPulseRepetitionFrequencyIndexMax() override {
    try {
      return m_manager->dotNETManager->PulseRepetitionFrequencyIndexMax;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getPulseRepetitionFrequencyIndex() override {
    try {
      return m_manager->dotNETManager->PulseRepetitionFrequencyIndex;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setPulseRepetitionFrequencyIndex(int index) override {
    try {
      m_manager->dotNETManager->PulseRepetitionFrequencyIndex = index;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  double getPulseRepetitionFrequencyMin() override {
    try {
      return m_manager->dotNETManager->PulseRepetitionFrequencyMin;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  double getPulseRepetitionFrequencyMax() override {
    try {
      return m_manager->dotNETManager->PulseRepetitionFrequencyMax;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  double getPulseRepetitionFrequency() override {
    try {
      return m_manager->dotNETManager->PulseRepetitionFrequency;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setPulseRepetitionFrequency(double frequency) override {
    try {
      m_manager->dotNETManager->PulseRepetitionFrequency = frequency;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  C_TRIGGER_POLARITY getTriggerEdgePolarity() override {
    try {
      return triggerPolarityFromManaged(
          m_manager->dotNETManager->TriggerEdgePolarity);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setTriggerEdgePolarity(C_TRIGGER_POLARITY polarity) override {
    try {
      m_manager->dotNETManager->TriggerEdgePolarity =
          triggerPolarityToManaged(polarity);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getTriggerEdgePolaritySupported() override {
    try {
      return m_manager->dotNETManager->TriggerEdgePolaritySupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  size_t getHighPassFilterValues(double *buffer, size_t bufferCount) override {
    try {
      return copyDoubleArray(m_manager->dotNETManager->HighPassFilterValues,
                             buffer, bufferCount);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getHighPassFilterIndexMax() override {
    try {
      return m_manager->dotNETManager->HighPassFilterIndexMax;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getHighPassFilterIndex() override {
    try {
      return m_manager->dotNETManager->HighPassFilterIndex;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setHighPassFilterIndex(int index) override {
    try {
      m_manager->dotNETManager->HighPassFilterIndex = index;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getLowPassFilterIndexMax() override {
    try {
      return m_manager->dotNETManager->LowPassFilterIndexMax;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getLowPassFilterIndex() override {
    try {
      return m_manager->dotNETManager->LowPassFilterIndex;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setLowPassFilterIndex(int index) override {
    try {
      m_manager->dotNETManager->LowPassFilterIndex = index;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  C_RECEIVER_MODE getReceiverMode() override {
    try {
      return receiverModeFromManaged(m_manager->dotNETManager->ReceiverMode);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setReceiverMode(C_RECEIVER_MODE mode) override {
    try {
      m_manager->dotNETManager->ReceiverMode = receiverModeToManaged(mode);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getReceiverModeBothSupported() override {
    try {
      return m_manager->dotNETManager->ReceiverModeBothSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getReceiverModeThruSupported() override {
    try {
      return m_manager->dotNETManager->ReceiverModeThruSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getReceiverModeEchoSupported() override {
    try {
      return m_manager->dotNETManager->ReceiverModeEchoSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  size_t getGainValues(double *buffer, size_t bufferCount) override {
    try {
      return copyDoubleArray(m_manager->dotNETManager->GainValues, buffer,
                             bufferCount);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getGainIndexMax() override {
    try {
      return m_manager->dotNETManager->GainIndexMax;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getGainIndex() override {
    try {
      return m_manager->dotNETManager->GainIndex;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setGainIndex(int index) override {
    try {
      m_manager->dotNETManager->GainIndex = index;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  size_t getLowPassFilterValues(double *buffer, size_t bufferCount) override {
    try {
      return copyDoubleArray(m_manager->dotNETManager->LowPassFilterValues,
                             buffer, bufferCount);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  double getHVSupplyMin() override {
    try {
      return m_manager->dotNETManager->HVSupplyMin;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getHVSupplyIndexSupported() override {
    try {
      return m_manager->dotNETManager->HVSupplyIndexSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getHVSupplyIndex() override {
    try {
      return m_manager->dotNETManager->HVSupplyIndex;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setHVSupplyIndex(int index) override {
    try {
      m_manager->dotNETManager->HVSupplyIndex = index;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  double getHVMeasurement() override {
    try {
      return m_manager->dotNETManager->HVMeasurement;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getHVMeasurementSupported() override {
    try {
      return m_manager->dotNETManager->HVMeasurementSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  size_t getUnitModelName(char *buffer, size_t bufferSize) override {
    try {
      return copyManagedStringToBuffer(m_manager->dotNETManager->UnitModelName,
                                       buffer, bufferSize);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setUnitModelName(const char *name) override {
    try {
      m_manager->dotNETManager->UnitModelName = gcnew String(name);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  size_t getUnitSerialNum(char *buffer, size_t bufferSize) override {
    try {
      return copyManagedStringToBuffer(m_manager->dotNETManager->UnitSerialNum,
                                       buffer, bufferSize);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setUnitSerialNum(const char *serialNum) override {
    try {
      m_manager->dotNETManager->UnitSerialNum = gcnew String(serialNum);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getPulserOEMDataSupported() override {
    try {
      return m_manager->dotNETManager->PulserOEMDataSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getReceiverOEMDataSupported() override {
    try {
      return m_manager->dotNETManager->ReceiverOEMDataSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getUnitModelNameSupported() override {
    try {
      return m_manager->dotNETManager->UnitModelNameSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getUnitSerialNumSupported() override {
    try {
      return m_manager->dotNETManager->UnitSerialNumSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  C_TRIGGER_SOURCE getTriggerSource() override {
    try {
      return triggerSourceFromManaged(m_manager->dotNETManager->TriggerSource);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setTriggerSource(C_TRIGGER_SOURCE source) override {
    try {
      m_manager->dotNETManager->TriggerSource = triggerSourceToManaged(source);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  size_t getPulserTriggerSourceValueNames(char **buffer, size_t bufferCount,
                                          size_t stringLength) override {
    try {
      return copyStringList(
          m_manager->dotNETManager->PulserTriggerSourceValueNames, buffer,
          bufferCount, stringLength);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getPulserTriggerSourceIndexMax() override {
    try {
      return m_manager->dotNETManager->PulserTriggerSourceIndexMax;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getPulserTriggerSourceIndex() override {
    try {
      return m_manager->dotNETManager->PulserTriggerSourceIndex;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setPulserTriggerSourceIndex(int index) override {
    try {
      m_manager->dotNETManager->PulserTriggerSourceIndex = index;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getGainIndexSupported() override {
    try {
      return m_manager->dotNETManager->GainIndexSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getTriggerSourceSlaveSupported() override {
    try {
      return m_manager->dotNETManager->TriggerSourceSlaveSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getTriggerSourceExternalSupported() override {
    try {
      return m_manager->dotNETManager->TriggerSourceExternalSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getTriggerEnable() override {
    try {
      return m_manager->dotNETManager->TriggerEnable;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setTriggerEnable(bool enable) override {
    try {
      m_manager->dotNETManager->TriggerEnable = enable;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  size_t getPulseEnergyValueNames(char **buffer, size_t bufferCount,
                                  size_t stringLength) override {
    try {
      return copyStringList(m_manager->dotNETManager->PulseEnergyValueNames,
                            buffer, bufferCount, stringLength);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getPulseEnergyIndexMax() override {
    try {
      return m_manager->dotNETManager->PulseEnergyIndexMax;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getPulseEnergyIndex() override {
    try {
      return m_manager->dotNETManager->PulseEnergyIndex;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setPulseEnergyIndex(int index) override {
    try {
      m_manager->dotNETManager->PulseEnergyIndex = index;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getPulseEnergyIndexSupported() override {
    try {
      return m_manager->dotNETManager->PulseEnergyIndexSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  size_t getDampingValues(double *buffer, size_t bufferCount) override {
    try {
      return copyDoubleArray(m_manager->dotNETManager->DampingValues, buffer,
                             bufferCount);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getDampingIndexMax() override {
    try {
      return m_manager->dotNETManager->DampingIndexMax;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getDampingIndex() override {
    try {
      return m_manager->dotNETManager->DampingIndex;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setDampingIndex(int index) override {
    try {
      m_manager->dotNETManager->DampingIndex = index;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getDampingIndexSupported() override {
    try {
      return m_manager->dotNETManager->DampingIndexSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  size_t getHVSupplyValues(double *buffer, size_t bufferCount) override {
    try {
      return copyDoubleArray(m_manager->dotNETManager->HVSupplyValues, buffer,
                             bufferCount);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getHVSupplyIndexMax() override {
    try {
      return m_manager->dotNETManager->HVSupplyIndexMax;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getTriggerSourceInternalSupported() override {
    try {
      return m_manager->dotNETManager->TriggerSourceInternalSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getGainStepSizeSupported() override {
    try {
      return m_manager->dotNETManager->GainStepSizeSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  double getGainStepSize() override {
    try {
      return m_manager->dotNETManager->GainStepSize;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  double getGainMax() override {
    try {
      return m_manager->dotNETManager->GainMax;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  size_t getPulserSerialNum(char *buffer, size_t bufferSize) override {
    try {
      return copyManagedStringToBuffer(
          m_manager->dotNETManager->PulserSerialNum, buffer, bufferSize);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setPulserSerialNum(const char *serialNum) override {
    try {
      m_manager->dotNETManager->PulserSerialNum = gcnew String(serialNum);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getPulserSerialNumSupported() override {
    try {
      return m_manager->dotNETManager->PulserSerialNumSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getReceiverHWRevSupported() override {
    try {
      return m_manager->dotNETManager->ReceiverHWRevSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  size_t getReceiverHWRev(char *buffer, size_t bufferSize) override {
    try {
      return copyManagedStringToBuffer(m_manager->dotNETManager->ReceiverHWRev,
                                       buffer, bufferSize);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setReceiverHWRev(const char *hwRev) override {
    try {
      m_manager->dotNETManager->ReceiverHWRev = gcnew String(hwRev);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getPulserHWRevSupported() override {
    try {
      return m_manager->dotNETManager->PulserHWRevSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  size_t getPulserHWRev(char *buffer, size_t bufferSize) override {
    try {
      return copyManagedStringToBuffer(m_manager->dotNETManager->PulserHWRev,
                                       buffer, bufferSize);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setPulserHWRev(const char *hwRev) override {
    try {
      m_manager->dotNETManager->PulserHWRev = gcnew String(hwRev);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getReceiverFirmwareVerSupported() override {
    try {
      return m_manager->dotNETManager->ReceiverFirmwareVerSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  size_t getReceiverFirmwareVer(char *buffer, size_t bufferSize) override {
    try {
      return copyManagedStringToBuffer(
          m_manager->dotNETManager->ReceiverFirmwareVer, buffer, bufferSize);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getPulserFirmwareVerSupported() override {
    try {
      return m_manager->dotNETManager->PulserFirmwareVerSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  size_t getPulserFirmwareVer(char *buffer, size_t bufferSize) override {
    try {
      return copyManagedStringToBuffer(
          m_manager->dotNETManager->PulserFirmwareVer, buffer, bufferSize);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  double getMaxFrequency() override {
    try {
      return m_manager->dotNETManager->MaxFrequency;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  C_TRIGGER_IMPEDANCE getTriggerImpedance() override {
    try {
      return triggerImpedanceFromManaged(
          m_manager->dotNETManager->TriggerImpedance);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setTriggerImpedance(C_TRIGGER_IMPEDANCE impedance) override {
    try {
      m_manager->dotNETManager->TriggerImpedance =
          triggerImpedanceToManaged(impedance);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getTriggerImpedanceSupported() override {
    try {
      return m_manager->dotNETManager->TriggerImpedanceSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  C_PULSER_IMPEDANCE getPulserImpedance() override {
    try {
      return pulserImpedanceFromManaged(
          m_manager->dotNETManager->PulserImpedance);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setPulserImpedance(C_PULSER_IMPEDANCE impedance) override {
    try {
      m_manager->dotNETManager->PulserImpedance =
          pulserImpedanceToManaged(impedance);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getPulserImpedanceSupported() override {
    try {
      return m_manager->dotNETManager->PulserImpedanceSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  double getEnergyPerPulse() override {
    try {
      return m_manager->dotNETManager->EnergyPerPulse;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  size_t getInfo(char **buffer, size_t bufferCount,
                 size_t stringLength) override {
    try {
      return copyStringList(m_manager->dotNETManager->Info, buffer, bufferCount,
                            stringLength);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  C_IS_PULSING getPulserIsPulsing() override {
    try {
      return isPulsingFromManaged(m_manager->dotNETManager->PulserIsPulsing);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  C_POWER_LIMIT getPulserPowerLimitStatus() override {
    try {
      return powerLimitFromManaged(
          m_manager->dotNETManager->PulserPowerLimitStatus);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getPulserTriggerCount() override {
    try {
      return m_manager->dotNETManager->PulserTriggerCount;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getPulserTriggerCountSupported() override {
    try {
      return m_manager->dotNETManager->PulserTriggerCountSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getHVSupplyEnable() override {
    try {
      return m_manager->dotNETManager->HVSupplyEnable;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setHVSupplyEnable(bool enable) override {
    try {
      m_manager->dotNETManager->HVSupplyEnable = enable;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getHVSupplyEnableSupported() override {
    try {
      return m_manager->dotNETManager->HVSupplyEnableSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getReceiverSerialNumSupported() override {
    try {
      return m_manager->dotNETManager->ReceiverSerialNumSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getIsPulserPresentSupported() override {
    try {
      return m_manager->dotNETManager->IsPulserPresentSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  size_t getReceiverSerialNum(char *buffer, size_t bufferSize) override {
    try {
      return copyManagedStringToBuffer(
          m_manager->dotNETManager->ReceiverSerialNum, buffer, bufferSize);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setReceiverSerialNum(const char *serialNum) override {
    try {
      m_manager->dotNETManager->ReceiverSerialNum = gcnew String(serialNum);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getPulserModelNameSupported() override {
    try {
      return m_manager->dotNETManager->PulserModelNameSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  double getGainMin() override {
    try {
      return m_manager->dotNETManager->GainMin;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  double getGain() override {
    try {
      return m_manager->dotNETManager->Gain;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setGain(double gain) override {
    try {
      m_manager->dotNETManager->Gain = gain;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getHasManualControls() override {
    try {
      return m_manager->dotNETManager->HasManualControls;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  size_t getLEDBlinkModeValues(char **buffer, size_t bufferCount,
                               size_t stringLength) override {
    try {
      return copyStringList(m_manager->dotNETManager->LEDBlinkModeValues,
                            buffer, bufferCount, stringLength);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getLEDBlinkModeIndexMax() override {
    try {
      return m_manager->dotNETManager->LEDBlinkModeIndexMax;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getLEDBlinkModeIndex() override {
    try {
      return m_manager->dotNETManager->LEDBlinkModeIndex;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setLEDBlinkModeIndex(int index) override {
    try {
      m_manager->dotNETManager->LEDBlinkModeIndex = index;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getLEDBlinkModeIndexSupported() override {
    try {
      return m_manager->dotNETManager->LEDBlinkModeIndexSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  PulserReceiverID getId() override {
    try {
      return pulsereceiverFromManaged(m_manager->dotNETManager->Id);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getIsPulserReceiverSelected() override {
    try {
      return m_manager->dotNETManager->IsPulserReceiverSelected;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  size_t getLastExceptionContextMessage(char *buffer,
                                        size_t bufferSize) override {
    try {
      return copyManagedStringToBuffer(
          m_manager->dotNETManager->LastExceptionContextMessage, buffer,
          bufferSize);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  size_t getLastExceptionOrNull(char *buffer, size_t bufferSize) override {
    try {
      Exception ^ exception = m_manager->dotNETManager->LastExceptionOrNull;
      if (exception == nullptr) {
        if (buffer != nullptr && bufferSize > 0) {
          buffer[0] = '\0';
        }
        return 0;
      }
      return copyManagedStringToBuffer(exception->Message, buffer, bufferSize);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  size_t getPluginPath(char *buffer, size_t bufferSize) override {
    try {
      return copyManagedStringToBuffer(m_manager->dotNETManager->PluginPath,
                                       buffer, bufferSize);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setPluginPath(const char *path) override {
    try {
      m_manager->dotNETManager->PluginPath = gcnew String(path);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  size_t getPulserModelName(char *buffer, size_t bufferSize) override {
    try {
      return copyManagedStringToBuffer(
          m_manager->dotNETManager->PulserModelName, buffer, bufferSize);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setPulserModelName(const char *name) override {
    try {
      m_manager->dotNETManager->PulserModelName = gcnew String(name);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getPluginsLoaded() override {
    try {
      return m_manager->dotNETManager->PluginsLoaded;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  C_MANAGER_STATE getManagerState() override {
    try {
      return managerStateFromManaged(m_manager->dotNETManager->ManagerState);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getPulserMaxPRFsSupported() override {
    try {
      return m_manager->dotNETManager->PulserMaxPRFsSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  size_t getPulserMaxPRFs(double *buffer, size_t bufferCount) override {
    try {
      return copyDoubleArray(m_manager->dotNETManager->PulserMaxPRFs, buffer,
                             bufferCount);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getPulserEnergyCapacitorValuesSupported() override {
    try {
      return m_manager->dotNETManager->PulserEnergyCapacitorValuesSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  size_t getPulserEnergyCapacitorValues(double *buffer,
                                        size_t bufferCount) override {
    try {
      return copyDoubleArray(
          m_manager->dotNETManager->PulserEnergyCapacitorValues, buffer,
          bufferCount);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  size_t getReceiverSupplyVoltages(char **buffer, size_t bufferCount,
                                   size_t stringLength) override {
    try {
      return copyStringList(m_manager->dotNETManager->ReceiverSupplyVoltages,
                            buffer, bufferCount, stringLength);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getReceiverSupplyVoltagesSupported() override {
    try {
      return m_manager->dotNETManager->ReceiverSupplyVoltagesSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getReceiverModelNameSupported() override {
    try {
      return m_manager->dotNETManager->ReceiverModelNameSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  size_t getReceiverModelName(char *buffer, size_t bufferSize) override {
    try {
      return copyManagedStringToBuffer(
          m_manager->dotNETManager->ReceiverModelName, buffer, bufferSize);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setReceiverModelName(const char *name) override {
    try {
      m_manager->dotNETManager->ReceiverModelName = gcnew String(name);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getArePluginsAvailable() override {
    try {
      return m_manager->dotNETManager->ArePluginsAvailable;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getIsPulserPresent() override {
    try {
      return m_manager->dotNETManager->IsPulserPresent;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
};

extern "C" {

__declspec(dllexport) JSRSDKManager *CreateJSRSDKManager() {
  try {
    return new JSRSDKManagerAdapter();
  } catch (System::Exception ^ exception) {
    std::string msg =
        msclr::interop::marshal_as<std::string>(exception->Message);
    throw std::runtime_error("Failed creating JSR adapter object: " + msg);
  }
}

__declspec(dllexport) void DestroyJSRSDKManager(JSRSDKManager *bridge) {
  delete bridge;
}
}
