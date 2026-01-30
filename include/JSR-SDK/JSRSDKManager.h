#pragma once

#ifndef JSRSDKMANAGER_C_H
#define JSRSDKMANAGER_C_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Opaque manager handle for C consumers
typedef void *JSRSDKManagerHandle;

// Factory and destructor
__declspec(dllexport) JSRSDKManagerHandle JSR_CreateManager();
__declspec(dllexport) void JSR_DestroyManager(JSRSDKManagerHandle mgr);

// Return codes: >=0 success (or size/required), 0 success for actions, -1 general error, -2 index out of range

// Plugin names
__declspec(dllexport) int JSR_GetPluginNamesCount(JSRSDKManagerHandle mgr);
__declspec(dllexport) int JSR_GetPluginName(JSRSDKManagerHandle mgr, int index, char *outBuf, int bufSize);

// Managed plugin names
__declspec(dllexport) int JSR_GetManagedPluginNamesCount(JSRSDKManagerHandle mgr);
__declspec(dllexport) int JSR_GetManagedPluginName(JSRSDKManagerHandle mgr, int index, char *outBuf, int bufSize);

// Custom settings
__declspec(dllexport) int JSR_GetCustomSettingsCount(JSRSDKManagerHandle mgr);
__declspec(dllexport) int JSR_GetCustomSetting(JSRSDKManagerHandle mgr, int index, char *outBuf, int bufSize);

// Open option names for a plugin
__declspec(dllexport) int JSR_GetOpenOptionNamesCount(JSRSDKManagerHandle mgr, const char *plugin);
__declspec(dllexport) int JSR_GetOpenOptionName(JSRSDKManagerHandle mgr, const char *plugin, int index, char *outBuf, int bufSize);

// Ports include/exclude
__declspec(dllexport) int JSR_GetPortsToExcludeCount(JSRSDKManagerHandle mgr, const char *plugin);
__declspec(dllexport) int JSR_GetPortToExclude(JSRSDKManagerHandle mgr, const char *plugin, int index, char *outBuf, int bufSize);
__declspec(dllexport) int JSR_GetPortsToIncludeCount(JSRSDKManagerHandle mgr, const char *plugin);
__declspec(dllexport) int JSR_GetPortToInclude(JSRSDKManagerHandle *mgr, const char *plugin, int index, char *outBuf, int bufSize);

// Instruments for a plugin
__declspec(dllexport) int JSR_GetInstrumentsCount(JSRSDKManagerHandle mgr, const char *plugin);
__declspec(dllexport) int JSR_GetInstrumentModelName(JSRSDKManagerHandle mgr, const char *plugin, int index, char *outBuf, int bufSize);
__declspec(dllexport) int JSR_GetInstrumentSerialNum(JSRSDKManagerHandle mgr, const char *plugin, int index, char *outBuf, int bufSize);
__declspec(dllexport) int JSR_GetInstrumentPort(JSRSDKManagerHandle mgr, const char *plugin, int index, char *outBuf, int bufSize);
__declspec(dllexport) int JSR_GetInstrumentPluginName(JSRSDKManagerHandle mgr, const char *plugin, int index, char *outBuf, int bufSize);

// PulserReceiver info (vector<string>)
__declspec(dllexport) int JSR_GetPulserReceiverInfoCount(JSRSDKManagerHandle mgr, const char *model, const char *serial, int idxPR);
__declspec(dllexport) int JSR_GetPulserReceiverInfoItem(JSRSDKManagerHandle mgr, const char *model, const char *serial, int idxPR, int index, char *outBuf, int bufSize);

// PulserReceivers list
__declspec(dllexport) int JSR_GetPulserReceiversCount(JSRSDKManagerHandle mgr);
__declspec(dllexport) int JSR_GetPulserReceiverInstrumentModelName(JSRSDKManagerHandle mgr, int index, char *outBuf, int bufSize);
__declspec(dllexport) int JSR_GetPulserReceiverInstrumentSerialNum(JSRSDKManagerHandle mgr, int index, char *outBuf, int bufSize);
__declspec(dllexport) int JSR_GetPulserReceiverIndex(JSRSDKManagerHandle mgr, int index);

