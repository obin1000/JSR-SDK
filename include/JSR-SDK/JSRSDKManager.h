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

/**
 * @brief Abstract class representing the interface to unmanaged C++.
 * This class provides a set of virtual functions to interact with the JSR SDK.
 * The C++/CLI implementation is hidden in the implementation, allowing
 * unmanaged C++ to use this header.
 */
class JSRSDKManager {
public:
  // === C++ constructor and destructor ===
  /**
   * This object should not be copied or moved.
   **/
  JSRSDKManager(const JSRSDKManager &) = delete;
  JSRSDKManager &operator=(const JSRSDKManager &) = delete;
  JSRSDKManager(JSRSDKManager &&) = delete;
  JSRSDKManager &operator=(JSRSDKManager &&) = delete;

  /**
   * @brief Constructs a manager and loads plugins from the binary directory.
   */
  JSRSDKManager() = default;

  /**
   * @brief Closes the connection to the devices and releases all resources.
   */
  virtual ~JSRSDKManager() = default;

  // === Static functions used to generate IDs ===
  // static std::string MakeIdString(IPulserReceiverIdentity prId) = 0;

  // static std::string MakeIdString(std::string model, std::string serialNum,
  //                                    int idxPR) = 0;

  // static void ParseIdString(std::string sID, out std::string model,
  //                           out std::string serialNum,
  //                           out int idxPR) = 0;

  // === Functions provided by SDK ===

  /**
   * @brief Adds a managed plugin by its name.
   * @param pluginName The name of the plugin to add.
   */
  virtual void AddManagedPlugin(std::string pluginName) = 0;

  /**
   * @brief Adds an open option for a specific plugin.
   * @param strPluginName The name of the plugin.
   * @param optionName The name of the option.
   * @param optionValue The value of the option.
   */
  virtual void AddPluginOpenOption(std::string strPluginName,
                                   std::string optionName,
                                   std::string optionValue) = 0;

  /**
   * @brief Adds a plugin type to the manager.
   * @param pluginType The type of the plugin to add.
   */
  virtual void AddPluginType(std::string pluginType) = 0;

  /**
   * @brief Forces the detachment of all plugins and resources.
   */
  virtual void ForceDetach() = 0;

  /**
   * @brief Retrieves custom settings available in the manager.
   * @return A vector of strings representing custom settings.
   */
  virtual std::vector<std::string> GetCustomSettings() = 0;

  /**
   * @brief Retrieves a list of instruments for a specific plugin.
   * @param pluginName The name of the plugin.
   * @return A vector of InstrumentID objects representing the instruments.
   */
  virtual std::vector<InstrumentID> GetInstruments(std::string pluginName) = 0;

  /**
   * @brief Retrieves the names of all managed plugins.
   * @return A vector of strings representing the names of managed plugins.
   */
  virtual std::vector<std::string> GetManagedPluginNames() = 0;

  /**
   * @brief Retrieves the open options for a specific plugin.
   * @param strPluginName The name of the plugin.
   * @return A map where the key is the option name and the value is a vector of
   * option values.
   */
  virtual std::map<std::string, std::vector<std::string>>
  GetPluginLibOpenOptions(std::string strPluginName) = 0;

  // virtual IJSRDotNET GetPluginLibraryInstance(std::string strPluginName) = 0;

  /**
   * @brief Retrieves metadata for a specific plugin library.
   * @param strPluginName The name of the plugin.
   * @return A JSRLibMetadata object containing metadata about the plugin.
   */
  virtual JSRLibMetadata
  GetPluginLibraryMetadata(std::string strPluginName) = 0;

  // virtual std::string GetPluginNameFromLibraryInstance(IJSRDotNET lib) = 0;
  /**
   * @brief Retrieves names of the loaded plugins.
   * @return A vector of strings representing the names of the loaded plugins.
   */
  virtual std::vector<std::string> GetPluginNames() = 0;

  // virtual InstrumentOpenCriteria
  // GetPluginOpenOptions(std::string strPluginName) = 0;

