#pragma once

#include "JSR-SDK/InstrumentID.h"

#include <string>
#include <vector>

// This abstract class is the interface to unmanaged C++.
// The extension will contain C++/CLI code, which the unmanaged C++ code will know nothing about.
class JSRSDKManager {
public:
    // Prevent copying and moving
    JSRSDKManager(const JSRSDKManager&) = delete;
    JSRSDKManager& operator=(const JSRSDKManager&) = delete;
    JSRSDKManager(JSRSDKManager&&) = delete;
    JSRSDKManager& operator=(JSRSDKManager&&) = delete;

    // Constucts a maanger and loads plugins from the binary directory
    JSRSDKManager() = default;
    // Closes the connection to the devices and releases all resources.
    virtual ~JSRSDKManager() = default;
    // Gives the number of loaded plugins.
    virtual int GetNumberOfPlugins() = 0;
    // Gives the names of the loaded plugins.
    virtual std::vector<std::string> GetPluginNames() = 0;
    // Registers the plugin???
    virtual void AddManagedPlugin(std::string pluginName) = 0;
    // Start or stop discovery of devices.
    virtual void SetDiscoveryEnable(bool bEnable) = 0;
    // Gives the devices detected by the plugin.
    virtual std::vector<InstrumentID> GetInstruments(std::string pluginName);



    // Variables
    virtual bool getPulseRepetitionFrequencyIndexSupported() = 0;
    virtual double getHVSupplyMax() = 0;
    virtual double getHVSupply() = 0;
    virtual void setHVSupply(double supply) = 0;

    virtual bool getHVSupplySupported() = 0;
    virtual int getPulseRepetitionFrequencyNumerator() = 0;
    virtual std::vector<double> getPulseRepetitionFrequencyValues() = 0; // TODO: Make std::Array<double>
    virtual int getPulseRepetitionFrequencyIndexMax() = 0;
    virtual int getPulseRepetitionFrequencyIndex() = 0;
    virtual void setPulseRepetitionFrequencyIndex(int index) = 0;

    virtual double getPulseRepetitionFrequencyMin() = 0;
    virtual double getPulseRepetitionFrequencyMax() = 0;

    virtual double getPulseRepetitionFrequency() = 0;
    virtual void setPulseRepetitionFrequency(double frequency) = 0;

    //virtual TRIGGER_POLARITY getTriggerEdgePolarity() = 0;
    //virtual void setTriggerEdgePolarity(TRIGGER_POLARITY polarity) = 0;

    virtual bool getTriggerEdgePolaritySupported() = 0;
  
    virtual std::vector<double> getHighPassFilterValues() = 0;

    virtual int getHighPassFilterIndexMax() = 0;

    virtual int getHighPassFilterIndex() = 0;
    virtual void setHighPassFilterIndex(int index) = 0;

