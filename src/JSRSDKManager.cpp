#include "JSR-SDK/JSRSDKManager.h"
#include "JSR-SDK/marshals/MarshalTypes.h"
#include "JSR-SDK/JSRSDKWrapper.h"

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
      throw "Failed creating JSR Bridge object: " + msg;
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

  // === Static functions used to generate IDs ===
  // static std::string MakeIdString(IPulserReceiverIdentity prId) = 0;

  // static std::string MakeIdString(std::string model, std::string serialNum,
  //                                    int idxPR) = 0;

  // static void ParseIdString(std::string sID, out std::string model,
  //                           out std::string serialNum,
  //                           out int idxPR) = 0;

  // === InstrumentOpenCriteria  ===

  void AddPortToExclude(std::string plugin, std::string port) override {
    try {
      m_manager->dotNETManager->GetPluginOpenOptions(marshal_as<String ^>(plugin))
          ->AddPortToExclude(marshal_as<String ^>(port));
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSRdotNETSDK produced Exception: " + msg);
    }
  }
  void AddPortToInclude(std::string plugin, std::string port) override {
    try {
      m_manager->dotNETManager
          ->GetPluginOpenOptions(marshal_as<String ^>(plugin))
          ->AddPortToInclude(marshal_as<String ^>(port));
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSRdotNETSDK produced Exception: " + msg);
    }
  }
  // === Functions provided by SDK ===
  void AddManagedPlugin(std::string pluginName) {
    try {

      m_manager->dotNETManager->AddManagedPlugin(
          marshal_as<String ^>(pluginName));
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  void AddPluginOpenOption(std::string strPluginName, std::string optionName,
                           std::string optionValue) {
    try {
      m_manager->dotNETManager->AddPluginOpenOption(
          marshal_as<String ^>(strPluginName), marshal_as<String ^>(optionName),
          marshal_as<String ^>(optionValue));
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void AddPluginType(std::string pluginType) {
    try {
      m_manager->dotNETManager->AddPluginType(marshal_as<String ^>(pluginType));
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void ForceDetach() {
    try {
      m_manager->dotNETManager->ForceDetach();
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  std::vector<std::string> GetCustomSettings() {
    try {
      return listToVectorMarshall < System::String ^,
             std::string > (m_manager->dotNETManager->GetCustomSettings());
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  std::vector<InstrumentID> GetInstruments(std::string pluginName) {
    try {

      auto instruments = m_manager->dotNETManager->GetInstruments(
          marshal_as<String ^>(pluginName));

      std::vector<InstrumentID> result(instruments->Length);

      for each (IInstrumentIdentity ^ instrument in instruments) {
        result.push_back(instrumentFromManaged(instrument));
      }
      return result;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  std::vector<std::string> GetManagedPluginNames() {
    try {
      return listToVectorMarshall < System::String ^,
             std::string > (m_manager->dotNETManager->GetManagedPluginNames());
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  std::map<std::string, std::vector<std::string>>
  GetPluginLibOpenOptions(std::string strPluginName) {
    try {

      auto options = m_manager->dotNETManager->GetPluginLibOpenOptions(
          marshal_as<String ^>(strPluginName));
      std::map<std::string, std::vector<std::string>> results;
      for each (KeyValuePair<String ^, List<String ^> ^> kvp in options) {
        const std::string key = marshal_as<std::string>(kvp.Key);

        results.emplace(key, listToVectorMarshall < System::String ^,
                        std::string > (kvp.Value));
      }
      return results;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  // virtual IJSRDotNET GetPluginLibraryInstance(std::string strPluginName) =
  // 0;

  JSRLibMetadata GetPluginLibraryMetadata(std::string strPluginName) {
    try {

      auto metadata = m_manager->dotNETManager->GetPluginLibraryMetadata(
          marshal_as<String ^>(strPluginName));
      return libMetadataFromManaged(metadata);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  // virtual std::string GetPluginNameFromLibraryInstance(IJSRDotNET lib) = 0;

  std::vector<std::string> GetPluginNames() {
    try {

      return listToVectorMarshall < System::String ^,
             std::string > (m_manager->dotNETManager->GetPluginNames());
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  // virtual InstrumentOpenCriteria
  // GetPluginOpenOptions(std::string strPluginName) = 0;

  int GetPulserPropertyAttributes(std::string settingName) {
    try {

      return m_manager->dotNETManager->GetPulserPropertyAttributes(
          marshal_as<String ^>(settingName));
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  C_PROPERTY_UNITS GetPulserPropertyUnits(std::string settingName) {
    try {

      return propertyUnitsFromManaged(
          m_manager->dotNETManager->GetPulserPropertyUnits(
              marshal_as<String ^>(settingName)));
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  std::string GetPulserPropertyUnitsAsString(std::string settingName,
                                             bool useShort = false) {
    try {

      return marshal_as<std::string>(
          m_manager->dotNETManager->GetPulserPropertyUnitsAsString(
              marshal_as<String ^>(settingName), useShort));
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  // TODO: Overload for other data types
  // std::string GetPulserPropertyValue(std::string strProp) {
  //  auto object =
  //      m_manager->GetPulserPropertyValue(marshal_as<String ^>(strProp));
  //}

  // std::string GetPulserPropertyValue(std::string settingName,
  //                                    PulserPropertyRoles role) {
  //   auto object = m_manager->GetPulserPropertyValue(
  //       marshal_as<String ^>(settingName),
  //       pulserPropertyRoleToManaged(role));
  // }

  // virtual IPulserReceiver GetPulserReceiver(PulserReceiverID prID) = 0;

  std::vector<std::string>
  GetPulserReceiverInfo(std::string model, std::string serialNum, int idxPR) {
    try {

      return listToVectorMarshall < System::String ^,
             std::string > (m_manager->dotNETManager->GetPulserReceiverInfo(
                               marshal_as<String ^>(model),
                               marshal_as<String ^>(serialNum), idxPR));
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  std::vector<std::string> GetPulserReceiverInfo(PulserReceiverID id) {
    return GetPulserReceiverInfo(id.InstrumentId.ModelName,
                                 id.InstrumentId.SerialNum,
                                 id.PulserReceiverIndex);
  }

  std::vector<PulserReceiverID> GetPulserReceivers() {
    try {

      auto managedReceivers =
          m_manager->dotNETManager->GetPulserReceivers(nullptr);
      std::vector<PulserReceiverID> result;
      for each (IPulserReceiverIdentity ^ receiver in managedReceivers) {
        result.push_back(pulsereceiverFromManaged(receiver));
      }
      return result;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  // virtual PulserSettingInfo GetPulserSettingInfo(std::string settingName) =
  // 0;

  bool IsPulserSettingSupported(std::string settingName) {
    try {

      return m_manager->dotNETManager->IsPulserSettingSupported(
          marshal_as<String ^>(settingName));
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void LoadPlugins(std::string pluginPath) {
    try {
      m_manager->dotNETManager->LoadPlugins(marshal_as<String ^>(pluginPath));
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void NotifyThreadProc() {
    try {
      m_manager->dotNETManager->NotifyThreadProc();
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void RemoveAllOpenOptions(std::string strPluginName) {
    try {
      m_manager->dotNETManager->RemoveAllOpenOptions(
          marshal_as<String ^>(strPluginName));
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void RemoveManagedPlugin(std::string pluginName) {
    try {

      m_manager->dotNETManager->RemoveManagedPlugin(
          marshal_as<String ^>(pluginName));
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void RequestThreadProc() {
    try {
      m_manager->dotNETManager->RequestThreadProc();
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void SetCurrentPulserReceiver(PulserReceiverID prID) {
    SetCurrentPulserReceiver(prID.InstrumentId.ModelName,
                             prID.InstrumentId.SerialNum,
                             prID.PulserReceiverIndex);
  }

  void SetCurrentPulserReceiver(std::string model, std::string serialNum,
                                int idxPR) {
    try {
      m_manager->dotNETManager->SetCurrentPulserReceiver(
          marshal_as<String ^>(model), marshal_as<String ^>(serialNum), idxPR);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void SetDiscoveryEnable(bool bEnable) {
    try {

      m_manager->dotNETManager->SetDiscoveryEnable(bEnable);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  // TODO: Overload for other data types
  void SetPulserPropertyValue(std::string strProp, const std::string &value) {
    try {
      m_manager->dotNETManager->SetPulserPropertyValue(
          marshal_as<String ^>(strProp), marshal_as<String ^>(value));
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void SetPulserPropertyValue(std::string settingName,
                              C_PULSER_PROPERTY_ROLE role,
                              const std::string &value) {
    try {

      m_manager->dotNETManager->SetPulserPropertyValue(
          marshal_as<String ^>(settingName), pulserPropertyRoleToManaged(role),
          marshal_as<String ^>(value));
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  void Shutdown() {
    try {
      m_manager->dotNETManager->Shutdown();
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  // Protected functions, so not accessible
  // virtual void addManagedPulserReceivers(IJSRDotNET lib);
  // virtual void removeManagedPulserReceivers(IJSRDotNET lib);
  // virtual void setDiscoveryEnable(object sender, bool bEnable);

  // === Getters and setters for variables in the manager ===
  bool getPulseRepetitionFrequencyIndexSupported() {
    try {
      return m_manager->dotNETManager->PulseRepetitionFrequencyIndexSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  double getHVSupplyMax() {
    try {
      return m_manager->dotNETManager->HVSupplyMax;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  double getHVSupply() {
    try {
      return m_manager->dotNETManager->HVSupply;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  void setHVSupply(double hvSupply) {
    try {

      m_manager->dotNETManager->HVSupply = hvSupply;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  bool getHVSupplySupported() {
    try {

      return m_manager->dotNETManager->HVSupplySupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  int getPulseRepetitionFrequencyNumerator() {
    try {

      return m_manager->dotNETManager->PulseRepetitionFrequencyNumerator;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  std::vector<double> getPulseRepetitionFrequencyValues() {
    try {

      return listToVector<System::Double, double>(
          m_manager->dotNETManager->PulseRepetitionFrequencyValues);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  int getPulseRepetitionFrequencyIndexMax() {
    try {

      return m_manager->dotNETManager->PulseRepetitionFrequencyIndexMax;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  int getPulseRepetitionFrequencyIndex() {
    try {

      return m_manager->dotNETManager->PulseRepetitionFrequencyIndex;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  void setPulseRepetitionFrequencyIndex(int index) {
    try {

      m_manager->dotNETManager->PulseRepetitionFrequencyIndex = index;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  double getPulseRepetitionFrequencyMin() {
    try {

      return m_manager->dotNETManager->PulseRepetitionFrequencyMin;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  double getPulseRepetitionFrequencyMax() {
    try {

      return m_manager->dotNETManager->PulseRepetitionFrequencyMax;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  double getPulseRepetitionFrequency() {
    try {

      return m_manager->dotNETManager->PulseRepetitionFrequency;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  void setPulseRepetitionFrequency(double frequency) {
    try {

      m_manager->dotNETManager->PulseRepetitionFrequency = frequency;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  C_TRIGGER_POLARITY getTriggerEdgePolarity() {
    try {

      return triggerPolarityFromManaged(
          m_manager->dotNETManager->TriggerEdgePolarity);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  void setTriggerEdgePolarity(C_TRIGGER_POLARITY polarity) {
    try {

      m_manager->dotNETManager->TriggerEdgePolarity =
          triggerPolarityToManaged(polarity);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getTriggerEdgePolaritySupported() {
    try {

      return m_manager->dotNETManager->TriggerEdgePolaritySupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  std::vector<double> getHighPassFilterValues() {
    try {

      return listToVector<System::Double, double>(
          m_manager->dotNETManager->HighPassFilterValues);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getHighPassFilterIndexMax() {
    try {

      return m_manager->dotNETManager->HighPassFilterIndexMax;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getHighPassFilterIndex() {
    try {

      return m_manager->dotNETManager->HighPassFilterIndex;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  void setHighPassFilterIndex(int index) {
    try {

      m_manager->dotNETManager->HighPassFilterIndex = index;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getLowPassFilterIndexMax() {
    try {

      return m_manager->dotNETManager->LowPassFilterIndexMax;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getLowPassFilterIndex() {
    try {

      return m_manager->dotNETManager->LowPassFilterIndex;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  void setLowPassFilterIndex(int index) {
    try {
      m_manager->dotNETManager->LowPassFilterIndex = index;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  C_RECEIVER_MODE getReceiverMode() {
    try {
      return receiverModeFromManaged(m_manager->dotNETManager->ReceiverMode);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  void setReceiverMode(C_RECEIVER_MODE mode) {
    try {
      m_manager->dotNETManager->ReceiverMode = receiverModeToManaged(mode);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getReceiverModeBothSupported() {
    try {
      return m_manager->dotNETManager->ReceiverModeBothSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getReceiverModeThruSupported() {
    try {

      return m_manager->dotNETManager->ReceiverModeThruSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getReceiverModeEchoSupported() {
    try {

      return m_manager->dotNETManager->ReceiverModeEchoSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  std::vector<double> getGainValues() {
    try {

      return listToVector<System::Double, double>(
          m_manager->dotNETManager->GainValues);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getGainIndexMax() {
    try {
      return m_manager->dotNETManager->GainIndexMax;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getGainIndex() {
    try {
      return m_manager->dotNETManager->GainIndex;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  void setGainIndex(int index) {
    try {
      m_manager->dotNETManager->GainIndex = index;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  std::vector<double> getLowPassFilterValues() {
    try {

      return listToVector<System::Double, double>(
          m_manager->dotNETManager->LowPassFilterValues);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  double getHVSupplyMin() {
    try {
      return m_manager->dotNETManager->HVSupplyMin;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getHVSupplyIndexSupported() {
    try {

      return m_manager->dotNETManager->HVSupplyIndexSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getHVSupplyIndex() {
    try {
      return m_manager->dotNETManager->HVSupplyIndex;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  void setHVSupplyIndex(int index) {
    try {

      m_manager->dotNETManager->HVSupplyIndex = index;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  double getHVMeasurement() {
    try {
      return m_manager->dotNETManager->HVMeasurement;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getHVMeasurementSupported() {
    try {

      return m_manager->dotNETManager->HVMeasurementSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  std::string getUnitModelName() {
    try {

      return marshal_as<std::string>(m_manager->dotNETManager->UnitModelName);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  void setUnitModelName(std::string name) {
    try {

      m_manager->dotNETManager->UnitModelName = marshal_as<String ^>(name);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  std::string getUnitSerialNum() {
    try {

      return marshal_as<std::string>(m_manager->dotNETManager->UnitSerialNum);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  void setUnitSerialNum(std::string serialNum) {
    try {

      m_manager->dotNETManager->UnitSerialNum = marshal_as<String ^>(serialNum);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getPulserOEMDataSupported() {
    try {

      return m_manager->dotNETManager->PulserOEMDataSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getReceiverOEMDataSupported() {
    try {

      return m_manager->dotNETManager->ReceiverOEMDataSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getUnitModelNameSupported() {
    try {

      return m_manager->dotNETManager->UnitModelNameSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getUnitSerialNumSupported() {
    try {

      return m_manager->dotNETManager->UnitSerialNumSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  C_TRIGGER_SOURCE getTriggerSource() {
    try {
      return triggerSourceFromManaged(m_manager->dotNETManager->TriggerSource);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  void setTriggerSource(C_TRIGGER_SOURCE source) {
    try {
      m_manager->dotNETManager->TriggerSource = triggerSourceToManaged(source);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  std::vector<std::string> getPulserTriggerSourceValueNames() {
    try {

      return listToVectorMarshall < System::String ^,
             std::string >
                 (m_manager->dotNETManager->PulserTriggerSourceValueNames);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getPulserTriggerSourceIndexMax() {
    try {

      return m_manager->dotNETManager->PulserTriggerSourceIndexMax;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getPulserTriggerSourceIndex() {
    try {

      return m_manager->dotNETManager->PulserTriggerSourceIndex;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  void setPulserTriggerSourceIndex(int index) {
    try {

      m_manager->dotNETManager->PulserTriggerSourceIndex = index;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getGainIndexSupported() {
    try {

      return m_manager->dotNETManager->GainIndexSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getTriggerSourceSlaveSupported() {
    try {

      return m_manager->dotNETManager->TriggerSourceSlaveSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getTriggerSourceExternalSupported() {
    try {

      return m_manager->dotNETManager->TriggerSourceExternalSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getTriggerEnable() {
    try {
      return m_manager->dotNETManager->TriggerEnable;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  void setTriggerEnable(bool enable) {
    try {

      m_manager->dotNETManager->TriggerEnable = enable;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  std::vector<std::string> getPulseEnergyValueNames() {
    try {

      return listToVectorMarshall < System::String ^,
             std::string > (m_manager->dotNETManager->PulseEnergyValueNames);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getPulseEnergyIndexMax() {
    try {

      return m_manager->dotNETManager->PulseEnergyIndexMax;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getPulseEnergyIndex() {
    try {

      return m_manager->dotNETManager->PulseEnergyIndex;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  void setPulseEnergyIndex(int index) {
    try {

      m_manager->dotNETManager->PulseEnergyIndex = index;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getPulseEnergyIndexSupported() {
    try {

      return m_manager->dotNETManager->PulseEnergyIndexSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  std::vector<double> getDampingValues() {
    try {

      return listToVector<System::Double, double>(
          m_manager->dotNETManager->DampingValues);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getDampingIndexMax() {
    try {
      return m_manager->dotNETManager->DampingIndexMax;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getDampingIndex() {
    try {
      return m_manager->dotNETManager->DampingIndex;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  void setDampingIndex(int index) {
    try {

      m_manager->dotNETManager->DampingIndex = index;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getDampingIndexSupported() {
    try {

      return m_manager->dotNETManager->DampingIndexSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  std::vector<double> getHVSupplyValues() {
    try {

      return listToVector<System::Double, double>(
          m_manager->dotNETManager->HVSupplyValues);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getHVSupplyIndexMax() {
    try {

      return m_manager->dotNETManager->HVSupplyIndexMax;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getTriggerSourceInternalSupported() {
    try {

      return m_manager->dotNETManager->TriggerSourceInternalSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getGainStepSizeSupported() {
    try {

      return m_manager->dotNETManager->GainStepSizeSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  double getGainStepSize() {
    try {
      return m_manager->dotNETManager->GainStepSize;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  double getGainMax() {
    try {
      return m_manager->dotNETManager->GainMax;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  std::string getPulserSerialNum() {
    try {

      return marshal_as<std::string>(m_manager->dotNETManager->PulserSerialNum);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  void setPulserSerialNum(std::string serialNum) {
    try {

      m_manager->dotNETManager->PulserSerialNum =
          marshal_as<String ^>(serialNum);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getPulserSerialNumSupported() {
    try {

      return m_manager->dotNETManager->PulserSerialNumSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getReceiverHWRevSupported() {
    try {

      return m_manager->dotNETManager->ReceiverHWRevSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  std::string getReceiverHWRev() {
    try {

      return marshal_as<std::string>(m_manager->dotNETManager->ReceiverHWRev);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  void setReceiverHWRev(std::string hwRev) {
    try {

      m_manager->dotNETManager->ReceiverHWRev = marshal_as<String ^>(hwRev);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getPulserHWRevSupported() {
    try {

      return m_manager->dotNETManager->PulserHWRevSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  std::string getPulserHWRev() {
    try {

      return marshal_as<std::string>(m_manager->dotNETManager->PulserHWRev);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  void setPulserHWRev(std::string hwRev) {
    try {

      m_manager->dotNETManager->PulserHWRev = marshal_as<String ^>(hwRev);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getReceiverFirmwareVerSupported() {
    try {

      return m_manager->dotNETManager->ReceiverFirmwareVerSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  std::string getReceiverFirmwareVer() {
    try {

      return marshal_as<std::string>(
          m_manager->dotNETManager->ReceiverFirmwareVer);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getPulserFirmwareVerSupported() {
    try {

      return m_manager->dotNETManager->PulserFirmwareVerSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  std::string getPulserFirmwareVer() {
    try {

      return marshal_as<std::string>(
          m_manager->dotNETManager->PulserFirmwareVer);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  double getMaxFrequency() {
    try {
      return m_manager->dotNETManager->MaxFrequency;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  C_TRIGGER_IMPEDANCE getTriggerImpedance() {
    try {

      return triggerImpedanceFromManaged(
          m_manager->dotNETManager->TriggerImpedance);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  void setTriggerImpedance(C_TRIGGER_IMPEDANCE impedance) {
    try {

      m_manager->dotNETManager->TriggerImpedance =
          triggerImpedanceToManaged(impedance);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getTriggerImpedanceSupported() {
    try {

      return m_manager->dotNETManager->TriggerImpedanceSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  C_PULSER_IMPEDANCE getPulserImpedance() {
    try {

      return pulserImpedanceFromManaged(
          m_manager->dotNETManager->PulserImpedance);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  void setPulserImpedance(C_PULSER_IMPEDANCE impedance) {
    try {

      m_manager->dotNETManager->PulserImpedance =
          pulserImpedanceToManaged(impedance);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getPulserImpedanceSupported() {
    try {

      return m_manager->dotNETManager->PulserImpedanceSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  double getEnergyPerPulse() {
    try {

      return m_manager->dotNETManager->EnergyPerPulse;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  std::vector<std::string> getInfo() {
    try {

      return listToVectorMarshall < System::String ^,
             std::string > (m_manager->dotNETManager->Info);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  C_IS_PULSING getPulserIsPulsing() {
    try {

      return isPulsingFromManaged(m_manager->dotNETManager->PulserIsPulsing);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  C_POWER_LIMIT getPulserPowerLimitStatus() {
    try {

      return powerLimitFromManaged(
          m_manager->dotNETManager->PulserPowerLimitStatus);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getPulserTriggerCount() {
    try {

      return m_manager->dotNETManager->PulserTriggerCount;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getPulserTriggerCountSupported() {
    try {

      return m_manager->dotNETManager->PulserTriggerCountSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getHVSupplyEnable() {
    try {
      return m_manager->dotNETManager->HVSupplyEnable;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  void setHVSupplyEnable(bool enable) {
    try {

      m_manager->dotNETManager->HVSupplyEnable = enable;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getHVSupplyEnableSupported() {
    try {

      return m_manager->dotNETManager->HVSupplyEnableSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  // std::vector<PulserSettingInfo> getPulserSettings() = 0;

  // PropertyChangeEventCriteria getStatusChangePropertyCriteria() = 0;
  // void setStatusChangePropertyCriteria(PropertyChangeEventCriteria
  // criteria) = 0;

  bool getReceiverSerialNumSupported() {
    try {

      return m_manager->dotNETManager->ReceiverSerialNumSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getIsPulserPresentSupported() {
    try {

      return m_manager->dotNETManager->IsPulserPresentSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  std::string getReceiverSerialNum() {
    try {

      return marshal_as<std::string>(
          m_manager->dotNETManager->ReceiverSerialNum);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  void setReceiverSerialNum(std::string serialNum) {
    try {

      m_manager->dotNETManager->ReceiverSerialNum =
          marshal_as<String ^>(serialNum);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getPulserModelNameSupported() {
    try {

      return m_manager->dotNETManager->PulserModelNameSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  double getGainMin() {
    try {
      return m_manager->dotNETManager->GainMin;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  double getGain() {
    try {
      return m_manager->dotNETManager->Gain;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  void setGain(double gain) {
    try {
      m_manager->dotNETManager->Gain = gain;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getHasManualControls() {
    try {

      return m_manager->dotNETManager->HasManualControls;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  std::vector<std::string> getLEDBlinkModeValues() {
    try {

      return listToVectorMarshall < System::String ^,
             std::string > (m_manager->dotNETManager->LEDBlinkModeValues);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getLEDBlinkModeIndexMax() {
    try {

      return m_manager->dotNETManager->LEDBlinkModeIndexMax;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  int getLEDBlinkModeIndex() {
    try {

      return m_manager->dotNETManager->LEDBlinkModeIndex;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  void setLEDBlinkModeIndex(int index) {
    try {

      m_manager->dotNETManager->LEDBlinkModeIndex = index;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getLEDBlinkModeIndexSupported() {
    try {

      return m_manager->dotNETManager->LEDBlinkModeIndexSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  PulserReceiverID getId() {
    try {

      return pulsereceiverFromManaged(m_manager->dotNETManager->Id);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getIsPulserReceiverSelected() {
    try {

      return m_manager->dotNETManager->IsPulserReceiverSelected;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  std::string getLastExceptionContextMessage() {
    try {

      return marshal_as<std::string>(
          m_manager->dotNETManager->LastExceptionContextMessage);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  std::string getLastExceptionOrNull() {
    try {

      Exception ^ exception = m_manager->dotNETManager->LastExceptionOrNull;
      if (exception != nullptr) {
        return marshal_as<std::string>(exception->Message);
      } else {
        return "";
      }
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  // void setLastExceptionOrNull(Exception exception) = 0;

  std::string getPluginPath() {
    try {

      return marshal_as<std::string>(m_manager->dotNETManager->PluginPath);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  void setPluginPath(std::string path) {
    try {

      m_manager->dotNETManager->PluginPath = marshal_as<String ^>(path);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  std::string getPulserModelName() {
    try {

      return marshal_as<std::string>(m_manager->dotNETManager->PulserModelName);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  void setPulserModelName(std::string name) {
    try {

      m_manager->dotNETManager->PulserModelName = marshal_as<String ^>(name);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getPluginsLoaded() {
    try {
      return m_manager->dotNETManager->PluginsLoaded;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  C_MANAGER_STATE getManagerState() {
    try {

      return managerStateFromManaged(m_manager->dotNETManager->ManagerState);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getPulserMaxPRFsSupported() {
    try {

      return m_manager->dotNETManager->PulserMaxPRFsSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  std::vector<double> getPulserMaxPRFs() {
    try {

      auto values = m_manager->dotNETManager->PulserMaxPRFs;
      std::vector<double> result(values->Length);

      for each (double value in values) {
        result.push_back(value);
      }
      return result;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getPulserEnergyCapacitorValuesSupported() {
    try {

      return m_manager->dotNETManager->PulserEnergyCapacitorValuesSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  std::vector<double> getPulserEnergyCapacitorValues() {
    try {

      return listToVector<System::Double, double>(
          m_manager->dotNETManager->PulserEnergyCapacitorValues);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  std::vector<std::string> getReceiverSupplyVoltages() {
    try {

      return listToVectorMarshall < System::String ^,
             std::string > (m_manager->dotNETManager->ReceiverSupplyVoltages);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getReceiverSupplyVoltagesSupported() {
    try {

      return m_manager->dotNETManager->ReceiverSupplyVoltagesSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  std::vector<unsigned char> getReceiverOEMData() {
    try {

      array<System::Byte> ^ data = m_manager->dotNETManager->ReceiverOEMData;
      if (data == nullptr)
        return {};

      std::vector<unsigned char> nativeData(data->Length);
      for (int i = 0; i < data->Length; ++i)
        nativeData[i] = data[i];

      return nativeData;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  void setReceiverOEMData(std::vector<unsigned char> data) {
    try {

      array<System::Byte> ^ managedArray =
          gcnew array<System::Byte>(static_cast<int>(data.size()));

      int index = 0;
      for (auto byte : data)
        managedArray[index++] = byte;

      m_manager->dotNETManager->ReceiverOEMData = managedArray;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  std::vector<unsigned char> getPulserOEMData() {
    try {

      array<System::Byte> ^ data = m_manager->dotNETManager->PulserOEMData;
      if (data == nullptr)
        return {};

      std::vector<unsigned char> nativeData(data->Length);
      for (int i = 0; i < data->Length; ++i)
        nativeData[i] = data[i];

      return nativeData;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  void setPulserOEMData(std::vector<unsigned char> data) {
    try {

      array<System::Byte> ^ managedArray =
          gcnew array<System::Byte>(static_cast<int>(data.size()));

      int index = 0;
      for (auto byte : data)
        managedArray[index++] = byte;

      m_manager->dotNETManager->PulserOEMData = managedArray;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getReceiverModelNameSupported() {
    try {

      return m_manager->dotNETManager->ReceiverModelNameSupported;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  std::string getReceiverModelName() {
    try {

      return marshal_as<std::string>(
          m_manager->dotNETManager->ReceiverModelName);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }
  void setReceiverModelName(std::string name) {
    try {

      m_manager->dotNETManager->ReceiverModelName = marshal_as<String ^>(name);
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getArePluginsAvailable() {
    try {

      return m_manager->dotNETManager->ArePluginsAvailable;
    } catch (System::Exception ^ exception) {
      std::string msg = marshal_as<std::string>(exception->Message);
      throw std::runtime_error("JSTdotNETSDK produced Exception: " + msg);
    }
  }

  bool getIsPulserPresent() {
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
    throw "Failed creating JSR Bridge object: " + msg;
  }
}

__declspec(dllexport) void DestroyJSRSDKManager(JSRSDKManager *bridge) {
  delete bridge;
}
}