  /**
   * @brief Retrieves the attributes of a specific pulser property.
   * @param settingName The name of the pulser property.
   * @return An integer representing the attributes of the property.
   */
  virtual int GetPulserPropertyAttributes(std::string settingName) = 0;

  /**
   * @brief Retrieves the units of a specific pulser property.
   * @param settingName The name of the pulser property.
   * @return A PropertyUnits enum value representing the units of the property.
   */
  virtual PropertyUnits GetPulserPropertyUnits(std::string settingName) = 0;

  /**
   * @brief Retrieves the units of a specific pulser property as a string.
   * @param settingName The name of the pulser property.
   * @param useShort Whether to use the short form of the units.
   * @return A string representing the units of the property.
   */
  virtual std::string GetPulserPropertyUnitsAsString(std::string settingName,
                                                     bool useShort = false) = 0;

  // TODO: Overload for other data types
  // virtual std::string GetPulserPropertyValue(std::string strProp) = 0;

  // virtual std::string GetPulserPropertyValue(std::string settingName,
  //                                            PulserPropertyRoles role) = 0;

  // virtual IPulserReceiver GetPulserReceiver(PulserReceiverID prID) = 0;

  /**
   * @brief Retrieves information about a pulser receiver based on its model,
   * serial number, and index.
   * @param model The model of the pulser receiver.
   * @param serialNum The serial number of the pulser receiver.
   * @param idxPR The index of the pulser receiver.
   * @return A vector of strings containing information about the pulser
   * receiver.
   */
  virtual std::vector<std::string> GetPulserReceiverInfo(std::string model,
                                                         std::string serialNum,
                                                         int idxPR) = 0;

  /**
   * @brief Retrieves information about a pulser receiver based on its ID.
   * @param id The PulserReceiverID of the pulser receiver.
   * @return A vector of strings containing information about the pulser
   * receiver.
   */
  virtual std::vector<std::string>
  GetPulserReceiverInfo(PulserReceiverID id) = 0;

  // virtual std::vector<PulserReceiverID>
  // GetPulserReceivers(InstrumentID instrId) = 0;

  // virtual PulserSettingInfo GetPulserSettingInfo(std::string settingName) =
  // 0;

  /**
   * @brief Checks if a specific pulser setting is supported.
   * @param settingName The name of the pulser setting.
   * @return True if the setting is supported, false otherwise.
   */
  virtual bool IsPulserSettingSupported(std::string settingName) = 0;

  /**
   * @brief Loads plugins from a specified path.
   * @param pluginPath The path to the plugins.
   */
  virtual void LoadPlugins(std::string pluginPath) = 0;

  /**
   * @brief Notifies the manager's thread procedure.
   */
  virtual void NotifyThreadProc() = 0;

  /**
   * @brief Removes all open options for a specific plugin.
   * @param strPluginName The name of the plugin.
   */
  virtual void RemoveAllOpenOptions(std::string strPluginName) = 0;

  /**
   * @brief Removes a managed plugin by its name.
   * @param pluginName The name of the plugin to remove. Defaults to an empty
   * string.
   */
  virtual void RemoveManagedPlugin(std::string pluginName = "") = 0;

  /**
   * @brief Requests the manager's thread procedure.
   */
  virtual void RequestThreadProc() = 0;

  /**
   * @brief Sets the current pulser receiver based on its ID.
   * @param prID The PulserReceiverID of the pulser receiver.
   */
  virtual void SetCurrentPulserReceiver(PulserReceiverID prID) = 0;

  /**
   * @brief Sets the current pulser receiver based on its model, serial number,
   * and index.
   * @param model The model of the pulser receiver.
   * @param serialNum The serial number of the pulser receiver.
   * @param idxPR The index of the pulser receiver.
   */
  virtual void SetCurrentPulserReceiver(std::string model,
                                        std::string serialNum, int idxPR) = 0;

  /**
   * @brief Enables or disables discovery mode.
   * @param bEnable True to enable discovery mode, false to disable it.
   */
  virtual void SetDiscoveryEnable(bool bEnable) = 0;

