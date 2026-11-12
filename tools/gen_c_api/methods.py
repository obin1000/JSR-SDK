"""The C API's method list: one entry per JSRSDKManager method.

generate.py turns this list into the per-method parts of jsr_sdk.h,
src/jsr_sdk_c.cpp, jsr_sdk.hpp and test/clr/CApiParityTest.cpp. To add an SDK
method to the C API, add it to JSRSDKManager.hpp and the adapter in
src/JSRSDKManager.cpp, add one line here, and run generate.py.

Each entry is (c_name, cpp_name, inputs, output, doc):
  c_name    the C function is JSR_<c_name>.
  cpp_name  the JSRSDKManager method it wraps, and the jsr::Manager method.
  inputs    list of (kind, name). Kinds:
              str     const char *, NULL is an error
              optstr  const char *, NULL means ""
              i32     int32_t (C++: int)
              bool    int32_t 0/1 (C++: bool)
              dbl     double
              prid    const JSR_PulserReceiverID *
              enum:<C++ enum>:<C enum typedef>   int32_t
  output    None, 'bool', 'i32', 'dbl', 'enum:...', 'str' (string result),
            'strs' (string list), 'dbls' (double list) or 'retbool:<name>'
            (a bool the C++ method returns, as an int32_t out-parameter).
  doc       one-line description; defaults to the @brief in JSRSDKManager.hpp.

Functions with struct results or callbacks (JSR_Create, JSR_Destroy,
JSR_Set*Callback, JSR_GetInstruments, JSR_GetPulserReceivers,
JSR_GetPluginLibraryMetadata, JSR_GetId) are written by hand in templates/.
"""

S = 'str'
ROLE = 'enum:C_PULSER_PROPERTY_ROLE:JSR_PulserPropertyRole'
UNITS = 'enum:C_PROPERTY_UNITS:JSR_PropertyUnits'
POL = 'enum:C_TRIGGER_POLARITY:JSR_TriggerPolarity'
MODE = 'enum:C_RECEIVER_MODE:JSR_ReceiverMode'
SRC = 'enum:C_TRIGGER_SOURCE:JSR_TriggerSource'
TIMP = 'enum:C_TRIGGER_IMPEDANCE:JSR_TriggerImpedance'
PIMP = 'enum:C_PULSER_IMPEDANCE:JSR_PulserImpedance'
PULS = 'enum:C_IS_PULSING:JSR_IsPulsing'
PWR = 'enum:C_POWER_LIMIT:JSR_PowerLimit'
MST = 'enum:C_MANAGER_STATE:JSR_ManagerState'

SECTIONS = []


def section(title):
    SECTIONS.append((title, []))


def m(c_name, cpp_name=None, inputs=(), output=None, doc=None):
    SECTIONS[-1][1].append((c_name, cpp_name or (c_name[0].lower() + c_name[1:]), list(inputs), output, doc))


def sdk(c_name, inputs=(), output=None, doc=None, cpp_name=None):
    """A method whose C++ name equals its C name (the SDK's own methods)."""
    m(c_name, cpp_name or c_name, inputs, output, doc)


section('Plugins')
m('LoadPluginsFromBinaryDir', doc='Loads the plugins from the directory that holds JSR-SDK.dll.')

section('Instrument open criteria')
sdk('AddPortToExclude', [(S, 'plugin'), (S, 'port')])
sdk('GetPortsToExclude', [(S, 'plugin')], 'strs')
sdk('AddPortToInclude', [(S, 'plugin'), (S, 'port')])
sdk('GetPortsToInclude', [(S, 'plugin')], 'strs')
sdk('AddOpenOption', [(S, 'plugin'), (S, 'optionName'), (S, 'optionValue')], 'retbool:added')
sdk('GetOpenOption', [(S, 'plugin'), (S, 'optionName')], 'str')
sdk('GetOpenOptionNames', [(S, 'plugin')], 'strs')
sdk('RemoveOpenOption', [(S, 'plugin'), (S, 'optionName')], 'retbool:removed')

section('Manager')
sdk('AddManagedPlugin', [(S, 'pluginName')])
sdk('AddPluginOpenOption', [(S, 'pluginName'), (S, 'optionName'), (S, 'optionValue')])
sdk('AddPluginType', [(S, 'pluginType')])
sdk('ForceDetach')
sdk('GetCustomSettings', [], 'strs')
sdk('GetManagedPluginNames', [], 'strs')
sdk('GetPluginNames', [], 'strs')
sdk('GetPulserPropertyAttributes', [(S, 'settingName')], 'i32')
sdk('GetPulserPropertyUnits', [(S, 'settingName')], UNITS)
sdk('GetPulserPropertyUnitsAsString', [(S, 'settingName'), ('bool', 'useShort')], 'str')
sdk('GetPulserReceiverInfoByModel', [(S, 'model'), (S, 'serialNum'), ('i32', 'index')], 'strs',
    doc='Retrieves information about a pulser receiver identified by model, serial number and index.',
    cpp_name='GetPulserReceiverInfo')
