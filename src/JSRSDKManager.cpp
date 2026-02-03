#include "JSR-SDK/JSRSDKManager.h"
#include "JSR-SDK/JSRSDKWrapper.h"
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
  void loadPluginsFromBinaryDir() {
    // Retrieve the directory of the current executable
    String ^ binaryDir = "";
    try {
      String ^ executablePath = Assembly::GetExecutingAssembly()->Location;
      binaryDir = Path::GetDirectoryName(executablePath);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("Failed finding the binary directory: " + msg);
    }
    // Load plugins from the binary directory
    try {
      m_manager->dotNETManager->LoadPlugins(binaryDir);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("Failed loading plugins from binary dir: " +
                               msg);
    }
  }

  // === Event handlers used for callbacks ===
  // --- Status-change ----
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

  // --- Notify ----
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
      m_manager->dotNETManager
          ->GetPluginOpenOptions(gcnew String(plugin))
          ->AddPortToExclude(gcnew String(port));
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSRdotNETSDK produced Exception: " + msg);
    }
  }

  CVector<CString> GetPortsToExclude(const char *plugin) override {
    try {
      return listToCVectorString<System::String^>(
          m_manager->dotNETManager
              ->GetPluginOpenOptions(gcnew String(plugin))
              ->PortsToExclude);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSRdotNETSDK produced Exception: " + msg);
    }
  }

  void AddPortToInclude(const char *plugin, const char *port) override {
    try {
      m_manager->dotNETManager
          ->GetPluginOpenOptions(gcnew String(plugin))
          ->AddPortToInclude(gcnew String(port));
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSRdotNETSDK produced Exception: " + msg);
    }
  }

  CVector<CString> GetPortsToInclude(const char *plugin) override {
    try {
      return listToCVectorString<System::String^>(
          m_manager->dotNETManager
              ->GetPluginOpenOptions(gcnew String(plugin))
              ->PortsToInclude);
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

  CString GetOpenOption(const char *plugin,
                        const char *openOptionName) override {
    try {
      std::string result = marshal_as<std::string>(
          m_manager->dotNETManager
              ->GetPluginOpenOptions(gcnew String(plugin))
              ->GetOpenOption(gcnew String(openOptionName)));
      return CString::from_std_string(result);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSRdotNETSDK produced Exception: " + msg);
    }
  }

  CVector<CString> GetOpenOptionNames(const char *plugin) override {
    try {
      return listToCVectorString<System::String^>(
          m_manager->dotNETManager
              ->GetPluginOpenOptions(gcnew String(plugin))
              ->GetOpenOptionNames());
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
      m_manager->dotNETManager->AddPluginOpenOption(
          gcnew String(strPluginName), gcnew String(optionName),
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

  CVector<CString> GetCustomSettings() override {
    try {
      return listToCVectorString<System::String^>(
          m_manager->dotNETManager->GetCustomSettings());
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  CVector<InstrumentID> GetInstruments(const char *pluginName) override {
    try {
      auto instruments = m_manager->dotNETManager->GetInstruments(
          gcnew String(pluginName));

      int count = instruments->Length;
      if (count == 0)
        return CVector<InstrumentID>();

      InstrumentID *buffer = new InstrumentID[count];
      int index = 0;
      for each (IInstrumentIdentity ^ instrument in instruments) {
        buffer[index++] = instrumentFromManaged(instrument);
      }
      return CVector<InstrumentID>(buffer, count, count);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  CVector<CString> GetManagedPluginNames() override {
    try {
      return listToCVectorString<System::String^>(
          m_manager->dotNETManager->GetManagedPluginNames());
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  JSRLibMetadata GetPluginLibraryMetadata(const char *strPluginName) override {
    try {
      auto metadata = m_manager->dotNETManager->GetPluginLibraryMetadata(
          gcnew String(strPluginName));
      return libMetadataFromManaged(metadata);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  CVector<CString> GetPluginNames() override {
    try {
      return listToCVectorString<System::String^>(
          m_manager->dotNETManager->GetPluginNames());
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

  CString GetPulserPropertyUnitsAsString(const char *settingName,
                                         bool useShort = false) override {
    try {
      std::string result = marshal_as<std::string>(
          m_manager->dotNETManager->GetPulserPropertyUnitsAsString(
              gcnew String(settingName), useShort));
      return CString::from_std_string(result);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  CVector<CString> GetPulserReceiverInfo(const char *model,
                                         const char *serialNum, int idxPR) override {
    try {
      return listToCVectorString<System::String^>(
          m_manager->dotNETManager->GetPulserReceiverInfo(
              gcnew String(model), gcnew String(serialNum), idxPR));
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  CVector<CString> GetPulserReceiverInfo(PulserReceiverID id) override {
    std::string model = id.InstrumentId.ModelName.to_std_string();
    std::string serial = id.InstrumentId.SerialNum.to_std_string();
    return GetPulserReceiverInfo(model.c_str(), serial.c_str(),
                                 id.PulserReceiverIndex);
  }

  CVector<PulserReceiverID> GetPulserReceivers() override {
    try {
      auto managedReceivers =
          m_manager->dotNETManager->GetPulserReceivers(nullptr);

      int count = managedReceivers->Length;
      if (count == 0)
        return CVector<PulserReceiverID>();

      PulserReceiverID *buffer = new PulserReceiverID[count];
      int index = 0;
      for each (IPulserReceiverIdentity ^ receiver in managedReceivers) {
        buffer[index++] = pulsereceiverFromManaged(receiver);
      }
      return CVector<PulserReceiverID>(buffer, count, count);
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
    std::string model = prID.InstrumentId.ModelName.to_std_string();
    std::string serial = prID.InstrumentId.SerialNum.to_std_string();
    SetCurrentPulserReceiver(model.c_str(), serial.c_str(),
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
    JSRDotNETSDK::JSRDotNETManager ^ manager;
    try {
      manager = m_manager->dotNETManager;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK manager produced Exception: " +
                               msg);
    }
    try {
      manager->SetDiscoveryEnable(bEnable);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK enable produced Exception: " +
                               msg);
    }
  }

  void SetPulserPropertyValue(const char *strProp, const char *value) override {
    try {
      m_manager->dotNETManager->SetPulserPropertyValue(
          gcnew String(strProp), gcnew String(value));
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

  CVector<double> getPulseRepetitionFrequencyValues() override {
    try {
      return listToCVector<System::Double, double>(
          m_manager->dotNETManager->PulseRepetitionFrequencyValues);
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

  CVector<double> getHighPassFilterValues() override {
    try {
      return listToCVector<System::Double, double>(
          m_manager->dotNETManager->HighPassFilterValues);
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

  CVector<double> getGainValues() override {
    try {
      return listToCVector<System::Double, double>(m_manager->dotNETManager->GainValues);
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

  CVector<double> getLowPassFilterValues() override {
    try {
      return listToCVector<System::Double, double>(
          m_manager->dotNETManager->LowPassFilterValues);
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

  CString getUnitModelName() override {
    try {
      std::string result = marshal_as<std::string>(m_manager->dotNETManager->UnitModelName);
      return CString::from_std_string(result);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setUnitModelName(CString name) override {
    try {
      std::string str = name.to_std_string();
      m_manager->dotNETManager->UnitModelName = marshal_as<String ^>(str);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  CString getUnitSerialNum() override {
    try {
      std::string result = marshal_as<std::string>(m_manager->dotNETManager->UnitSerialNum);
      return CString::from_std_string(result);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setUnitSerialNum(CString serialNum) override {
    try {
      std::string str = serialNum.to_std_string();
      m_manager->dotNETManager->UnitSerialNum = marshal_as<String ^>(str);
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

  CVector<CString> getPulserTriggerSourceValueNames() override {
    try {
      return listToCVectorString<System::String^>(
          m_manager->dotNETManager->PulserTriggerSourceValueNames);
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

  CVector<CString> getPulseEnergyValueNames() override {
    try {
      return listToCVectorString<System::String^>(
          m_manager->dotNETManager->PulseEnergyValueNames);
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

  CVector<double> getDampingValues() override {
    try {
      return listToCVector<System::Double, double>(
          m_manager->dotNETManager->DampingValues);
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

  CVector<double> getHVSupplyValues() override {
    try {
      return listToCVector<System::Double, double>(
          m_manager->dotNETManager->HVSupplyValues);
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

  CString getPulserSerialNum() override {
    try {
      std::string result = marshal_as<std::string>(m_manager->dotNETManager->PulserSerialNum);
      return CString::from_std_string(result);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setPulserSerialNum(CString serialNum) override {
    try {
      std::string str = serialNum.to_std_string();
      m_manager->dotNETManager->PulserSerialNum = marshal_as<String ^>(str);
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

  CString getReceiverHWRev() override {
    try {
      std::string result = marshal_as<std::string>(m_manager->dotNETManager->ReceiverHWRev);
      return CString::from_std_string(result);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setReceiverHWRev(CString hwRev) override {
    try {
      std::string str = hwRev.to_std_string();
      m_manager->dotNETManager->ReceiverHWRev = marshal_as<String ^>(str);
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

  CString getPulserHWRev() override {
    try {
      std::string result = marshal_as<std::string>(m_manager->dotNETManager->PulserHWRev);
      return CString::from_std_string(result);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setPulserHWRev(CString hwRev) override {
    try {
      std::string str = hwRev.to_std_string();
      m_manager->dotNETManager->PulserHWRev = marshal_as<String ^>(str);
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

  CString getReceiverFirmwareVer() override {
    try {
      std::string result = marshal_as<std::string>(m_manager->dotNETManager->ReceiverFirmwareVer);
      return CString::from_std_string(result);
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

  CString getPulserFirmwareVer() override {
    try {
      std::string result = marshal_as<std::string>(m_manager->dotNETManager->PulserFirmwareVer);
      return CString::from_std_string(result);
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

  CVector<CString> getInfo() override {
    try {
      return listToCVectorString<System::String^>(
          m_manager->dotNETManager->Info);
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

  CString getReceiverSerialNum() override {
    try {
      std::string result = marshal_as<std::string>(
          m_manager->dotNETManager->ReceiverSerialNum);
      return CString::from_std_string(result);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setReceiverSerialNum(CString serialNum) override {
    try {
      std::string str = serialNum.to_std_string();
      m_manager->dotNETManager->ReceiverSerialNum = marshal_as<String ^>(str);
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

  CVector<CString> getLEDBlinkModeValues() override {
    try {
      return listToCVectorString<System::String^>(
          m_manager->dotNETManager->LEDBlinkModeValues);
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

  CString getLastExceptionContextMessage() override {
    try {
      std::string result = marshal_as<std::string>(m_manager->dotNETManager->LastExceptionContextMessage);
      return CString::from_std_string(result);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  CString getLastExceptionOrNull() override {
    try {
      Exception ^ exception = m_manager->dotNETManager->LastExceptionOrNull;
      if (exception != nullptr) {
        std::string result = marshal_as<std::string>(exception->Message);
        return CString::from_std_string(result);
      } else {
        return CString();
      }
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  CString getPluginPath() override {
    try {
      std::string result = marshal_as<std::string>(m_manager->dotNETManager->PluginPath);
      return CString::from_std_string(result);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setPluginPath(CString path) override {
    try {
      std::string str = path.to_std_string();
      m_manager->dotNETManager->PluginPath = marshal_as<String ^>(str);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  CString getPulserModelName() override {
    try {
      std::string result = marshal_as<std::string>(m_manager->dotNETManager->PulserModelName);
      return CString::from_std_string(result);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setPulserModelName(CString name) override {
    try {
      std::string str = name.to_std_string();
      m_manager->dotNETManager->PulserModelName = marshal_as<String ^>(str);
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

  CVector<double> getPulserMaxPRFs() override {
    try {
      auto values = m_manager->dotNETManager->PulserMaxPRFs;
      int count = values->Length;
      if (count == 0)
        return CVector<double>();

      double *buffer = new double[count];
      for (int i = 0; i < count; ++i)
        buffer[i] = values[i];

      return CVector<double>(buffer, count, count);
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

  CVector<double> getPulserEnergyCapacitorValues() override {
    try {
      return listToCVector<System::Double, double>(
          m_manager->dotNETManager->PulserEnergyCapacitorValues);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  CVector<CString> getReceiverSupplyVoltages() override {
    try {
      return listToCVectorString<System::String^>(
          m_manager->dotNETManager->ReceiverSupplyVoltages);
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

  CString getReceiverModelName() override {
    try {
      std::string result = marshal_as<std::string>(
          m_manager->dotNETManager->ReceiverModelName);
      return CString::from_std_string(result);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void setReceiverModelName(CString name) override {
    try {
      std::string str = name.to_std_string();
      m_manager->dotNETManager->ReceiverModelName = marshal_as<String ^>(str);
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