  /**
   * @brief Sets the value of a specific pulser property.
   * @param strProp The name of the property.
   * @param value The value to set for the property.
   */
  virtual void SetPulserPropertyValue(std::string strProp,
                                      const std::string &value) = 0;

  /**
   * @brief Sets the value of a specific pulser property with a role.
   * @param settingName The name of the property.
   * @param role The role of the property.
   * @param value The value to set for the property.
   */
  virtual void SetPulserPropertyValue(std::string settingName,
                                      PulserPropertyRoles role,
                                      const std::string &value) = 0;

  /**
   * @brief Shuts down the manager and releases all resources.
   */
  virtual void Shutdown() = 0;

  // Protected functions, so not accessible
  // virtual void addManagedPulserReceivers(IJSRDotNET lib);
  // virtual void removeManagedPulserReceivers(IJSRDotNET lib);
  // virtual void setDiscoveryEnable(object sender, bool bEnable);

  // === Getters and setters for variables in the manager ===
  /**
   * @brief Checks if the pulse repetition frequency index is supported.
   * @return True if the pulse repetition frequency index is supported, false
   * otherwise.
   */
  virtual bool getPulseRepetitionFrequencyIndexSupported() = 0;

  /**
   * @brief Retrieves the maximum high voltage supply value.
   * @return The maximum high voltage supply value.
   */
  virtual double getHVSupplyMax() = 0;

  /**
   * @brief Retrieves the current high voltage supply value.
   * @return The current high voltage supply value.
   */
  virtual double getHVSupply() = 0;

  /**
   * @brief Sets the high voltage supply value.
   * @param supply The high voltage supply value to set.
   */
  virtual void setHVSupply(double supply) = 0;

  /**
   * @brief Checks if the high voltage supply is supported.
   * @return True if the high voltage supply is supported, false otherwise.
   */
  virtual bool getHVSupplySupported() = 0;

  /**
   * @brief Retrieves the numerator of the pulse repetition frequency.
   * @return The numerator of the pulse repetition frequency.
   */
  virtual int getPulseRepetitionFrequencyNumerator() = 0;

  /**
   * @brief Retrieves the available pulse repetition frequency values.
   * @return A vector of available pulse repetition frequency values.
   */
  virtual std::vector<double> getPulseRepetitionFrequencyValues() = 0;

  /**
   * @brief Retrieves the maximum index of the pulse repetition frequency.
   * @return The maximum index of the pulse repetition frequency.
   */
  virtual int getPulseRepetitionFrequencyIndexMax() = 0;

  /**
   * @brief Retrieves the current index of the pulse repetition frequency.
   * @return The current index of the pulse repetition frequency.
   */
  virtual int getPulseRepetitionFrequencyIndex() = 0;

  /**
   * @brief Sets the index of the pulse repetition frequency.
   * @param index The index to set for the pulse repetition frequency.
   */
  virtual void setPulseRepetitionFrequencyIndex(int index) = 0;

  /**
   * @brief Retrieves the minimum pulse repetition frequency value.
   * @return The minimum pulse repetition frequency value.
   */
  virtual double getPulseRepetitionFrequencyMin() = 0;

  /**
   * @brief Retrieves the maximum pulse repetition frequency value.
   * @return The maximum pulse repetition frequency value.
   */
  virtual double getPulseRepetitionFrequencyMax() = 0;

  /**
   * @brief Retrieves the current pulse repetition frequency value.
   * @return The current pulse repetition frequency value.
   */
  virtual double getPulseRepetitionFrequency() = 0;

  /**
   * @brief Sets the pulse repetition frequency value.
   * @param frequency The pulse repetition frequency value to set.
   */
  virtual void setPulseRepetitionFrequency(double frequency) = 0;

  /**
   * @brief Retrieves the trigger edge polarity.
   * @return The current trigger edge polarity.
   */
  virtual TriggerPolarity getTriggerEdgePolarity() = 0;