sdk('GetPulserReceiverInfoById', [('prid', 'id')], 'strs',
    doc='Retrieves information about a pulser receiver identified by its ID.',
    cpp_name='GetPulserReceiverInfo')
sdk('IsPulserSettingSupported', [(S, 'settingName')], 'bool')
sdk('LoadPlugins', [(S, 'pluginPath')])
sdk('NotifyThreadProc')
sdk('RemoveAllOpenOptions', [(S, 'pluginName')])
sdk('RemoveManagedPlugin', [('optstr', 'pluginName')],
    doc='Removes a managed plugin by its name. NULL or "" removes the default plugin.')
sdk('RequestThreadProc')
sdk('SetCurrentPulserReceiverById', [('prid', 'id')],
    doc='Selects the current pulser receiver by its ID.', cpp_name='SetCurrentPulserReceiver')
sdk('SetCurrentPulserReceiverByModel', [(S, 'model'), (S, 'serialNum'), ('i32', 'index')],
    doc='Selects the current pulser receiver by model, serial number and index.',
    cpp_name='SetCurrentPulserReceiver')
sdk('SetDiscoveryEnable', [('bool', 'enable')])
sdk('SetPulserPropertyValue', [(S, 'property'), (S, 'value')])
sdk('SetPulserPropertyValueWithRole', [(S, 'property'), (ROLE, 'role'), (S, 'value')],
    doc='Sets the value of a pulser property for a specific role.', cpp_name='SetPulserPropertyValue')
sdk('Shutdown')
m('GetPluginsLoaded', output='bool')
m('GetArePluginsAvailable', output='bool')
m('GetManagerState', output=MST)
m('GetPluginPath', output='str')
m('SetPluginPath', inputs=[(S, 'path')])
m('GetIsPulserReceiverSelected', output='bool')
m('GetLastExceptionContextMessage', output='str')
m('GetLastExceptionOrNull', output='str',
  doc='Retrieves the message of the SDK\'s last exception. *required is 0 when there is none.')
m('GetInfo', output='strs')

section('Pulse repetition frequency')
m('GetPulseRepetitionFrequencyIndexSupported', output='bool')
m('GetPulseRepetitionFrequencyNumerator', output='i32')
m('GetPulseRepetitionFrequencyValues', output='dbls')
m('GetPulseRepetitionFrequencyIndexMax', output='i32')
m('GetPulseRepetitionFrequencyIndex', output='i32')
m('SetPulseRepetitionFrequencyIndex', inputs=[('i32', 'index')])
m('GetPulseRepetitionFrequencyMin', output='dbl')
m('GetPulseRepetitionFrequencyMax', output='dbl')
m('GetPulseRepetitionFrequency', output='dbl')
m('SetPulseRepetitionFrequency', inputs=[('dbl', 'frequency')])
m('GetMaxFrequency', output='dbl')
m('GetPulserMaxPRFsSupported', output='bool')
m('GetPulserMaxPRFs', output='dbls')

section('High voltage supply')
m('GetHVSupplyMax', output='dbl')
m('GetHVSupplyMin', output='dbl')
m('GetHVSupply', output='dbl')
m('SetHVSupply', inputs=[('dbl', 'supply')])
m('GetHVSupplySupported', output='bool')
m('GetHVSupplyValues', output='dbls')
m('GetHVSupplyIndexSupported', output='bool')
m('GetHVSupplyIndexMax', output='i32')
m('GetHVSupplyIndex', output='i32')
m('SetHVSupplyIndex', inputs=[('i32', 'index')])
m('GetHVSupplyEnable', output='bool')
m('SetHVSupplyEnable', inputs=[('bool', 'enable')])
m('GetHVSupplyEnableSupported', output='bool')
m('GetHVMeasurement', output='dbl')
m('GetHVMeasurementSupported', output='bool')

section('Trigger')
m('GetTriggerEdgePolarity', output=POL)
m('SetTriggerEdgePolarity', inputs=[(POL, 'polarity')])
m('GetTriggerEdgePolaritySupported', output='bool')
m('GetTriggerSource', output=SRC)
m('SetTriggerSource', inputs=[(SRC, 'source')])
m('GetTriggerSourceInternalSupported', output='bool')
m('GetTriggerSourceExternalSupported', output='bool')
m('GetTriggerSourceSlaveSupported', output='bool')
m('GetPulserTriggerSourceValueNames', output='strs')
m('GetPulserTriggerSourceIndexMax', output='i32')
m('GetPulserTriggerSourceIndex', output='i32')
m('SetPulserTriggerSourceIndex', inputs=[('i32', 'index')])
m('GetTriggerEnable', output='bool')
m('SetTriggerEnable', inputs=[('bool', 'enable')])
m('GetTriggerImpedance', output=TIMP)
m('SetTriggerImpedance', inputs=[(TIMP, 'impedance')])
m('GetTriggerImpedanceSupported', output='bool')
m('GetPulserTriggerCount', output='i32')
m('GetPulserTriggerCountSupported', output='bool')

