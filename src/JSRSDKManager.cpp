#include "JSR-SDK/JSRSDKManager.h"
#include "JSRSDKWrapper.cpp"
#include "MarshalTypes.cpp"
#include <msclr/marshal.h>
#include <msclr/marshal_cppstd.h>
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
  StatusChangeCallback m_statusCallback;
  NotifyCallback m_notifyCallback;

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

  // === Event handlers used for callbacks ===
  void replaceStatusChangeEventHandler(
      const StatusChangeCallback &callback) override {
    m_statusCallback = callback;
    m_manager->SetStatusChangeCallback(&m_statusCallback);
  }

  void removeStatusChangeEventHandler() override {
    m_manager->SetStatusChangeCallback(nullptr);
  }

  void replaceNotifyEventHandler(const NotifyCallback &callback) override {
    m_notifyCallback = callback;
    m_manager->SetNotifyCallback(&m_notifyCallback);
  }

  void removeNotifyEventHandler() override {
    m_manager->SetNotifyCallback(nullptr);
  }

  // === Static functions used to generate IDs ===
  // static std::string MakeIdString(IPulserReceiverIdentity prId) = 0;

  // static std::string MakeIdString(std::string model, std::string serialNum,
  //                                    int idxPR) = 0;

  // static void ParseIdString(std::string sID, out std::string model,
  //                           out std::string serialNum,
  //                           out int idxPR) = 0;

  // === Functions provided by SDK ===
  void AddManagedPlugin(std::string pluginName) {
    m_manager->dotNETManager->AddManagedPlugin(
        marshal_as<String ^>(pluginName));
  }
  void AddPluginOpenOption(std::string strPluginName, std::string optionName,
                           std::string optionValue) {
    m_manager->dotNETManager->AddPluginOpenOption(
        marshal_as<String ^>(strPluginName), marshal_as<String ^>(optionName),
        marshal_as<String ^>(optionValue));
  }

  void AddPluginType(std::string pluginType) {
    m_manager->dotNETManager->AddPluginType(marshal_as<String ^>(pluginType));
  }

  void ForceDetach() { m_manager->dotNETManager->ForceDetach(); }

  std::vector<std::string> GetCustomSettings() {
    return listToVectorMarshall < System::String ^,
           std::string > (m_manager->dotNETManager->GetCustomSettings());
  }

  std::vector<InstrumentID> GetInstruments(std::string pluginName) {
    auto instruments = m_manager->dotNETManager->GetInstruments(
        marshal_as<String ^>(pluginName));
    std::vector<InstrumentID> result(instruments->Length);

    for each (IInstrumentIdentity ^ instrument in instruments) {
      result.push_back(instrumentFromManaged(instrument));
    }
    return result;
  }

  std::vector<std::string> GetManagedPluginNames() {
    return listToVectorMarshall < System::String ^,
           std::string > (m_manager->dotNETManager->GetManagedPluginNames());
  }

  std::map<std::string, std::vector<std::string>>
  GetPluginLibOpenOptions(std::string strPluginName) {
    auto options = m_manager->dotNETManager->GetPluginLibOpenOptions(
        marshal_as<String ^>(strPluginName));
    std::map<std::string, std::vector<std::string>> results;
    for each (KeyValuePair<String ^, List<String ^> ^> kvp in options) {
      const std::string key = marshal_as<std::string>(kvp.Key);

      results.emplace(key, listToVectorMarshall < System::String ^,
                      std::string > (kvp.Value));
    }
    return results;
  }

  // virtual IJSRDotNET GetPluginLibraryInstance(std::string strPluginName) = 0;

  JSRLibMetadata GetPluginLibraryMetadata(std::string strPluginName) {
    auto metadata = m_manager->dotNETManager->GetPluginLibraryMetadata(
        marshal_as<String ^>(strPluginName));
    return libMetadataFromManaged(metadata);
  }

  // virtual std::string GetPluginNameFromLibraryInstance(IJSRDotNET lib) = 0;

  std::vector<std::string> GetPluginNames() {
    return listToVectorMarshall < System::String ^,
           std::string > (m_manager->dotNETManager->GetPluginNames());
  }

  // virtual InstrumentOpenCriteria
  // GetPluginOpenOptions(std::string strPluginName) = 0;

  int GetPulserPropertyAttributes(std::string settingName) {
    return m_manager->dotNETManager->GetPulserPropertyAttributes(
        marshal_as<String ^>(settingName));
  }

  PropertyUnits GetPulserPropertyUnits(std::string settingName) {
    return propertyUnitsFromManaged(
        m_manager->dotNETManager->GetPulserPropertyUnits(
            marshal_as<String ^>(settingName)));
  }

  std::string GetPulserPropertyUnitsAsString(std::string settingName,
                                             bool useShort = false) {
    return marshal_as<std::string>(
        m_manager->dotNETManager->GetPulserPropertyUnitsAsString(
            marshal_as<String ^>(settingName), useShort));
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
    return listToVectorMarshall < System::String ^,
           std::string > (m_manager->dotNETManager->GetPulserReceiverInfo(
                             marshal_as<String ^>(model),
                             marshal_as<String ^>(serialNum), idxPR));
  }

  std::vector<std::string> GetPulserReceiverInfo(PulserReceiverID id) {
    return GetPulserReceiverInfo(id.InstrumentId.ModelName,
                                 id.InstrumentId.SerialNum,
                                 id.PulserReceiverIndex);
  }

  // std::vector<PulserReceiverID> GetPulserReceivers(InstrumentID instrId) = 0;

  // virtual PulserSettingInfo GetPulserSettingInfo(std::string settingName) =
  // 0;

  bool IsPulserSettingSupported(std::string settingName) {
    return m_manager->dotNETManager->IsPulserSettingSupported(
        marshal_as<String ^>(settingName));
  }

  void LoadPlugins(std::string pluginPath) {
    m_manager->dotNETManager->LoadPlugins(marshal_as<String ^>(pluginPath));
  }

  void NotifyThreadProc() { m_manager->dotNETManager->NotifyThreadProc(); }

  void RemoveAllOpenOptions(std::string strPluginName) {
    m_manager->dotNETManager->RemoveAllOpenOptions(
        marshal_as<String ^>(strPluginName));
  }

  void RemoveManagedPlugin(std::string pluginName) {
    m_manager->dotNETManager->RemoveManagedPlugin(
        marshal_as<String ^>(pluginName));
  }

  void RequestThreadProc() { m_manager->dotNETManager->RequestThreadProc(); }

  void SetCurrentPulserReceiver(PulserReceiverID prID) {
    SetCurrentPulserReceiver(prID.InstrumentId.ModelName,
                             prID.InstrumentId.SerialNum,
                             prID.PulserReceiverIndex);
  }

  void SetCurrentPulserReceiver(std::string model, std::string serialNum,
                                int idxPR) {
    m_manager->dotNETManager->SetCurrentPulserReceiver(
        marshal_as<String ^>(model), marshal_as<String ^>(serialNum), idxPR);
  }

  void SetDiscoveryEnable(bool bEnable) {
    m_manager->dotNETManager->SetDiscoveryEnable(bEnable);
  }

  // TODO: Overload for other data types
  void SetPulserPropertyValue(std::string strProp, const std::string &value) {
    m_manager->dotNETManager->SetPulserPropertyValue(
        marshal_as<String ^>(strProp), marshal_as<String ^>(value));
  }

  void SetPulserPropertyValue(std::string settingName, PulserPropertyRoles role,
                              const std::string &value) {
    m_manager->dotNETManager->SetPulserPropertyValue(
        marshal_as<String ^>(settingName), pulserPropertyRoleToManaged(role),
        marshal_as<String ^>(value));
  }

  void Shutdown() { m_manager->dotNETManager->Shutdown(); }
  // Protected functions, so not accessible
  // virtual void addManagedPulserReceivers(IJSRDotNET lib);
  // virtual void removeManagedPulserReceivers(IJSRDotNET lib);
  // virtual void setDiscoveryEnable(object sender, bool bEnable);

  // === Getters and setters for variables in the manager ===
  bool getPulseRepetitionFrequencyIndexSupported() {
    return m_manager->dotNETManager->PulseRepetitionFrequencyIndexSupported;
  }
  double getHVSupplyMax() { return m_manager->dotNETManager->HVSupplyMax; }
  double getHVSupply() { return m_manager->dotNETManager->HVSupply; }
  void setHVSupply(double hvSupply) {
    m_manager->dotNETManager->HVSupply = hvSupply;
  }
  bool getHVSupplySupported() {
    return m_manager->dotNETManager->HVSupplySupported;
  }
  int getPulseRepetitionFrequencyNumerator() {
    return m_manager->dotNETManager->PulseRepetitionFrequencyNumerator;
  }
  std::vector<double> getPulseRepetitionFrequencyValues() {
    return listToVector<System::Double, double>(
        m_manager->dotNETManager->PulseRepetitionFrequencyValues);
  }
  int getPulseRepetitionFrequencyIndexMax() {
    return m_manager->dotNETManager->PulseRepetitionFrequencyIndexMax;
  }
  int getPulseRepetitionFrequencyIndex() {
    return m_manager->dotNETManager->PulseRepetitionFrequencyIndex;
  }
  void setPulseRepetitionFrequencyIndex(int index) {
    m_manager->dotNETManager->PulseRepetitionFrequencyIndex = index;
  }

  double getPulseRepetitionFrequencyMin() {
    return m_manager->dotNETManager->PulseRepetitionFrequencyMin;
  }
  double getPulseRepetitionFrequencyMax() {
    return m_manager->dotNETManager->PulseRepetitionFrequencyMax;
  }

  double getPulseRepetitionFrequency() {
    return m_manager->dotNETManager->PulseRepetitionFrequency;
  }
  void setPulseRepetitionFrequency(double frequency) {
    m_manager->dotNETManager->PulseRepetitionFrequency = frequency;
  }

  TriggerPolarity getTriggerEdgePolarity() {
    return triggerPolarityFromManaged(
        m_manager->dotNETManager->TriggerEdgePolarity);
  }
  void setTriggerEdgePolarity(TriggerPolarity polarity) {
    m_manager->dotNETManager->TriggerEdgePolarity =
        triggerPolarityToManaged(polarity);
  }

  bool getTriggerEdgePolaritySupported() {
    return m_manager->dotNETManager->TriggerEdgePolaritySupported;
  }

  std::vector<double> getHighPassFilterValues() {
    return listToVector<System::Double, double>(
        m_manager->dotNETManager->HighPassFilterValues);
  }

  int getHighPassFilterIndexMax() {
    return m_manager->dotNETManager->HighPassFilterIndexMax;
  }

  int getHighPassFilterIndex() {
    return m_manager->dotNETManager->HighPassFilterIndex;
  }
  void setHighPassFilterIndex(int index) {
    m_manager->dotNETManager->HighPassFilterIndex = index;
  }

  int getLowPassFilterIndexMax() {
    return m_manager->dotNETManager->LowPassFilterIndexMax;
  }

  int getLowPassFilterIndex() {
    return m_manager->dotNETManager->LowPassFilterIndex;
  }
  void setLowPassFilterIndex(int index) {
    m_manager->dotNETManager->LowPassFilterIndex = index;
  }

  RecieverMode getReceiverMode() {
    return receiverModeFromManaged(m_manager->dotNETManager->ReceiverMode);
  }
  void setReceiverMode(RecieverMode mode) {
    m_manager->dotNETManager->ReceiverMode = receiverModeToManaged(mode);
  }

  bool getReceiverModeBothSupported() {
    return m_manager->dotNETManager->ReceiverModeBothSupported;
  }

  bool getReceiverModeThruSupported() {
    return m_manager->dotNETManager->ReceiverModeThruSupported;
  }

  bool getReceiverModeEchoSupported() {
    return m_manager->dotNETManager->ReceiverModeEchoSupported;
  }

  std::vector<double> getGainValues() {
    return listToVector<System::Double, double>(
        m_manager->dotNETManager->GainValues);
  }

  int getGainIndexMax() { return m_manager->dotNETManager->GainIndexMax; }

  int getGainIndex() { return m_manager->dotNETManager->GainIndex; }
  void setGainIndex(int index) { m_manager->dotNETManager->GainIndex = index; }

  std::vector<double> getLowPassFilterValues() {
    return listToVector<System::Double, double>(
        m_manager->dotNETManager->LowPassFilterValues);
  }

  double getHVSupplyMin() { return m_manager->dotNETManager->HVSupplyMin; }

  bool getHVSupplyIndexSupported() {
    return m_manager->dotNETManager->HVSupplyIndexSupported;
  }

  int getHVSupplyIndex() { return m_manager->dotNETManager->HVSupplyIndex; }
  void setHVSupplyIndex(int index) {
    m_manager->dotNETManager->HVSupplyIndex = index;
  }

  double getHVMeasurement() { return m_manager->dotNETManager->HVMeasurement; }

  bool getHVMeasurementSupported() {
    return m_manager->dotNETManager->HVMeasurementSupported;
  }

  std::string getUnitModelName() {
    return marshal_as<std::string>(m_manager->dotNETManager->UnitModelName);
  }
  void setUnitModelName(std::string name) {
    m_manager->dotNETManager->UnitModelName = marshal_as<String ^>(name);
  }

  std::string getUnitSerialNum() {
    return marshal_as<std::string>(m_manager->dotNETManager->UnitSerialNum);
  }
  void setUnitSerialNum(std::string serialNum) {
    m_manager->dotNETManager->UnitSerialNum = marshal_as<String ^>(serialNum);
  }

  bool getPulserOEMDataSupported() {
    return m_manager->dotNETManager->PulserOEMDataSupported;
  }

  bool getReceiverOEMDataSupported() {
    return m_manager->dotNETManager->ReceiverOEMDataSupported;
  }

  bool getUnitModelNameSupported() {
    return m_manager->dotNETManager->UnitModelNameSupported;
  }

  bool getUnitSerialNumSupported() {
    return m_manager->dotNETManager->UnitSerialNumSupported;
  }

  TriggerSource getTriggerSource() {
    return triggerSourceFromManaged(m_manager->dotNETManager->TriggerSource);
  }
  void setTriggerSource(TriggerSource source) {
    m_manager->dotNETManager->TriggerSource = triggerSourceToManaged(source);
  }

  std::vector<std::string> getPulserTriggerSourceValueNames() {
    return listToVectorMarshall < System::String ^,
           std::string >
               (m_manager->dotNETManager->PulserTriggerSourceValueNames);
  }

  int getPulserTriggerSourceIndexMax() {
    return m_manager->dotNETManager->PulserTriggerSourceIndexMax;
  }

  int getPulserTriggerSourceIndex() {
    return m_manager->dotNETManager->PulserTriggerSourceIndex;
  }
  void setPulserTriggerSourceIndex(int index) {
    m_manager->dotNETManager->PulserTriggerSourceIndex = index;
  }

  bool getGainIndexSupported() {
    return m_manager->dotNETManager->GainIndexSupported;
  }

  bool getTriggerSourceSlaveSupported() {
    return m_manager->dotNETManager->TriggerSourceSlaveSupported;
  }

  bool getTriggerSourceExternalSupported() {
    return m_manager->dotNETManager->TriggerSourceExternalSupported;
  }

  bool getTriggerEnable() { return m_manager->dotNETManager->TriggerEnable; }
  void setTriggerEnable(bool enable) {
    m_manager->dotNETManager->TriggerEnable = enable;
  }

  std::vector<std::string> getPulseEnergyValueNames() {
    return listToVectorMarshall < System::String ^,
           std::string > (m_manager->dotNETManager->PulseEnergyValueNames);
  }

  int getPulseEnergyIndexMax() {
    return m_manager->dotNETManager->PulseEnergyIndexMax;
  }

  int getPulseEnergyIndex() {
    return m_manager->dotNETManager->PulseEnergyIndex;
  }
  void setPulseEnergyIndex(int index) {
    m_manager->dotNETManager->PulseEnergyIndex = index;
  }

  bool getPulseEnergyIndexSupported() {
    return m_manager->dotNETManager->PulseEnergyIndexSupported;
  }

  std::vector<double> getDampingValues() {
    return listToVector<System::Double, double>(
        m_manager->dotNETManager->DampingValues);
  }

  int getDampingIndexMax() { return m_manager->dotNETManager->DampingIndexMax; }

  int getDampingIndex() { return m_manager->dotNETManager->DampingIndex; }
  void setDampingIndex(int index) {
    m_manager->dotNETManager->DampingIndex = index;
  }

  bool getDampingIndexSupported() {
    return m_manager->dotNETManager->DampingIndexSupported;
  }

  std::vector<double> getHVSupplyValues() {
    return listToVector<System::Double, double>(
        m_manager->dotNETManager->HVSupplyValues);
  }

  int getHVSupplyIndexMax() {
    return m_manager->dotNETManager->HVSupplyIndexMax;
  }

  bool getTriggerSourceInternalSupported() {
    return m_manager->dotNETManager->TriggerSourceInternalSupported;
  }

  bool getGainStepSizeSupported() {
    return m_manager->dotNETManager->GainStepSizeSupported;
  }

  double getGainStepSize() { return m_manager->dotNETManager->GainStepSize; }

  double getGainMax() { return m_manager->dotNETManager->GainMax; }

  std::string getPulserSerialNum() {
    return marshal_as<std::string>(m_manager->dotNETManager->PulserSerialNum);
  }
  void setPulserSerialNum(std::string serialNum) {
    m_manager->dotNETManager->PulserSerialNum = marshal_as<String ^>(serialNum);
  }

  bool getPulserSerialNumSupported() {
    return m_manager->dotNETManager->PulserSerialNumSupported;
  }

  bool getReceiverHWRevSupported() {
    return m_manager->dotNETManager->ReceiverHWRevSupported;
  }

  std::string getReceiverHWRev() {
    return marshal_as<std::string>(m_manager->dotNETManager->ReceiverHWRev);
  }
  void setReceiverHWRev(std::string hwRev) {
    m_manager->dotNETManager->ReceiverHWRev = marshal_as<String ^>(hwRev);
  }

  bool getPulserHWRevSupported() {
    return m_manager->dotNETManager->PulserHWRevSupported;
  }

  std::string getPulserHWRev() {
    return marshal_as<std::string>(m_manager->dotNETManager->PulserHWRev);
  }
  void setPulserHWRev(std::string hwRev) {
    m_manager->dotNETManager->PulserHWRev = marshal_as<String ^>(hwRev);
  }

  bool getReceiverFirmwareVerSupported() {
    return m_manager->dotNETManager->ReceiverFirmwareVerSupported;
  }

  std::string getReceiverFirmwareVer() {
    return marshal_as<std::string>(
        m_manager->dotNETManager->ReceiverFirmwareVer);
  }

  bool getPulserFirmwareVerSupported() {
    return m_manager->dotNETManager->PulserFirmwareVerSupported;
  }

  std::string getPulserFirmwareVer() {
    return marshal_as<std::string>(m_manager->dotNETManager->PulserFirmwareVer);
  }

  double getMaxFrequency() { return m_manager->dotNETManager->MaxFrequency; }

  TriggerImpedance getTriggerImpedance() {
    return triggerImpedanceFromManaged(
        m_manager->dotNETManager->TriggerImpedance);
  }
  void setTriggerImpedance(TriggerImpedance impedance) {
    m_manager->dotNETManager->TriggerImpedance =
        triggerImpedanceToManaged(impedance);
  }

  bool getTriggerImpedanceSupported() {
    return m_manager->dotNETManager->TriggerImpedanceSupported;
  }

  PulserImpedance getPulserImpedance() {
    return pulserImpedanceFromManaged(
        m_manager->dotNETManager->PulserImpedance);
  }
  void setPulserImpedance(PulserImpedance impedance) {
    m_manager->dotNETManager->PulserImpedance =
        pulserImpedanceToManaged(impedance);
  }

  bool getPulserImpedanceSupported() {
    return m_manager->dotNETManager->PulserImpedanceSupported;
  }

  double getEnergyPerPulse() {
    return m_manager->dotNETManager->EnergyPerPulse;
  }

  std::vector<std::string> getInfo() {
    return listToVectorMarshall < System::String ^,
           std::string > (m_manager->dotNETManager->Info);
  }

  IsPulsing getPulserIsPulsing() {
    return isPulsingFromManaged(m_manager->dotNETManager->PulserIsPulsing);
  }

  PowerLimit getPulserPowerLimitStatus() {
    return powerLimitFromManaged(
        m_manager->dotNETManager->PulserPowerLimitStatus);
  }

  int getPulserTriggerCount() {
    return m_manager->dotNETManager->PulserTriggerCount;
  }

  bool getPulserTriggerCountSupported() {
    return m_manager->dotNETManager->PulserTriggerCountSupported;
  }

  bool getHVSupplyEnable() { return m_manager->dotNETManager->HVSupplyEnable; }
  void setHVSupplyEnable(bool enable) {
    m_manager->dotNETManager->HVSupplyEnable = enable;
  }

  bool getHVSupplyEnableSupported() {
    return m_manager->dotNETManager->HVSupplyEnableSupported;
  }

  // std::vector<PulserSettingInfo> getPulserSettings() = 0;

  // PropertyChangeEventCriteria getStatusChangePropertyCriteria() = 0;
  // void setStatusChangePropertyCriteria(PropertyChangeEventCriteria criteria)
  // = 0;

  bool getReceiverSerialNumSupported() {
    return m_manager->dotNETManager->ReceiverSerialNumSupported;
  }

  bool getIsPulserPresentSupported() {
    return m_manager->dotNETManager->IsPulserPresentSupported;
  }

  std::string getReceiverSerialNum() {
    return marshal_as<std::string>(m_manager->dotNETManager->ReceiverSerialNum);
  }
  void setReceiverSerialNum(std::string serialNum) {
    m_manager->dotNETManager->ReceiverSerialNum =
        marshal_as<String ^>(serialNum);
  }

  bool getPulserModelNameSupported() {
    return m_manager->dotNETManager->PulserModelNameSupported;
  }

  double getGainMin() { return m_manager->dotNETManager->GainMin; }

  double getGain() { return m_manager->dotNETManager->Gain; }
  void setGain(double gain) { m_manager->dotNETManager->Gain = gain; }

  bool getHasManualControls() {
    return m_manager->dotNETManager->HasManualControls;
  }

  std::vector<std::string> getLEDBlinkModeValues() {
    return listToVectorMarshall < System::String ^,
           std::string > (m_manager->dotNETManager->LEDBlinkModeValues);
  }

  int getLEDBlinkModeIndexMax() {
    return m_manager->dotNETManager->LEDBlinkModeIndexMax;
  }

  int getLEDBlinkModeIndex() {
    return m_manager->dotNETManager->LEDBlinkModeIndex;
  }
  void setLEDBlinkModeIndex(int index) {
    m_manager->dotNETManager->LEDBlinkModeIndex = index;
  }

  bool getLEDBlinkModeIndexSupported() {
    return m_manager->dotNETManager->LEDBlinkModeIndexSupported;
  }

  PulserReceiverID getId() {
    return pulsereceiverFromManaged(m_manager->dotNETManager->Id);
  }

  bool getIsPulserReceiverSelected() {
    return m_manager->dotNETManager->IsPulserReceiverSelected;
  }

  std::string getLastExceptionContextMessage() {
    return marshal_as<std::string>(
        m_manager->dotNETManager->LastExceptionContextMessage);
  }

  std::string getLastExceptionOrNull() {
    Exception ^ exception = m_manager->dotNETManager->LastExceptionOrNull;
    if (exception != nullptr) {
      return marshal_as<std::string>(exception->Message);
    } else {
      return "";
    }
  }
  // void setLastExceptionOrNull(Exception exception) = 0;

  std::string getPluginPath() {
    return marshal_as<std::string>(m_manager->dotNETManager->PluginPath);
  }
  void setPluginPath(std::string path) {
    m_manager->dotNETManager->PluginPath = marshal_as<String ^>(path);
  }

  std::string getPulserModelName() {
    return marshal_as<std::string>(m_manager->dotNETManager->PulserModelName);
  }
  void setPulserModelName(std::string name) {
    m_manager->dotNETManager->PulserModelName = marshal_as<String ^>(name);
  }

  bool getPluginsLoaded() { return m_manager->dotNETManager->PluginsLoaded; }

  ManagerState getManagerState() {
    return managerStateFromManaged(m_manager->dotNETManager->ManagerState);
  }

  bool getPulserMaxPRFsSupported() {
    return m_manager->dotNETManager->PulserMaxPRFsSupported;
  }

  std::vector<double> getPulserMaxPRFs() {
    auto values = m_manager->dotNETManager->PulserMaxPRFs;
    std::vector<double> result(values->Length);

    for each (double value in values) {
      result.push_back(value);
    }
    return result;
  }

  bool getPulserEnergyCapacitorValuesSupported() {
    return m_manager->dotNETManager->PulserEnergyCapacitorValuesSupported;
  }

  std::vector<double> getPulserEnergyCapacitorValues() {
    return listToVector<System::Double, double>(
        m_manager->dotNETManager->PulserEnergyCapacitorValues);
  }

  std::vector<std::string> getReceiverSupplyVoltages() {
    return listToVectorMarshall < System::String ^,
           std::string > (m_manager->dotNETManager->ReceiverSupplyVoltages);
  }

  bool getReceiverSupplyVoltagesSupported() {
    return m_manager->dotNETManager->ReceiverSupplyVoltagesSupported;
  }

  std::vector<unsigned char> getReceiverOEMData() {
    array<System::Byte> ^ data = m_manager->dotNETManager->ReceiverOEMData;
    if (data == nullptr)
      return {};

    std::vector<unsigned char> nativeData(data->Length);
    for (int i = 0; i < data->Length; ++i)
      nativeData[i] = data[i];

    return nativeData;
  }
  void setReceiverOEMData(std::vector<unsigned char> data) {
    array<System::Byte> ^ managedArray =
        gcnew array<System::Byte>(static_cast<int>(data.size()));

    int index = 0;
    for (auto byte : data)
      managedArray[index++] = byte;

    m_manager->dotNETManager->ReceiverOEMData = managedArray;
  }

  std::vector<unsigned char> getPulserOEMData() {
    array<System::Byte> ^ data = m_manager->dotNETManager->PulserOEMData;
    if (data == nullptr)
      return {};

    std::vector<unsigned char> nativeData(data->Length);
    for (int i = 0; i < data->Length; ++i)
      nativeData[i] = data[i];

    return nativeData;
  }
  void setPulserOEMData(std::vector<unsigned char> data) {
    array<System::Byte> ^ managedArray =
        gcnew array<System::Byte>(static_cast<int>(data.size()));

    int index = 0;
    for (auto byte : data)
      managedArray[index++] = byte;

    m_manager->dotNETManager->PulserOEMData = managedArray;
  }

  bool getReceiverModelNameSupported() {
    return m_manager->dotNETManager->ReceiverModelNameSupported;
  }

  std::string getReceiverModelName() {
    return marshal_as<std::string>(m_manager->dotNETManager->ReceiverModelName);
  }
  void setReceiverModelName(std::string name) {
    m_manager->dotNETManager->ReceiverModelName = marshal_as<String ^>(name);
  }

  bool getArePluginsAvailable() {
    return m_manager->dotNETManager->ArePluginsAvailable;
  }

  bool getIsPulserPresent() {
    return m_manager->dotNETManager->IsPulserPresent;
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