  /**
   * @brief Sets the trigger edge polarity.
   * @param polarity The trigger edge polarity to set.
   */
  virtual void setTriggerEdgePolarity(TriggerPolarity polarity) = 0;

  /**
   * @brief Checks if the trigger edge polarity is supported.
   * @return True if the trigger edge polarity is supported, false otherwise.
   */
  virtual bool getTriggerEdgePolaritySupported() = 0;

  /**
   * @brief Retrieves the available high-pass filter values.
   * @return A vector of available high-pass filter values.
   */
  virtual std::vector<double> getHighPassFilterValues() = 0;

  /**
   * @brief Retrieves the maximum index of the high-pass filter.
   * @return The maximum index of the high-pass filter.
   */
  virtual int getHighPassFilterIndexMax() = 0;

  /**
   * @brief Retrieves the current index of the high-pass filter.
   * @return The current index of the high-pass filter.
   */
  virtual int getHighPassFilterIndex() = 0;

  /**
   * @brief Sets the index of the high-pass filter.
   * @param index The index to set for the high-pass filter.
   */
  virtual void setHighPassFilterIndex(int index) = 0;

  /**
   * @brief Retrieves the maximum index of the low-pass filter.
   * @return The maximum index of the low-pass filter.
   */
  virtual int getLowPassFilterIndexMax() = 0;

  /**
   * @brief Retrieves the current index of the low-pass filter.
   * @return The current index of the low-pass filter.
   */
  virtual int getLowPassFilterIndex() = 0;

  /**
   * @brief Sets the index of the low-pass filter.
   * @param index The index to set for the low-pass filter.
   */
  virtual void setLowPassFilterIndex(int index) = 0;

  /**
   * @brief Retrieves the current receiver mode.
   * @return The current receiver mode.
   */
  virtual RecieverMode getReceiverMode() = 0;

  /**
   * @brief Sets the receiver mode.
   * @param mode The receiver mode to set.
   */
  virtual void setReceiverMode(RecieverMode mode) = 0;

  /**
   * @brief Checks if the "Both" receiver mode is supported.
   * @return True if the "Both" receiver mode is supported, false otherwise.
   */
  virtual bool getReceiverModeBothSupported() = 0;

  /**
   * @brief Checks if the "Thru" receiver mode is supported.
   * @return True if the "Thru" receiver mode is supported, false otherwise.
   */
  virtual bool getReceiverModeThruSupported() = 0;

  /**
   * @brief Checks if the "Echo" receiver mode is supported.
   * @return True if the "Echo" receiver mode is supported, false otherwise.
   */
  virtual bool getReceiverModeEchoSupported() = 0;

  /**
   * @brief Retrieves the available gain values.
   * @return A vector of available gain values.
   */
  virtual std::vector<double> getGainValues() = 0;

  /**
   * @brief Retrieves the maximum index of the gain.
   * @return The maximum index of the gain.
   */
  virtual int getGainIndexMax() = 0;

  /**
   * @brief Retrieves the current index of the gain.
   * @return The current index of the gain.
   */
  virtual int getGainIndex() = 0;

  /**
   * @brief Sets the index of the gain.
   * @param index The index to set for the gain.
   */
  virtual void setGainIndex(int index) = 0;

  /**
   * @brief Retrieves the available low-pass filter values.
   * @return A vector of available low-pass filter values.
   */
  virtual std::vector<double> getLowPassFilterValues() = 0;

  /**
   * @brief Retrieves the minimum high voltage supply value.
   * @return The minimum high voltage supply value.
   */
  virtual double getHVSupplyMin() = 0;

  /**
   * @brief Checks if the high voltage supply index is supported.
   * @return True if the high voltage supply index is supported, false
   * otherwise.
   */
  virtual bool getHVSupplyIndexSupported() = 0;

  /**
   * @brief Retrieves the current high voltage supply index.
   * @return The current high voltage supply index.
   */
  virtual int getHVSupplyIndex() = 0;