section('Filters')
m('GetHighPassFilterValues', output='dbls')
m('GetHighPassFilterIndexMax', output='i32')
m('GetHighPassFilterIndex', output='i32')
m('SetHighPassFilterIndex', inputs=[('i32', 'index')])
m('GetLowPassFilterValues', output='dbls')
m('GetLowPassFilterIndexMax', output='i32')
m('GetLowPassFilterIndex', output='i32')
m('SetLowPassFilterIndex', inputs=[('i32', 'index')])

section('Receiver')
m('GetReceiverMode', output=MODE)
m('SetReceiverMode', inputs=[(MODE, 'mode')])
m('GetReceiverModeBothSupported', output='bool')
m('GetReceiverModeThruSupported', output='bool')
m('GetReceiverModeEchoSupported', output='bool')
m('GetReceiverSupplyVoltages', output='strs')
m('GetReceiverSupplyVoltagesSupported', output='bool')

section('Gain')
m('GetGainValues', output='dbls')
m('GetGainIndexMax', output='i32')
m('GetGainIndex', output='i32')
m('SetGainIndex', inputs=[('i32', 'index')])
m('GetGainIndexSupported', output='bool')
m('GetGainStepSizeSupported', output='bool')
m('GetGainStepSize', output='dbl')
m('GetGainMin', output='dbl')
m('GetGainMax', output='dbl')
m('GetGain', output='dbl')
m('SetGain', inputs=[('dbl', 'gain')])

section('Pulse energy and damping')
m('GetPulseEnergyValueNames', output='strs')
m('GetPulseEnergyIndexMax', output='i32')
m('GetPulseEnergyIndex', output='i32')
m('SetPulseEnergyIndex', inputs=[('i32', 'index')])
m('GetPulseEnergyIndexSupported', output='bool')
m('GetEnergyPerPulse', output='dbl')
m('GetPulserEnergyCapacitorValuesSupported', output='bool')
m('GetPulserEnergyCapacitorValues', output='dbls')
m('GetDampingValues', output='dbls')
m('GetDampingIndexMax', output='i32')
m('GetDampingIndex', output='i32')
m('SetDampingIndex', inputs=[('i32', 'index')])
m('GetDampingIndexSupported', output='bool')

section('Pulser state')
m('GetPulserImpedance', output=PIMP)
m('SetPulserImpedance', inputs=[(PIMP, 'impedance')])
m('GetPulserImpedanceSupported', output='bool')
m('GetPulserIsPulsing', output=PULS)
m('GetPulserPowerLimitStatus', output=PWR)
m('GetIsPulserPresent', output='bool')
m('GetIsPulserPresentSupported', output='bool')
m('GetHasManualControls', output='bool')
m('GetLEDBlinkModeValues', output='strs')
m('GetLEDBlinkModeIndexMax', output='i32')
m('GetLEDBlinkModeIndex', output='i32')
m('SetLEDBlinkModeIndex', inputs=[('i32', 'index')])
m('GetLEDBlinkModeIndexSupported', output='bool')

section('Unit identification')
m('GetUnitModelName', output='str')
m('SetUnitModelName', inputs=[(S, 'name')])
m('GetUnitModelNameSupported', output='bool')
m('GetUnitSerialNum', output='str')
m('SetUnitSerialNum', inputs=[(S, 'serialNum')])
m('GetUnitSerialNumSupported', output='bool')
m('GetPulserModelName', output='str')
m('SetPulserModelName', inputs=[(S, 'name')])
m('GetPulserModelNameSupported', output='bool')
m('GetPulserSerialNum', output='str')
m('SetPulserSerialNum', inputs=[(S, 'serialNum')])
m('GetPulserSerialNumSupported', output='bool')
m('GetPulserHWRev', output='str')
m('SetPulserHWRev', inputs=[(S, 'hwRev')])
m('GetPulserHWRevSupported', output='bool')
m('GetPulserFirmwareVer', output='str')
m('GetPulserFirmwareVerSupported', output='bool')
m('GetPulserOEMDataSupported', output='bool')
m('GetReceiverModelName', output='str')
m('SetReceiverModelName', inputs=[(S, 'name')])
m('GetReceiverModelNameSupported', output='bool')
m('GetReceiverSerialNum', output='str')
m('SetReceiverSerialNum', inputs=[(S, 'serialNum')])
m('GetReceiverSerialNumSupported', output='bool')
m('GetReceiverHWRev', output='str')
m('SetReceiverHWRev', inputs=[(S, 'hwRev')])
m('GetReceiverHWRevSupported', output='bool')
m('GetReceiverFirmwareVer', output='str')
m('GetReceiverFirmwareVerSupported', output='bool')
m('GetReceiverOEMDataSupported', output='bool')

# Getters whose value changes by itself (measurements, counters): the parity
# test can't expect two simulated devices to agree on them, so it skips them.
VOLATILE = set()
