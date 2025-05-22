#pragma once

#include "JSR-SDK/InstrumentID.h"
#include "JSR-SDK/JSRLibMetadata.h"
#include "JSR-SDK/PulserReceiverID.h"
#include "JSR-SDK/enums/IsPulsing.h"
#include "JSR-SDK/enums/ManagerState.h"
#include "JSR-SDK/enums/PowerLimit.h"
#include "JSR-SDK/enums/PropertyUnits.h"
#include "JSR-SDK/enums/PulserImpedance.h"
#include "JSR-SDK/enums/PulserPropertyRoles.h"
#include "JSR-SDK/enums/ReceiverMode.h"
#include "JSR-SDK/enums/TriggerImpedance.h"
#include "JSR-SDK/enums/TriggerPolarity.h"
#include "JSR-SDK/enums/TriggerSource.h"

#include <map>
#include <string>
#include <vector>

// This abstract class is the interface to unmanaged C++.
// The extension will contain C++/CLI code, which the unmanaged C++ code will
// know nothing about.
// TODO: std::vector can be replaced with std::array or std::span if the size is
// known at compile time.
class JSRSDKManager {
public:
  // === C++ constructor and destructor ===
  // Prevent copying and moving
  JSRSDKManager(const JSRSDKManager &) = delete;
  JSRSDKManager &operator=(const JSRSDKManager &) = delete;
  JSRSDKManager(JSRSDKManager &&) = delete;
  JSRSDKManager &operator=(JSRSDKManager &&) = delete;

  // Constucts a manager and loads plugins from the binary directory
  JSRSDKManager() = default;
  // Closes the connection to the devices and releases all resources.
  virtual ~JSRSDKManager() = default;

  // === Static functions used to generate IDs ===
  // static std::string MakeIdString(IPulserReceiverIdentity prId) = 0;

  // static std::string MakeIdString(std::string model, std::string serialNum,
  //                                    int idxPR) = 0;

  // static void ParseIdString(std::string sID, out std::string model,
  //                           out std::string serialNum,
  //                           out int idxPR) = 0;

  // === Functions provided by SDK ===
  virtual void AddManagedPlugin(std::string pluginName) = 0;

  virtual void AddPluginOpenOption(std::string strPluginName,
                                   std::string optionName,
                                   std::string optionValue) = 0;

  virtual void AddPluginType(std::string pluginType) = 0;

  virtual void ForceDetach() = 0;

  virtual std::vector<std::string> GetCustomSettings() = 0;

  virtual std::vector<InstrumentID> GetInstruments(std::string pluginName) = 0;

  virtual std::vector<std::string> GetManagedPluginNames() = 0;

  virtual std::map<std::string, std::vector<std::string>>
  GetPluginLibOpenOptions(std::string strPluginName) = 0;

  // virtual IJSRDotNET GetPluginLibraryInstance(std::string strPluginName) = 0;

  virtual JSRLibMetadata
  GetPluginLibraryMetadata(std::string strPluginName) = 0;

  // virtual std::string GetPluginNameFromLibraryInstance(IJSRDotNET lib) = 0;

  virtual std::vector<std::string> GetPluginNames() = 0;

  // virtual InstrumentOpenCriteria
  // GetPluginOpenOptions(std::string strPluginName) = 0;

  virtual int GetPulserPropertyAttributes(std::string settingName) = 0;

  virtual PropertyUnits GetPulserPropertyUnits(std::string settingName) = 0;

  virtual std::string GetPulserPropertyUnitsAsString(std::string settingName,
                                                     bool useShort = false) = 0;
  // TODO: Overload for other data types
  // virtual std::string GetPulserPropertyValue(std::string strProp) = 0;

  // virtual std::string GetPulserPropertyValue(std::string settingName,
  //                                            PulserPropertyRoles role) = 0;

  // virtual IPulserReceiver GetPulserReceiver(PulserReceiverID prID) = 0;

  virtual std::vector<std::string> GetPulserReceiverInfo(std::string model,
                                                         std::string serialNum,
                                                         int idxPR) = 0;

  virtual std::vector<std::string>
  GetPulserReceiverInfo(PulserReceiverID id) = 0;

  // virtual std::vector<PulserReceiverID>
  // GetPulserReceivers(InstrumentID instrId) = 0;

  // virtual PulserSettingInfo GetPulserSettingInfo(std::string settingName) =
  // 0;

  virtual bool IsPulserSettingSupported(std::string settingName) = 0;

  virtual void LoadPlugins(std::string pluginPath) = 0;

  virtual void NotifyThreadProc() = 0;

  virtual void RemoveAllOpenOptions(std::string strPluginName) = 0;

  virtual void RemoveManagedPlugin(std::string pluginName = "") = 0;

  virtual void RequestThreadProc() = 0;

  virtual void SetCurrentPulserReceiver(PulserReceiverID prID) = 0;

  virtual void SetCurrentPulserReceiver(std::string model,
                                        std::string serialNum, int idxPR) = 0;

  virtual void SetDiscoveryEnable(bool bEnable) = 0;