  /**
   * @brief Sets the high voltage supply index.
   * @param index The high voltage supply index to set.
   */
  virtual void setHVSupplyIndex(int index) = 0;

  /**
   * @brief Retrieves the high voltage measurement value.
   * @return The high voltage measurement value.
   */
  virtual double getHVMeasurement() = 0;

  virtual bool getHVMeasurementSupported() = 0;

  virtual std::string getUnitModelName() = 0;
  virtual void setUnitModelName(std::string name) = 0;

  /**
   * @brief Retrieves the unit serial number.
   * @return A string representing the unit serial number.
   */
  virtual std::string getUnitSerialNum() = 0;

  /**
   * @brief Sets the unit serial number.
   * @param serialNum The unit serial number to set.
   */
  virtual void setUnitSerialNum(std::string serialNum) = 0;

  /**
   * @brief Checks if pulser OEM data is supported.
   * @return True if pulser OEM data is supported, false otherwise.
   */
  virtual bool getPulserOEMDataSupported() = 0;

  /**
   * @brief Checks if receiver OEM data is supported.
   * @return True if receiver OEM data is supported, false otherwise.
   */
  virtual bool getReceiverOEMDataSupported() = 0;

  /**
   * @brief Checks if the unit model name is supported.
   * @return True if the unit model name is supported, false otherwise.
   */
  virtual bool getUnitModelNameSupported() = 0;

  /**
   * @brief Checks if the unit serial number is supported.
   * @return True if the unit serial number is supported, false otherwise.
   */
  virtual bool getUnitSerialNumSupported() = 0;

  /**
   * @brief Retrieves the current trigger source.
   * @return The current trigger source as a TriggerSource enum value.
   */
  virtual TriggerSource getTriggerSource() = 0;

  /**
   * @brief Sets the trigger source.
   * @param source The trigger source to set as a TriggerSource enum value.
   */
  virtual void setTriggerSource(TriggerSource source) = 0;

  /**
   * @brief Retrieves the names of available pulser trigger source values.
   * @return A vector of strings representing the names of pulser trigger source
   * values.
   */
  virtual std::vector<std::string> getPulserTriggerSourceValueNames() = 0;

  /**
   * @brief Retrieves the maximum index of pulser trigger sources.
   * @return The maximum index of pulser trigger sources.
   */
  virtual int getPulserTriggerSourceIndexMax() = 0;

  /**
   * @brief Retrieves the current index of the pulser trigger source.
   * @return The current index of the pulser trigger source.
   */
  virtual int getPulserTriggerSourceIndex() = 0;

  /**
   * @brief Sets the index of the pulser trigger source.
   * @param index The index to set for the pulser trigger source.
   */
  virtual void setPulserTriggerSourceIndex(int index) = 0;

  /**
   * @brief Checks if the gain index is supported.
   * @return True if the gain index is supported, false otherwise.
   */
  virtual bool getGainIndexSupported() = 0;

  /**
   * @brief Checks if the slave trigger source is supported.
   * @return True if the slave trigger source is supported, false otherwise.
   */
  virtual bool getTriggerSourceSlaveSupported() = 0;

  /**
   * @brief Checks if the external trigger source is supported.
   * @return True if the external trigger source is supported, false otherwise.
   */
  virtual bool getTriggerSourceExternalSupported() = 0;

  /**
   * @brief Checks if the trigger is enabled.
   * @return True if the trigger is enabled, false otherwise.
   */
  virtual bool getTriggerEnable() = 0;

  /**
   * @brief Enables or disables the trigger.
   * @param enable True to enable the trigger, false to disable it.
   */
  virtual void setTriggerEnable(bool enable) = 0;

  /**
   * @brief Retrieves the names of available pulse energy values.
   * @return A vector of strings representing the names of pulse energy values.
   */
  virtual std::vector<std::string> getPulseEnergyValueNames() = 0;

  /**
   * @brief Retrieves the maximum index of pulse energy values.
   * @return The maximum index of pulse energy values.
   */
  virtual int getPulseEnergyIndexMax() = 0;