    virtual int getLowPassFilterIndexMax() = 0;
//
//    virtual int getLowPassFilterIndex() = 0;
//    virtual void setLowPassFilterIndex(int index) = 0;
//
//    virtual RECEIVER_MODE getReceiverMode() = 0;
//    virtual void setReceiverMode(RECEIVER_MODE mode) = 0;
//
//
//    virtual bool getReceiverModeBothSupported() = 0;
//
//    virtual bool getReceiverModeThruSupported() = 0;
//
//    virtual bool getReceiverModeEchoSupported() = 0;
//
//    virtual std::vector<double> getGainValues() = 0;
//
//    virtual int getGainIndexMax() = 0;
//
//    virtual int getGainIndex() = 0;
//    virtual void setGainIndex(int index) = 0;
//
//    virtual std::vector<double> getLowPassFilterValues() = 0;
//
//    virtual double getHVSupplyMin() = 0;
//
//    virtual bool getHVSupplyIndexSupported() = 0;
//
//    virtual int getHVSupplyIndex() = 0;
//    virtual void setHVSupplyIndex(int index) = 0;
//
//    virtual double getHVMeasurement() = 0;
//
//    virtual bool getHVMeasurementSupported() = 0;
//
//    virtual std::string getUnitModelName() = 0;
//    virtual void setUnitModelName(std::string name) = 0;
//
//    virtual std::string getUnitSerialNum() = 0;
//    virtual void setUnitSerialNum(std::string serialNum) = 0;
//
//    virtual bool getPulserOEMDataSupported() = 0;
//
//    virtual bool getReceiverOEMDataSupported() = 0;
//
//    virtual bool getUnitModelNameSupported() = 0;
//
//    virtual bool getUnitSerialNumSupported() = 0;
//
//    virtual TRIGGER_SOURCE getTriggerSource() = 0;
//    virtual void setTriggerSource(TRIGGER_SOURCE source) = 0;
//
//    virtual std::vector<std::string> getPulserTriggerSourceValueNames() = 0;
//
//    virtual int getPulserTriggerSourceIndexMax() = 0;
//
//    virtual int getPulserTriggerSourceIndex() = 0;
//    virtual void setPulserTriggerSourceIndex(int index) = 0;
//
//    virtual bool getGainIndexSupported() = 0;
//
//    virtual bool getTriggerSourceSlaveSupported() = 0;
// 
//    virtual bool getTriggerSourceExternalSupported() = 0;
//
//    virtual bool getTriggerEnable() = 0;
//    virtual void setTriggerEnable(bool enable) = 0;
//
//    virtual std::vector<std::string> getPulseEnergyValueNames() = 0;
//
//    virtual int getPulseEnergyIndexMax() = 0;
//
//    virtual int getPulseEnergyIndex() = 0;
//    virtual void setPulseEnergyIndex(int index) = 0;
//
//    virtual bool getPulseEnergyIndexSupported() = 0;
//
//    virtual std::vector<double> getDampingValues() = 0;
//
//    virtual int getDampingIndexMax() = 0;
//
//    virtual int getDampingIndex() = 0;
//    virtual void setDampingIndex(int index) = 0;
//
//    virtual bool getDampingIndexSupported() = 0;
//
//    virtual std::vector<double> getHVSupplyValues() = 0;
//
//    virtual int getHVSupplyIndexMax() = 0;
//
//    virtual bool getTriggerSourceInternalSupported() = 0;
//
//    virtual bool getGainStepSizeSupported() = 0;
//
//    virtual double getGainStepSize() = 0;
//
//    virtual double getGainMax() = 0;
//
//    virtual std::string getPulserSerialNum() = 0;
//    virtual void setPulserSerialNum(std::string serialNum) = 0;
//
//    virtual bool getPulserSerialNumSupported() = 0;
//
//    virtual bool getReceiverHWRevSupported() = 0;
//
//    virtual std::string getReceiverHWRev() = 0;
//    virtual void setReceiverHWRev(std::string hwRev) = 0;
//
//    virtual bool getPulserHWRevSupported() = 0;
//
//    virtual std::string getPulserHWRev() = 0;
//    virtual void setPulserHWRev(std::string hwRev) = 0;
//
//    virtual bool getReceiverFirmwareVerSupported() = 0;
//
//    virtual std::string getReceiverFirmwareVer() = 0;
//
//    virtual bool getPulserFirmwareVerSupported() = 0;
//
//    virtual std::string getPulserFirmwareVer() = 0;
//
//    virtual double getMaxFrequency() = 0;
//
//    virtual TRIGGER_IMPEDANCE getTriggerImpedance() = 0;
//    virtual void setTriggerImpedance(TRIGGER_IMPEDANCE impedance) = 0;
//
//    virtual bool getTriggerImpedanceSupported() = 0;
//
//    virtual PULSER_IMPEDANCE getPulserImpedance() = 0;
//    virtual void setPulserImpedance(PULSER_IMPEDANCE impedance) = 0;
//
//    virtual bool getPulserImpedanceSupported() = 0;
//
//    virtual double getEnergyPerPulse() = 0;
//
//    virtual std::vector<std::string> getInfo() = 0;
//
//    virtual IS_PULSING getPulserIsPulsing() = 0;
//
//    virtual POWER_LIMIT getPulserPowerLimitStatus() = 0;
//
//    virtual int getPulserTriggerCount() = 0;
//
//    virtual bool getPulserTriggerCountSupported() = 0;
//
//    virtual bool getHVSupplyEnable() = 0;
//    virtual void setHVSupplyEnable(bool enable) = 0;
//
//
//    virtual bool getHVSupplyEnableSupported() = 0;
//
//    virtual std::vector<PulserSettingInfo> getPulserSettings() = 0;
//
//    virtual PropertyChangeEventCriteria getStatusChangePropertyCriteria() = 0;
//    virtual void setStatusChangePropertyCriteria(PropertyChangeEventCriteria criteria) = 0;
//
//    virtual bool getReceiverSerialNumSupported() = 0;
//
//    virtual bool getIsPulserPresentSupported() = 0;
//
//    virtual std::string getReceiverSerialNum() = 0;
//    virtual void setReceiverSerialNum(std::string serialNum) = 0;
//
//    virtual bool getPulserModelNameSupported() = 0;
//
//    virtual double getGainMin() = 0;
//
//    virtual double getGain() = 0;
//    virtual void setGain(double gain) = 0;
//
//    virtual bool getHasManualControls() = 0;
//
//    virtual std::vector<std::string> getLEDBlinkModeValues() = 0;
//
//    virtual int getLEDBlinkModeIndexMax() = 0;
//
//    virtual int getLEDBlinkModeIndex() = 0;
//    virtual void setLEDBlinkModeIndex(int index) = 0;
//
//    virtual bool getLEDBlinkModeIndexSupported() = 0;
//
//    //virtual IPulserReceiverIdentity getId() = 0;
//
//    virtual bool getIsPulserReceiverSelected() = 0;
//
//    virtual std::string getLastExceptionContextMessage() = 0;
//
//    virtual Exception getLastExceptionOrNull() = 0;
//    virtual void setLastExceptionOrNull(Exception exception) = 0;
//
//    virtual std::string getPluginPath() = 0;
//    virtual void setPluginPath(std::string path) = 0;
//
//    virtual std::string getPulserModelName() = 0;
//    virtual void setPulserModelName(std::string name) = 0;
//
//
//    virtual bool getPluginsLoaded() = 0;
//
//    //virtual MANAGER_STATE getManagerState() = 0;
//    //virtual void setManagerState(MANAGER_STATE state) = 0;
//
//    virtual bool getPulserMaxPRFsSupported() = 0;
//
//    virtual std::vector<double> getPulserMaxPRFs() = 0;
//
//    virtual bool getPulserEnergyCapacitorValuesSupported() = 0;
//
//    virtual std::vector<double> getPulserEnergyCapacitorValues() = 0;
//
//    virtual std::vector<std::string> getReceiverSupplyVoltages() = 0;
//
//    virtual bool getReceiverSupplyVoltagesSupported() = 0;
//
//    virtual std::vector<byte> getReceiverOEMData() = 0;
//    virtual void setReceiverOEMData(std::vector<byte> date) = 0;
//
//
//    virtual std::vector<byte> getPulserOEMData() = 0;
//    virtual void setPulserOEMData(std::vector<byte> data) = 0;
//
//    virtual bool getReceiverModelNameSupported() = 0;
//
//    virtual std::string getReceiverModelName() = 0;
//    virtual void setReceiverModelName(std::string name) = 0;
//
//    virtual bool getArePluginsAvailable() = 0;
//
//    virtual bool getIsPulserPresent() = 0;

};

extern "C" {
    // Factory function to create the manager
    __declspec(dllexport) JSRSDKManager* CreateJSRSDKManager();

    // Explicitly destroy the manager
    __declspec(dllexport) void DestroyJSRSDKManager(JSRSDKManager* bridge);
}