  // TODO: Overload for other data types
  virtual void SetPulserPropertyValue(std::string strProp,
                                      const std::string &value) = 0;

  virtual void SetPulserPropertyValue(std::string settingName,
                                      PulserPropertyRoles role,
                                      const std::string &value) = 0;

  virtual void Shutdown() = 0;
  // Protected functions, so not accessible
  // virtual void addManagedPulserReceivers(IJSRDotNET lib);
  // virtual void removeManagedPulserReceivers(IJSRDotNET lib);
  // virtual void setDiscoveryEnable(object sender, bool bEnable);

  // === Getters and setters for variables in the manager ===
  virtual bool getPulseRepetitionFrequencyIndexSupported() = 0;
  virtual double getHVSupplyMax() = 0;
  virtual double getHVSupply() = 0;
  virtual void setHVSupply(double supply) = 0;

  virtual bool getHVSupplySupported() = 0;
  virtual int getPulseRepetitionFrequencyNumerator() = 0;
  virtual std::vector<double> getPulseRepetitionFrequencyValues() = 0;
  virtual int getPulseRepetitionFrequencyIndexMax() = 0;
  virtual int getPulseRepetitionFrequencyIndex() = 0;
  virtual void setPulseRepetitionFrequencyIndex(int index) = 0;

  virtual double getPulseRepetitionFrequencyMin() = 0;
  virtual double getPulseRepetitionFrequencyMax() = 0;

  virtual double getPulseRepetitionFrequency() = 0;
  virtual void setPulseRepetitionFrequency(double frequency) = 0;

  virtual TriggerPolarity getTriggerEdgePolarity() = 0;
  virtual void setTriggerEdgePolarity(TriggerPolarity polarity) = 0;

  virtual bool getTriggerEdgePolaritySupported() = 0;

  virtual std::vector<double> getHighPassFilterValues() = 0;

  virtual int getHighPassFilterIndexMax() = 0;

  virtual int getHighPassFilterIndex() = 0;
  virtual void setHighPassFilterIndex(int index) = 0;

  virtual int getLowPassFilterIndexMax() = 0;

  virtual int getLowPassFilterIndex() = 0;
  virtual void setLowPassFilterIndex(int index) = 0;

  virtual RecieverMode getReceiverMode() = 0;
  virtual void setReceiverMode(RecieverMode mode) = 0;

  virtual bool getReceiverModeBothSupported() = 0;

  virtual bool getReceiverModeThruSupported() = 0;

  virtual bool getReceiverModeEchoSupported() = 0;

  virtual std::vector<double> getGainValues() = 0;

  virtual int getGainIndexMax() = 0;

  virtual int getGainIndex() = 0;
  virtual void setGainIndex(int index) = 0;

  virtual std::vector<double> getLowPassFilterValues() = 0;

  virtual double getHVSupplyMin() = 0;

  virtual bool getHVSupplyIndexSupported() = 0;

  virtual int getHVSupplyIndex() = 0;
  virtual void setHVSupplyIndex(int index) = 0;

  virtual double getHVMeasurement() = 0;

  virtual bool getHVMeasurementSupported() = 0;

  virtual std::string getUnitModelName() = 0;
  virtual void setUnitModelName(std::string name) = 0;

  virtual std::string getUnitSerialNum() = 0;
  virtual void setUnitSerialNum(std::string serialNum) = 0;

  virtual bool getPulserOEMDataSupported() = 0;

  virtual bool getReceiverOEMDataSupported() = 0;

  virtual bool getUnitModelNameSupported() = 0;

  virtual bool getUnitSerialNumSupported() = 0;

  virtual TriggerSource getTriggerSource() = 0;
  virtual void setTriggerSource(TriggerSource source) = 0;

  virtual std::vector<std::string> getPulserTriggerSourceValueNames() = 0;

  virtual int getPulserTriggerSourceIndexMax() = 0;

  virtual int getPulserTriggerSourceIndex() = 0;
  virtual void setPulserTriggerSourceIndex(int index) = 0;

  virtual bool getGainIndexSupported() = 0;

  virtual bool getTriggerSourceSlaveSupported() = 0;

  virtual bool getTriggerSourceExternalSupported() = 0;

  virtual bool getTriggerEnable() = 0;
  virtual void setTriggerEnable(bool enable) = 0;

  virtual std::vector<std::string> getPulseEnergyValueNames() = 0;

  virtual int getPulseEnergyIndexMax() = 0;

  virtual int getPulseEnergyIndex() = 0;
  virtual void setPulseEnergyIndex(int index) = 0;

  virtual bool getPulseEnergyIndexSupported() = 0;

  virtual std::vector<double> getDampingValues() = 0;

  virtual int getDampingIndexMax() = 0;

  virtual int getDampingIndex() = 0;
  virtual void setDampingIndex(int index) = 0;

  virtual bool getDampingIndexSupported() = 0;

  virtual std::vector<double> getHVSupplyValues() = 0;

  virtual int getHVSupplyIndexMax() = 0;