  /**
   * @brief Retrieves the current index of the pulse energy.
   * @return The current index of the pulse energy.
   */
  virtual int getPulseEnergyIndex() = 0;

  /**
   * @brief Sets the index of the pulse energy.
   * @param index The index to set for the pulse energy.
   */
  virtual void setPulseEnergyIndex(int index) = 0;

  /**
   * @brief Checks if the pulse energy index is supported.
   * @return True if the pulse energy index is supported, false otherwise.
   */
  virtual bool getPulseEnergyIndexSupported() = 0;

  /**
   * @brief Retrieves the available damping values.
   * @return A vector of doubles representing the available damping values.
   */
  virtual std::vector<double> getDampingValues() = 0;

  /**
   * @brief Retrieves the maximum index of damping values.
   * @return The maximum index of damping values.
   */
  virtual int getDampingIndexMax() = 0;

  /**
   * @brief Retrieves the current index of the damping.
   * @return The current index of the damping.
   */
  virtual int getDampingIndex() = 0;

  /**
   * @brief Sets the index of the damping.
   * @param index The index to set for the damping.
   */
  virtual void setDampingIndex(int index) = 0;

  /**
   * @brief Checks if the damping index is supported.
   * @return True if the damping index is supported, false otherwise.
   */
  virtual bool getDampingIndexSupported() = 0;

  /**
   * @brief Retrieves the available high voltage supply values.
   * @return A vector of doubles representing the available high voltage supply
   * values.
   */
  virtual std::vector<double> getHVSupplyValues() = 0;

  /**
   * @brief Retrieves the maximum index of high voltage supply values.
   * @return The maximum index of high voltage supply values.
   */
  virtual int getHVSupplyIndexMax() = 0;

  /**
   * @brief Checks if the internal trigger source is supported.
   * @return True if the internal trigger source is supported, false otherwise.
   */
  virtual bool getTriggerSourceInternalSupported() = 0;

  /**
   * @brief Checks if the gain step size is supported.
   * @return True if the gain step size is supported, false otherwise.
   */
  virtual bool getGainStepSizeSupported() = 0;

  /**
   * @brief Retrieves the gain step size.
   * @return The gain step size as a double.
   */
  virtual double getGainStepSize() = 0;

  /**
   * @brief Retrieves the maximum gain value.
   * @return The maximum gain value as a double.
   */
  virtual double getGainMax() = 0;

  /**
   * @brief Retrieves the pulser serial number.
   * @return A string representing the pulser serial number.
   */
  virtual std::string getPulserSerialNum() = 0;

  /**
   * @brief Sets the pulser serial number.
   * @param serialNum The pulser serial number to set.
   */
  virtual void setPulserSerialNum(std::string serialNum) = 0;

  /**
   * @brief Checks if the pulser serial number is supported.
   * @return True if the pulser serial number is supported, false otherwise.
   */
  virtual bool getPulserSerialNumSupported() = 0;

  /**
   * @brief Checks if the receiver hardware revision is supported.
   * @return True if the receiver hardware revision is supported, false
   * otherwise.
   */
  virtual bool getReceiverHWRevSupported() = 0;

  /**
   * @brief Retrieves the receiver hardware revision.
   * @return A string representing the receiver hardware revision.
   */
  virtual std::string getReceiverHWRev() = 0;

  /**
   * @brief Sets the receiver hardware revision.
   * @param hwRev The receiver hardware revision to set.
   */
  virtual void setReceiverHWRev(std::string hwRev) = 0;

  /**
   * @brief Checks if the pulser hardware revision is supported.
   * @return True if the pulser hardware revision is supported, false otherwise.
   */
  virtual bool getPulserHWRevSupported() = 0;

  /**
   * @brief Retrieves the pulser hardware revision.
   * @return A string representing the pulser hardware revision.
   */
  virtual std::string getPulserHWRev() = 0;