// PulserMaxPRFs (double vector)
__declspec(dllexport) int JSR_GetPulserMaxPRFsCount(JSRSDKManagerHandle mgr);
__declspec(dllexport) double JSR_GetPulserMaxPRF(JSRSDKManagerHandle mgr, int index);

// Pulse repetition frequency values
__declspec(dllexport) int JSR_GetPulseRepetitionFrequencyValuesCount(JSRSDKManagerHandle mgr);
__declspec(dllexport) double JSR_GetPulseRepetitionFrequencyValue(JSRSDKManagerHandle mgr, int index);

// OEM data (byte arrays)
__declspec(dllexport) int JSR_GetReceiverOEMDataSize(JSRSDKManagerHandle mgr);
__declspec(dllexport) int JSR_CopyReceiverOEMData(JSRSDKManagerHandle mgr, unsigned char *dest, int destSize);
__declspec(dllexport) int JSR_GetPulserOEMDataSize(JSRSDKManagerHandle mgr);
__declspec(dllexport) int JSR_CopyPulserOEMData(JSRSDKManagerHandle mgr, unsigned char *dest, int destSize);

// --- Additional actions and properties ---
// Add / configure plugins
__declspec(dllexport) int JSR_AddManagedPlugin(JSRSDKManagerHandle mgr, const char *pluginName);
__declspec(dllexport) int JSR_AddPluginOpenOption(JSRSDKManagerHandle mgr, const char *pluginName, const char *optionName, const char *optionValue);
__declspec(dllexport) int JSR_AddPluginType(JSRSDKManagerHandle mgr, const char *pluginType);
__declspec(dllexport) int JSR_ForceDetach(JSRSDKManagerHandle mgr);

// Load plugins from path
__declspec(dllexport) int JSR_LoadPlugins(JSRSDKManagerHandle mgr, const char *pluginPath);

// Discovery and selection
__declspec(dllexport) int JSR_SetDiscoveryEnable(JSRSDKManagerHandle mgr, int enable);
__declspec(dllexport) int JSR_SetCurrentPulserReceiverByModel(JSRSDKManagerHandle mgr, const char *model, const char *serial, int idxPR);

// Property setters
__declspec(dllexport) int JSR_SetPulserPropertyValue(JSRSDKManagerHandle mgr, const char *propName, const char *value);
__declspec(dllexport) int JSR_SetPulserPropertyValueWithRole(JSRSDKManagerHandle mgr, const char *propName, int role, const char *value);

// Shutdown
__declspec(dllexport) int JSR_Shutdown(JSRSDKManagerHandle mgr);

// Generic string getters/setters
__declspec(dllexport) int JSR_GetPluginPath(JSRSDKManagerHandle mgr, char *outBuf, int bufSize);
__declspec(dllexport) int JSR_SetPluginPath(JSRSDKManagerHandle mgr, const char *path);

__declspec(dllexport) int JSR_GetUnitModelName(JSRSDKManagerHandle mgr, char *outBuf, int bufSize);
__declspec(dllexport) int JSR_GetUnitSerialNum(JSRSDKManagerHandle mgr, char *outBuf, int bufSize);

__declspec(dllexport) int JSR_GetPulserModelName(JSRSDKManagerHandle mgr, char *outBuf, int bufSize);
__declspec(dllexport) int JSR_GetPulserSerialNum(JSRSDKManagerHandle mgr, char *outBuf, int bufSize);

__declspec(dllexport) int JSR_GetReceiverModelName(JSRSDKManagerHandle mgr, char *outBuf, int bufSize);
__declspec(dllexport) int JSR_GetReceiverSerialNum(JSRSDKManagerHandle mgr, char *outBuf, int bufSize);

// Manager state
__declspec(dllexport) int JSR_GetManagerState(JSRSDKManagerHandle mgr);

#endif // JSRSDKMANAGER_C_H

#ifdef __cplusplus
}
#endif