  virtual bool getTriggerSourceInternalSupported() = 0;

  virtual bool getGainStepSizeSupported() = 0;

  virtual double getGainStepSize() = 0;

  virtual double getGainMax() = 0;

  virtual std::string getPulserSerialNum() = 0;
  virtual void setPulserSerialNum(std::string serialNum) = 0;

  virtual bool getPulserSerialNumSupported() = 0;

  virtual bool getReceiverHWRevSupported() = 0;

  virtual std::string getReceiverHWRev() = 0;
  virtual void setReceiverHWRev(std::string hwRev) = 0;

  virtual bool getPulserHWRevSupported() = 0;

  virtual std::string getPulserHWRev() = 0;
  virtual void setPulserHWRev(std::string hwRev) = 0;

  virtual bool getReceiverFirmwareVerSupported() = 0;

  virtual std::string getReceiverFirmwareVer() = 0;

  virtual bool getPulserFirmwareVerSupported() = 0;

  virtual std::string getPulserFirmwareVer() = 0;

  virtual double getMaxFrequency() = 0;

  virtual TriggerImpedance getTriggerImpedance() = 0;
  virtual void setTriggerImpedance(TriggerImpedance impedance) = 0;

  virtual bool getTriggerImpedanceSupported() = 0;

  virtual PulserImpedance getPulserImpedance() = 0;
  virtual void setPulserImpedance(PulserImpedance impedance) = 0;

  virtual bool getPulserImpedanceSupported() = 0;

  virtual double getEnergyPerPulse() = 0;

  virtual std::vector<std::string> getInfo() = 0;

  virtual IsPulsing getPulserIsPulsing() = 0;

  virtual PowerLimit getPulserPowerLimitStatus() = 0;

  virtual int getPulserTriggerCount() = 0;

  virtual bool getPulserTriggerCountSupported() = 0;

  virtual bool getHVSupplyEnable() = 0;
  virtual void setHVSupplyEnable(bool enable) = 0;

  virtual bool getHVSupplyEnableSupported() = 0;

  // virtual std::vector<PulserSettingInfo> getPulserSettings() = 0;

  // virtual PropertyChangeEventCriteria getStatusChangePropertyCriteria() = 0;
  // virtual void
  // setStatusChangePropertyCriteria(PropertyChangeEventCriteria criteria) = 0;

  virtual bool getReceiverSerialNumSupported() = 0;

  virtual bool getIsPulserPresentSupported() = 0;

  virtual std::string getReceiverSerialNum() = 0;
  virtual void setReceiverSerialNum(std::string serialNum) = 0;

  virtual bool getPulserModelNameSupported() = 0;

  virtual double getGainMin() = 0;

  virtual double getGain() = 0;
  virtual void setGain(double gain) = 0;

  virtual bool getHasManualControls() = 0;

  virtual std::vector<std::string> getLEDBlinkModeValues() = 0;

  virtual int getLEDBlinkModeIndexMax() = 0;

  virtual int getLEDBlinkModeIndex() = 0;
  virtual void setLEDBlinkModeIndex(int index) = 0;

  virtual bool getLEDBlinkModeIndexSupported() = 0;

  virtual PulserReceiverID getId() = 0;

  virtual bool getIsPulserReceiverSelected() = 0;

  virtual std::string getLastExceptionContextMessage() = 0;

  virtual std::string getLastExceptionOrNull() = 0;
  // virtual void setLastExceptionOrNull(Exception exception) = 0;

  virtual std::string getPluginPath() = 0;
  virtual void setPluginPath(std::string path) = 0;

  virtual std::string getPulserModelName() = 0;
  virtual void setPulserModelName(std::string name) = 0;

  virtual bool getPluginsLoaded() = 0;

  virtual ManagerState getManagerState() = 0;

  virtual bool getPulserMaxPRFsSupported() = 0;

  virtual std::vector<double> getPulserMaxPRFs() = 0;

  virtual bool getPulserEnergyCapacitorValuesSupported() = 0;

  virtual std::vector<double> getPulserEnergyCapacitorValues() = 0;

  virtual std::vector<std::string> getReceiverSupplyVoltages() = 0;

  virtual bool getReceiverSupplyVoltagesSupported() = 0;

  virtual std::vector<unsigned char> getReceiverOEMData() = 0;
  virtual void setReceiverOEMData(std::vector<unsigned char> date) = 0;

  virtual std::vector<unsigned char> getPulserOEMData() = 0;
  virtual void setPulserOEMData(std::vector<unsigned char> data) = 0;

  virtual bool getReceiverModelNameSupported() = 0;

  virtual std::string getReceiverModelName() = 0;
  virtual void setReceiverModelName(std::string name) = 0;

  virtual bool getArePluginsAvailable() = 0;

  virtual bool getIsPulserPresent() = 0;
};

extern "C" {
// Factory function to create the manager
__declspec(dllexport) JSRSDKManager *CreateJSRSDKManager();

// Explicitly destroy the manager
__declspec(dllexport) void DestroyJSRSDKManager(JSRSDKManager *bridge);
}
