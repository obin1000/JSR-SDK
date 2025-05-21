#include "JSR-SDK/JSRSDKManager.h"
#include "MarshalTypes.cpp"
#include <msclr/marshal_cppstd.h>
#include <vcclr.h>

using namespace System;
using namespace JSRDotNETSDK;
using namespace msclr::interop;
using namespace System::Reflection;
using namespace System::IO;

class JSRSDKManagerWrapper : public JSRSDKManager {
public:
  JSRSDKManagerWrapper() {
    try {
      m_manager = gcnew JSRDotNETManager("");
      String ^ dllPath = Assembly::GetExecutingAssembly()->Location;
      String ^ dllFolder = Path::GetDirectoryName(dllPath);

      Console::WriteLine("Reading plugins from: " + dllFolder);
      m_manager->LoadPlugins(dllFolder);
    } catch (System::Exception ^ exception) {
      std::string msg =
          msclr::interop::marshal_as<std::string>(exception->Message);
      throw msg;
    }
  }

  ~JSRSDKManagerWrapper() override {
    if (static_cast<JSRDotNETManager ^>(m_manager) != nullptr) {
      m_manager->Shutdown();
      m_manager = nullptr;
    }
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

  void AddManagedPlugin(std::string pluginName) {
    m_manager->AddManagedPlugin(marshal_as<String ^>(pluginName));
  }

  void SetDiscoveryEnable(bool bEnable) {
    m_manager->SetDiscoveryEnable(bEnable);
  }

  std::vector<InstrumentID> GetInstruments(std::string pluginName) {
    std::vector<InstrumentID> result;
    auto instruments =
        m_manager->GetInstruments(marshal_as<String ^>(pluginName));
    for each (IInstrumentIdentity ^ instrument in instruments) {
      result.push_back(instrumentFromManaged(instrument));
    }
    return result;
  }
  std::vector<PulserReceiverID> GetPulserReceivers() {
    std::vector<PulserReceiverID> result;
    auto pulsereceivers = m_manager->GetPulserReceivers(nullptr);

    for each (IPulserReceiverIdentity ^ pulsereceiver in pulsereceivers) {
      result.push_back(pulsereceiverFromManaged(pulsereceiver));
    }
    return result;
  }

  void SetCurrentPulserReceiver(PulserReceiverID prID) {
    SetCurrentPulserReceiver(prID.InstrumentId.ModelName,
                             prID.InstrumentId.SerialNum,
                             prID.PulserReceiverIndex);
  }
  void SetCurrentPulserReceiver(std::string model, std::string serialNum,
                                int idxPR) {
    m_manager->SetCurrentPulserReceiver(marshal_as<String ^>(model),
                                        marshal_as<String ^>(serialNum), idxPR);
  }

  // Variables
  bool getPulseRepetitionFrequencyIndexSupported() {
    return m_manager->PulseRepetitionFrequencyIndexSupported;
  }
  double getHVSupplyMax() { return m_manager->HVSupplyMax; }
  double getHVSupply() { return m_manager->HVSupply; }
  void setHVSupply(double hvSupply) { m_manager->HVSupply = hvSupply; }
  bool getHVSupplySupported() { return m_manager->HVSupplySupported; }
  int getPulseRepetitionFrequencyNumerator() {
    return m_manager->PulseRepetitionFrequencyNumerator;
  }
  std::vector<double> getPulseRepetitionFrequencyValues() {
    std::vector<double> result;
    auto values = m_manager->PulseRepetitionFrequencyValues;
    for each (double value in values) {
      result.push_back(value);
    }
    return result;
  }
  int getPulseRepetitionFrequencyIndexMax() {
    return m_manager->PulseRepetitionFrequencyIndexMax;
  }
  int getPulseRepetitionFrequencyIndex() {
    return m_manager->PulseRepetitionFrequencyIndex;
  }
  void setPulseRepetitionFrequencyIndex(int index) {
    m_manager->PulseRepetitionFrequencyIndex = index;
  }

  double getPulseRepetitionFrequencyMin() {
    return m_manager->PulseRepetitionFrequencyMin;
  }
  double getPulseRepetitionFrequencyMax() {
    return m_manager->PulseRepetitionFrequencyMax;
  }

  double getPulseRepetitionFrequency() {
    return m_manager->PulseRepetitionFrequency;
  }
  void setPulseRepetitionFrequency(double frequency) {
    m_manager->PulseRepetitionFrequency = frequency;
  }

  TriggerPolarity getTriggerEdgePolarity() {
    return triggerPolarityFromManaged(m_manager->TriggerEdgePolarity);
  }
  void setTriggerEdgePolarity(TriggerPolarity polarity) {
    m_manager->TriggerEdgePolarity = triggerPolarityToManaged(polarity);
  }

  bool getTriggerEdgePolaritySupported() {
    return m_manager->TriggerEdgePolaritySupported;
  }

  std::vector<double> getHighPassFilterValues() {
    std::vector<double> result;
    auto values = m_manager->HighPassFilterValues;
    for each (double value in values) {
      result.push_back(value);
    }
    return result;
  }

  int getHighPassFilterIndexMax() { return m_manager->HighPassFilterIndexMax; }

  int getHighPassFilterIndex() { return m_manager->HighPassFilterIndex; }
  void setHighPassFilterIndex(int index) {
    m_manager->HighPassFilterIndex = index;
  }

  int getLowPassFilterIndexMax() { return m_manager->LowPassFilterIndexMax; }

  int getLowPassFilterIndex() { return m_manager->LowPassFilterIndex; }
  void setLowPassFilterIndex(int index) {
    m_manager->LowPassFilterIndex = index;
  }

  RecieverMode getReceiverMode() {
    return receiverModeFromManaged(m_manager->ReceiverMode);
  }
  void setReceiverMode(RecieverMode mode) {
    m_manager->ReceiverMode = receiverModeToManaged(mode);
  }

  bool getReceiverModeBothSupported() {
    return m_manager->ReceiverModeBothSupported;
  }

  bool getReceiverModeThruSupported() {
    return m_manager->ReceiverModeThruSupported;
  }

  bool getReceiverModeEchoSupported() {
    return m_manager->ReceiverModeEchoSupported;
  }

  std::vector<double> getGainValues() {
    std::vector<double> result;
    auto values = m_manager->GainValues;
    for each (double value in values) {
      result.push_back(value);
    }
    return result;
  }

  int getGainIndexMax() { return m_manager->GainIndexMax; }

  int getGainIndex() { return m_manager->GainIndex; }
  void setGainIndex(int index) { m_manager->GainIndex = index; }

  std::vector<double> getLowPassFilterValues() {
    std::vector<double> result;
    auto values = m_manager->LowPassFilterValues;
    for each (double value in values) {
      result.push_back(value);
    }
    return result;
  }

  double getHVSupplyMin() { return m_manager->HVSupplyMin; }

  bool getHVSupplyIndexSupported() { return m_manager->HVSupplyIndexSupported; }

  int getHVSupplyIndex() { return m_manager->HVSupplyIndex; }
  void setHVSupplyIndex(int index) { m_manager->HVSupplyIndex = index; }

  double getHVMeasurement() { return m_manager->HVMeasurement; }

  bool getHVMeasurementSupported() { return m_manager->HVMeasurementSupported; }

  std::string getUnitModelName() {
    return marshal_as<std::string>(m_manager->UnitModelName);
  }
  void setUnitModelName(std::string name) {
    m_manager->UnitModelName = marshal_as<String ^>(name);
  }

  std::string getUnitSerialNum() {
    return marshal_as<std::string>(m_manager->UnitSerialNum);
  }
  void setUnitSerialNum(std::string serialNum) {
    m_manager->UnitSerialNum = marshal_as<String ^>(serialNum);
  }

  bool getPulserOEMDataSupported() { return m_manager->PulserOEMDataSupported; }

  bool getReceiverOEMDataSupported() {
    return m_manager->ReceiverOEMDataSupported;
  }

  bool getUnitModelNameSupported() { return m_manager->UnitModelNameSupported; }

  bool getUnitSerialNumSupported() { return m_manager->UnitSerialNumSupported; }

  TriggerSource getTriggerSource() {
    return triggerSourceFromManaged(m_manager->TriggerSource);
  }
  void setTriggerSource(TriggerSource source) {
    m_manager->TriggerSource = triggerSourceToManaged(source);
  }

  std::vector<std::string> getPulserTriggerSourceValueNames() {
    std::vector<std::string> result;
    auto names = m_manager->PulserTriggerSourceValueNames;
    for each (String ^ name in names) {
      result.push_back(marshal_as<std::string>(name));
    }
    return result;
  }

  int getPulserTriggerSourceIndexMax() {
    return m_manager->PulserTriggerSourceIndexMax;
  }

  int getPulserTriggerSourceIndex() {
    return m_manager->PulserTriggerSourceIndex;
  }
  void setPulserTriggerSourceIndex(int index) {
    m_manager->PulserTriggerSourceIndex = index;
  }

  bool getGainIndexSupported() { return m_manager->GainIndexSupported; }

  bool getTriggerSourceSlaveSupported() {
    return m_manager->TriggerSourceSlaveSupported;
  }

  bool getTriggerSourceExternalSupported() {
    return m_manager->TriggerSourceExternalSupported;
  }

  bool getTriggerEnable() { return m_manager->TriggerEnable; }
  void setTriggerEnable(bool enable) { m_manager->TriggerEnable = enable; }

  std::vector<std::string> getPulseEnergyValueNames() {
    std::vector<std::string> result;
    auto names = m_manager->PulseEnergyValueNames;
    for each (String ^ name in names) {
      result.push_back(marshal_as<std::string>(name));
    }
    return result;
  }

  int getPulseEnergyIndexMax() { return m_manager->PulseEnergyIndexMax; }

  int getPulseEnergyIndex() { return m_manager->PulseEnergyIndex; }
  void setPulseEnergyIndex(int index) { m_manager->PulseEnergyIndex = index; }

  bool getPulseEnergyIndexSupported() {
    return m_manager->PulseEnergyIndexSupported;
  }

  std::vector<double> getDampingValues() {
    std::vector<double> result;
    auto values = m_manager->DampingValues;
    for each (double value in values) {
      result.push_back(value);
    }
    return result;
  }

  int getDampingIndexMax() { return m_manager->DampingIndexMax; }

  int getDampingIndex() { return m_manager->DampingIndex; }
  void setDampingIndex(int index) { m_manager->DampingIndex = index; }

  bool getDampingIndexSupported() { return m_manager->DampingIndexSupported; }

  std::vector<double> getHVSupplyValues() {
    std::vector<double> result;
    auto values = m_manager->HVSupplyValues;
    for each (double value in values) {
      result.push_back(value);
    }
    return result;
  }

  int getHVSupplyIndexMax() { return m_manager->HVSupplyIndexMax; }

  bool getTriggerSourceInternalSupported() {
    return m_manager->TriggerSourceInternalSupported;
  }

  bool getGainStepSizeSupported() { return m_manager->GainStepSizeSupported; }

  double getGainStepSize() { return m_manager->GainStepSize; }

  double getGainMax() { return m_manager->GainMax; }

  std::string getPulserSerialNum() {
    return marshal_as<std::string>(m_manager->PulserSerialNum);
  }
  void setPulserSerialNum(std::string serialNum) {
    m_manager->PulserSerialNum = marshal_as<String ^>(serialNum);
  }

  bool getPulserSerialNumSupported() {
    return m_manager->PulserSerialNumSupported;
  }

  bool getReceiverHWRevSupported() { return m_manager->ReceiverHWRevSupported; }

  std::string getReceiverHWRev() {
    return marshal_as<std::string>(m_manager->ReceiverHWRev);
  }
  void setReceiverHWRev(std::string hwRev) {
    m_manager->ReceiverHWRev = marshal_as<String ^>(hwRev);
  }

  bool getPulserHWRevSupported() { return m_manager->PulserHWRevSupported; }

  std::string getPulserHWRev() {
    return marshal_as<std::string>(m_manager->PulserHWRev);
  }
  void setPulserHWRev(std::string hwRev) {
    m_manager->PulserHWRev = marshal_as<String ^>(hwRev);
  }

  bool getReceiverFirmwareVerSupported() {
    return m_manager->ReceiverFirmwareVerSupported;
  }

  std::string getReceiverFirmwareVer() {
    return marshal_as<std::string>(m_manager->ReceiverFirmwareVer);
  }

  bool getPulserFirmwareVerSupported() {
    return m_manager->PulserFirmwareVerSupported;
  }

  std::string getPulserFirmwareVer() {
    return marshal_as<std::string>(m_manager->PulserFirmwareVer);
  }

  double getMaxFrequency() { return m_manager->MaxFrequency; }

  TriggerImpedance getTriggerImpedance() {
    return triggerImpedanceFromManaged(m_manager->TriggerImpedance);
  }
  void setTriggerImpedance(TriggerImpedance impedance) {
    m_manager->TriggerImpedance = triggerImpedanceToManaged(impedance);
  }

  bool getTriggerImpedanceSupported() {
    return m_manager->TriggerImpedanceSupported;
  }

  PulserImpedance getPulserImpedance() {
    return pulserImpedanceFromManaged(m_manager->PulserImpedance);
  }
  void setPulserImpedance(PulserImpedance impedance) {
    m_manager->PulserImpedance = pulserImpedanceToManaged(impedance);
  }

  bool getPulserImpedanceSupported() {
    return m_manager->PulserImpedanceSupported;
  }

  double getEnergyPerPulse() { return m_manager->EnergyPerPulse; }

  std::vector<std::string> getInfo() {
    std::vector<std::string> result;
    auto info = m_manager->Info;
    for each (String ^ str in info) {
      result.push_back(marshal_as<std::string>(str));
    }
    return result;
  }

  IsPulsing getPulserIsPulsing() {
    return isPulsingFromManaged(m_manager->PulserIsPulsing);
  }

  PowerLimit getPulserPowerLimitStatus() {
    return powerLimitFromManaged(m_manager->PulserPowerLimitStatus);
  }

  int getPulserTriggerCount() { return m_manager->PulserTriggerCount; }

  bool getPulserTriggerCountSupported() {
    return m_manager->PulserTriggerCountSupported;
  }

  bool getHVSupplyEnable() { return m_manager->HVSupplyEnable; }
  void setHVSupplyEnable(bool enable) { m_manager->HVSupplyEnable = enable; }

  bool getHVSupplyEnableSupported() {
    return m_manager->HVSupplyEnableSupported;
  }

  // std::vector<PulserSettingInfo> getPulserSettings() = 0;

  // PropertyChangeEventCriteria getStatusChangePropertyCriteria() = 0;
  // void setStatusChangePropertyCriteria(PropertyChangeEventCriteria criteria)
  // = 0;

  bool getReceiverSerialNumSupported() {
    return m_manager->ReceiverSerialNumSupported;
  }

  bool getIsPulserPresentSupported() {
    return m_manager->IsPulserPresentSupported;
  }

  std::string getReceiverSerialNum() {
    return marshal_as<std::string>(m_manager->ReceiverSerialNum);
  }
  void setReceiverSerialNum(std::string serialNum) {
    m_manager->ReceiverSerialNum = marshal_as<String ^>(serialNum);
  }

  bool getPulserModelNameSupported() {
    return m_manager->PulserModelNameSupported;
  }

  double getGainMin() { return m_manager->GainMin; }

  double getGain() { return m_manager->Gain; }
  void setGain(double gain) { m_manager->Gain = gain; }

  bool getHasManualControls() { return m_manager->HasManualControls; }

  std::vector<std::string> getLEDBlinkModeValues() {
    std::vector<std::string> result;
    auto values = m_manager->LEDBlinkModeValues;
    for each (String ^ value in values) {
      result.push_back(marshal_as<std::string>(value));
    }
    return result;
  }

  int getLEDBlinkModeIndexMax() { return m_manager->LEDBlinkModeIndexMax; }

  int getLEDBlinkModeIndex() { return m_manager->LEDBlinkModeIndex; }
  void setLEDBlinkModeIndex(int index) { m_manager->LEDBlinkModeIndex = index; }

  bool getLEDBlinkModeIndexSupported() {
    return m_manager->LEDBlinkModeIndexSupported;
  }

  PulserReceiverID getId() { return pulsereceiverFromManaged(m_manager->Id); }

  bool getIsPulserReceiverSelected() {
    return m_manager->IsPulserReceiverSelected;
  }

  std::string getLastExceptionContextMessage() {
    return marshal_as<std::string>(m_manager->LastExceptionContextMessage);
  }

  std::string getLastExceptionOrNull() {
    Exception ^ exception = m_manager->LastExceptionOrNull;
    if (exception != nullptr) {
      return marshal_as<std::string>(exception->Message);
    } else {
      return "";
    }
  }
  // void setLastExceptionOrNull(Exception exception) = 0;

  std::string getPluginPath() {
    return marshal_as<std::string>(m_manager->PluginPath);
  }
  void setPluginPath(std::string path) {
    m_manager->PluginPath = marshal_as<String ^>(path);
  }

  std::string getPulserModelName() {
    return marshal_as<std::string>(m_manager->PulserModelName);
  }
  void setPulserModelName(std::string name) {
    m_manager->PulserModelName = marshal_as<String ^>(name);
  }

  bool getPluginsLoaded() { return m_manager->PluginsLoaded; }

  ManagerState getManagerState() {
    return managerStateFromManaged(m_manager->ManagerState);
  }

  bool getPulserMaxPRFsSupported() { return m_manager->PulserMaxPRFsSupported; }

  std::vector<double> getPulserMaxPRFs() {
    std::vector<double> result;
    auto values = m_manager->PulserMaxPRFs;
    for each (double value in values) {
      result.push_back(value);
    }
    return result;
  }

  bool getPulserEnergyCapacitorValuesSupported() {
    return m_manager->PulserEnergyCapacitorValuesSupported;
  }

  std::vector<double> getPulserEnergyCapacitorValues() {
    std::vector<double> result;
    auto values = m_manager->PulserEnergyCapacitorValues;
    for each (double value in values) {
      result.push_back(value);
    }
    return result;
  }

  std::vector<std::string> getReceiverSupplyVoltages() {
    std::vector<std::string> result;
    auto values = m_manager->ReceiverSupplyVoltages;
    for each (String ^ value in values) {
      result.push_back(marshal_as<std::string>(value));
    }
    return result;
  }

  bool getReceiverSupplyVoltagesSupported() {
    return m_manager->ReceiverSupplyVoltagesSupported;
  }

  std::vector<unsigned char> getReceiverOEMData() {
    array<System::Byte> ^ data = m_manager->ReceiverOEMData;
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

    m_manager->ReceiverOEMData = managedArray;
  }

  std::vector<unsigned char> getPulserOEMData() {
    array<System::Byte> ^ data = m_manager->PulserOEMData;
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

    m_manager->PulserOEMData = managedArray;
  }

  bool getReceiverModelNameSupported() {
    return m_manager->ReceiverModelNameSupported;
  }

  std::string getReceiverModelName() {
    return marshal_as<std::string>(m_manager->ReceiverModelName);
  }
  void setReceiverModelName(std::string name) {
    m_manager->ReceiverModelName = marshal_as<String ^>(name);
  }

  bool getArePluginsAvailable() { return m_manager->ArePluginsAvailable; }

  bool getIsPulserPresent() { return m_manager->IsPulserPresent; }

private:
  gcroot<JSRDotNETManager ^> m_manager;
};

extern "C" {

__declspec(dllexport) JSRSDKManager *CreateJSRSDKManager() {
  try {
    return new JSRSDKManagerWrapper();
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