  /**
   * @brief Sets the pulser hardware revision.
   * @param hwRev The pulser hardware revision to set.
   */
  virtual void setPulserHWRev(std::string hwRev) = 0;

  /**
   * @brief Checks if the receiver firmware version is supported.
   * @return True if the receiver firmware version is supported, false
   * otherwise.
   */
  virtual bool getReceiverFirmwareVerSupported() = 0;

  /**
   * @brief Retrieves the receiver firmware version.
   * @return A string representing the receiver firmware version.
   */
  virtual std::string getReceiverFirmwareVer() = 0;

  /**
   * @brief Checks if the pulser firmware version is supported.
   * @return True if the pulser firmware version is supported, false otherwise.
   */
  virtual bool getPulserFirmwareVerSupported() = 0;

  /**
   * @brief Retrieves the pulser firmware version.
   * @return A string representing the pulser firmware version.
   */
  virtual std::string getPulserFirmwareVer() = 0;

  /**
   * @brief Retrieves the maximum frequency.
   * @return The maximum frequency as a double.
   */
  virtual double getMaxFrequency() = 0;

  /**
   * @brief Retrieves the trigger impedance.
   * @return The trigger impedance as a TriggerImpedance enum value.
   */
  virtual TriggerImpedance getTriggerImpedance() = 0;

  /**
   * @brief Sets the trigger impedance.
   * @param impedance The trigger impedance to set as a TriggerImpedance enum
   * value.
   */
  virtual void setTriggerImpedance(TriggerImpedance impedance) = 0;

  /**
   * @brief Checks if the trigger impedance is supported.
   * @return True if the trigger impedance is supported, false otherwise.
   */
  virtual bool getTriggerImpedanceSupported() = 0;

  /**
   * @brief Retrieves the pulser impedance.
   * @return The pulser impedance as a PulserImpedance enum value.
   */
  virtual PulserImpedance getPulserImpedance() = 0;

  /**
   * @brief Sets the pulser impedance.
   * @param impedance The pulser impedance to set as a PulserImpedance enum
   * value.
   */
  virtual void setPulserImpedance(PulserImpedance impedance) = 0;

  /**
   * @brief Checks if the pulser impedance is supported.
   * @return True if the pulser impedance is supported, false otherwise.
   */
  virtual bool getPulserImpedanceSupported() = 0;

  /**
   * @brief Retrieves the energy per pulse.
   * @return The energy per pulse as a double.
   */
  virtual double getEnergyPerPulse() = 0;

  /**
   * @brief Retrieves general information about the system.
   * @return A vector of strings containing general information.
   */
  virtual std::vector<std::string> getInfo() = 0;

  /**
   * @brief Retrieves the pulser's current pulsing state.
   * @return The pulsing state as an IsPulsing enum value.
   */
  virtual IsPulsing getPulserIsPulsing() = 0;

  /**
   * @brief Retrieves the pulser's power limit status.
   * @return The power limit status as a PowerLimit enum value.
   */
  virtual PowerLimit getPulserPowerLimitStatus() = 0;

  /**
   * @brief Retrieves the pulser trigger count.
   * @return The pulser trigger count as an integer.
   */
  virtual int getPulserTriggerCount() = 0;

  /**
   * @brief Checks if the pulser trigger count is supported.
   * @return True if the pulser trigger count is supported, false otherwise.
   */
  virtual bool getPulserTriggerCountSupported() = 0;

  /**
   * @brief Checks if the high voltage supply is enabled.
   * @return True if the high voltage supply is enabled, false otherwise.
   */
  virtual bool getHVSupplyEnable() = 0;

  /**
   * @brief Enables or disables the high voltage supply.
   * @param enable True to enable the high voltage supply, false to disable it.
   */
  virtual void setHVSupplyEnable(bool enable) = 0;

  /**
   * @brief Checks if enabling the high voltage supply is supported.
   * @return True if enabling the high voltage supply is supported, false
   * otherwise.
   */
  virtual bool getHVSupplyEnableSupported() = 0;
  */ virtual int getHVSupplyIndex() = 0;

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
