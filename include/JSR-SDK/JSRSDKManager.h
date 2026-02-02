/**
 * @file JSRSDKManager.h
 * @brief C API for JSR SDK Manager
 * 
 * This header provides a C-compatible interface to the JSR SDK, allowing
 * interaction with ultrasonic pulser/receiver devices through a simple
 * handle-based API.
 * 
 * @section usage Usage Pattern
 * 1. Create manager: JSR_CreateManager()
 * 2. Load plugins: JSR_LoadPlugins()
 * 3. Set callbacks (optional): JSR_SetStatusChangeCallback(), JSR_SetNotifyCallback()
 * 4. Configure devices: JSR_SetCurrentPulserReceiverByModel()
 * 5. Use device functions
 * 6. Cleanup: JSR_DestroyManager()
 * 
 * @section return_codes Return Code Convention
 * - >= 0: Success (count, size, or 0 for actions)
 * - -1: General error (null handle, exception, etc.)
 * - -2: Index out of range
 * 
 * String buffer functions return the required buffer size when outBuf is NULL.
 */

#pragma once

#ifndef JSRSDKMANAGER_C_H
#define JSRSDKMANAGER_C_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
class StatusChangedEvent;
class NotifyEvent;
#else
typedef struct StatusChangedEvent StatusChangedEvent;
typedef struct NotifyEvent NotifyEvent;
#endif

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// Core Types
// ============================================================================

/** @brief Opaque handle to the SDK manager instance */
typedef void *JSRSDKManagerHandle;

/** @brief Callback invoked when device status changes */
typedef void (*JSR_StatusChangeCallback)(const StatusChangedEvent *evt, void *user_data);

/** @brief Callback invoked for notifications from the manager */
typedef void (*JSR_NotifyCallback)(const NotifyEvent *evt, void *user_data);

// ============================================================================
// Manager Lifecycle
// ============================================================================

/**
 * @brief Creates a new SDK manager instance
 * @return Handle to the created manager, or NULL on failure
 */
__declspec(dllexport) JSRSDKManagerHandle JSR_CreateManager();

/**
 * @brief Destroys the SDK manager and releases all resources
 * @param mgr The manager handle to destroy
 * @note Automatically removes all registered callbacks
 */
__declspec(dllexport) void JSR_DestroyManager(JSRSDKManagerHandle mgr);

// ============================================================================
// Callback Management
// ============================================================================

/**
 * @brief Registers a callback for status change events
 * @param mgr Manager handle
 * @param cb Callback function pointer (or NULL to clear)
 * @param user_data User context passed to callback
 * @return 0 on success, -1 on error
 */
__declspec(dllexport) int JSR_SetStatusChangeCallback(JSRSDKManagerHandle mgr, JSR_StatusChangeCallback cb, void *user_data);

/**
 * @brief Removes the status change callback
 * @param mgr Manager handle
 * @return 0 on success, -1 on error
 */
__declspec(dllexport) int JSR_RemoveStatusChangeCallback(JSRSDKManagerHandle mgr);

/**
 * @brief Registers a callback for notify events
 * @param mgr Manager handle
 * @param cb Callback function pointer (or NULL to clear)
 * @param user_data User context passed to callback
 * @return 0 on success, -1 on error
 */
__declspec(dllexport) int JSR_SetNotifyCallback(JSRSDKManagerHandle mgr, JSR_NotifyCallback cb, void *user_data);

/**
 * @brief Removes the notify callback
 * @param mgr Manager handle
 * @return 0 on success, -1 on error
 */
__declspec(dllexport) int JSR_RemoveNotifyCallback(JSRSDKManagerHandle mgr);

// ============================================================================
// Plugin Management
// ============================================================================

/**
 * @brief Gets the number of available plugins
 * @param mgr Manager handle
 * @return Number of plugins, or -1 on error
 */
__declspec(dllexport) int JSR_GetPluginNamesCount(JSRSDKManagerHandle mgr);

/**
 * @brief Gets the name of a plugin by index
 * @param mgr Manager handle
 * @param index Plugin index (0 to count-1)
 * @param outBuf Output buffer (or NULL to query size)
 * @param bufSize Size of output buffer
 * @return Required buffer size (including null terminator), or -1/-2 on error
 */
__declspec(dllexport) int JSR_GetPluginName(JSRSDKManagerHandle mgr, int index, char *outBuf, int bufSize);

/**
 * @brief Gets the number of managed plugins
 * @param mgr Manager handle
 * @return Number of managed plugins, or -1 on error
 */
__declspec(dllexport) int JSR_GetManagedPluginNamesCount(JSRSDKManagerHandle mgr);

/**
 * @brief Gets the name of a managed plugin by index
 * @param mgr Manager handle
 * @param index Plugin index
 * @param outBuf Output buffer
 * @param bufSize Size of output buffer
 * @return Required buffer size, or -1/-2 on error
 */
__declspec(dllexport) int JSR_GetManagedPluginName(JSRSDKManagerHandle mgr, int index, char *outBuf, int bufSize);

/**
 * @brief Adds a plugin to the managed plugins list
 * @param mgr Manager handle
 * @param pluginName Name of the plugin to add
 * @return 0 on success, -1 on error
 */
__declspec(dllexport) int JSR_AddManagedPlugin(JSRSDKManagerHandle mgr, const char *pluginName);

/**
 * @brief Adds a plugin type to be managed
 * @param mgr Manager handle
 * @param pluginType Type identifier of the plugin
 * @return 0 on success, -1 on error
 */
__declspec(dllexport) int JSR_AddPluginType(JSRSDKManagerHandle mgr, const char *pluginType);

/**
 * @brief Loads plugins from the specified directory
 * @param mgr Manager handle
 * @param pluginPath Path to the directory containing plugins
 * @return 0 on success, -1 on error
 */
__declspec(dllexport) int JSR_LoadPlugins(JSRSDKManagerHandle mgr, const char *pluginPath);

/**
 * @brief Gets the plugin path
 * @param mgr Manager handle
 * @param outBuf Output buffer
 * @param bufSize Size of output buffer
 * @return Required buffer size, or -1 on error
 */
__declspec(dllexport) int JSR_GetPluginPath(JSRSDKManagerHandle mgr, char *outBuf, int bufSize);

/**
 * @brief Sets the plugin path
 * @param mgr Manager handle
 * @param path New plugin path
 * @return 0 on success, -1 on error
 */
__declspec(dllexport) int JSR_SetPluginPath(JSRSDKManagerHandle mgr, const char *path);

// ============================================================================
// Plugin Configuration
// ============================================================================

/**
 * @brief Gets the number of custom settings
 * @param mgr Manager handle
 * @return Number of settings, or -1 on error
 */
__declspec(dllexport) int JSR_GetCustomSettingsCount(JSRSDKManagerHandle mgr);

/**
 * @brief Gets a custom setting by index
 * @param mgr Manager handle
 * @param index Setting index
 * @param outBuf Output buffer
 * @param bufSize Size of output buffer
 * @return Required buffer size, or -1/-2 on error
 */
__declspec(dllexport) int JSR_GetCustomSetting(JSRSDKManagerHandle mgr, int index, char *outBuf, int bufSize);

/**
 * @brief Gets the number of open option names for a plugin
 * @param mgr Manager handle
 * @param plugin Plugin name
 * @return Number of options, or -1 on error
 */
__declspec(dllexport) int JSR_GetOpenOptionNamesCount(JSRSDKManagerHandle mgr, const char *plugin);

/**
 * @brief Gets an open option name for a plugin
 * @param mgr Manager handle
 * @param plugin Plugin name
 * @param index Option index
 * @param outBuf Output buffer
 * @param bufSize Size of output buffer
 * @return Required buffer size, or -1/-2 on error
 */
__declspec(dllexport) int JSR_GetOpenOptionName(JSRSDKManagerHandle mgr, const char *plugin, int index, char *outBuf, int bufSize);

/**
 * @brief Adds an open option for a plugin
 * @param mgr Manager handle
 * @param pluginName Plugin name
 * @param optionName Option name
 * @param optionValue Option value
 * @return 0 on success, -1 on error
 */
__declspec(dllexport) int JSR_AddPluginOpenOption(JSRSDKManagerHandle mgr, const char *pluginName, const char *optionName, const char *optionValue);

// ============================================================================
// Port Filtering
// ============================================================================

/**
 * @brief Gets the number of ports to exclude for a plugin
 * @param mgr Manager handle
 * @param plugin Plugin name
 * @return Number of excluded ports, or -1 on error
 */
__declspec(dllexport) int JSR_GetPortsToExcludeCount(JSRSDKManagerHandle mgr, const char *plugin);

/**
 * @brief Gets an excluded port by index
 * @param mgr Manager handle
 * @param plugin Plugin name
 * @param index Port index
 * @param outBuf Output buffer
 * @param bufSize Size of output buffer
 * @return Required buffer size, or -1/-2 on error
 */
__declspec(dllexport) int JSR_GetPortToExclude(JSRSDKManagerHandle mgr, const char *plugin, int index, char *outBuf, int bufSize);

/**
 * @brief Gets the number of ports to include for a plugin
 * @param mgr Manager handle
 * @param plugin Plugin name
 * @return Number of included ports, or -1 on error
 */
__declspec(dllexport) int JSR_GetPortsToIncludeCount(JSRSDKManagerHandle mgr, const char *plugin);

/**
 * @brief Gets an included port by index
 * @param mgr Manager handle
 * @param plugin Plugin name
 * @param index Port index
 * @param outBuf Output buffer
 * @param bufSize Size of output buffer
 * @return Required buffer size, or -1/-2 on error
 */
__declspec(dllexport) int JSR_GetPortToInclude(JSRSDKManagerHandle mgr, const char *plugin, int index, char *outBuf, int bufSize);

// ============================================================================
// Instrument Discovery
// ============================================================================

/**
 * @brief Gets the number of discovered instruments for a plugin
 * @param mgr Manager handle
 * @param plugin Plugin name
 * @return Number of instruments, or -1 on error
 */
__declspec(dllexport) int JSR_GetInstrumentsCount(JSRSDKManagerHandle mgr, const char *plugin);

/**
 * @brief Gets an instrument's model name
 * @param mgr Manager handle
 * @param plugin Plugin name
 * @param index Instrument index
 * @param outBuf Output buffer
 * @param bufSize Size of output buffer
 * @return Required buffer size, or -1/-2 on error
 */
__declspec(dllexport) int JSR_GetInstrumentModelName(JSRSDKManagerHandle mgr, const char *plugin, int index, char *outBuf, int bufSize);

/**
 * @brief Gets an instrument's serial number
 * @param mgr Manager handle
 * @param plugin Plugin name
 * @param index Instrument index
 * @param outBuf Output buffer
 * @param bufSize Size of output buffer
 * @return Required buffer size, or -1/-2 on error
 */
__declspec(dllexport) int JSR_GetInstrumentSerialNum(JSRSDKManagerHandle mgr, const char *plugin, int index, char *outBuf, int bufSize);

/**
 * @brief Gets an instrument's port identifier
 * @param mgr Manager handle
 * @param plugin Plugin name
 * @param index Instrument index
 * @param outBuf Output buffer
 * @param bufSize Size of output buffer
 * @return Required buffer size, or -1/-2 on error
 */
__declspec(dllexport) int JSR_GetInstrumentPort(JSRSDKManagerHandle mgr, const char *plugin, int index, char *outBuf, int bufSize);

/**
 * @brief Gets an instrument's plugin name
 * @param mgr Manager handle
 * @param plugin Plugin name
 * @param index Instrument index
 * @param outBuf Output buffer
 * @param bufSize Size of output buffer
 * @return Required buffer size, or -1/-2 on error
 */
__declspec(dllexport) int JSR_GetInstrumentPluginName(JSRSDKManagerHandle mgr, const char *plugin, int index, char *outBuf, int bufSize);

/**
 * @brief Enables or disables instrument discovery
 * @param mgr Manager handle
 * @param enable 1 to enable, 0 to disable
 * @return 0 on success, -1 on error
 */
__declspec(dllexport) int JSR_SetDiscoveryEnable(JSRSDKManagerHandle mgr, int enable);

// ============================================================================
// Pulser/Receiver Management
// ============================================================================

/**
 * @brief Gets the number of available pulser/receivers
 * @param mgr Manager handle
 * @return Number of pulser/receivers, or -1 on error
 */
__declspec(dllexport) int JSR_GetPulserReceiversCount(JSRSDKManagerHandle mgr);

/**
 * @brief Gets a pulser/receiver's instrument model name
 * @param mgr Manager handle
 * @param index Pulser/receiver index
 * @param outBuf Output buffer
 * @param bufSize Size of output buffer
 * @return Required buffer size, or -1/-2 on error
 */
__declspec(dllexport) int JSR_GetPulserReceiverInstrumentModelName(JSRSDKManagerHandle mgr, int index, char *outBuf, int bufSize);

/**
 * @brief Gets a pulser/receiver's instrument serial number
 * @param mgr Manager handle
 * @param index Pulser/receiver index
 * @param outBuf Output buffer
 * @param bufSize Size of output buffer
 * @return Required buffer size, or -1/-2 on error
 */
__declspec(dllexport) int JSR_GetPulserReceiverInstrumentSerialNum(JSRSDKManagerHandle mgr, int index, char *outBuf, int bufSize);

/**
 * @brief Gets a pulser/receiver's index
 * @param mgr Manager handle
 * @param index Pulser/receiver index
 * @return The pulser/receiver index, or -1/-2 on error
 */
__declspec(dllexport) int JSR_GetPulserReceiverIndex(JSRSDKManagerHandle mgr, int index);

/**
 * @brief Gets the number of info items for a pulser/receiver
 * @param mgr Manager handle
 * @param model Instrument model name
 * @param serial Instrument serial number
 * @param idxPR Pulser/receiver index
 * @return Number of info items, or -1 on error
 */
__declspec(dllexport) int JSR_GetPulserReceiverInfoCount(JSRSDKManagerHandle mgr, const char *model, const char *serial, int idxPR);

/**
 * @brief Gets a pulser/receiver info item
 * @param mgr Manager handle
 * @param model Instrument model name
 * @param serial Instrument serial number
 * @param idxPR Pulser/receiver index
 * @param index Info item index
 * @param outBuf Output buffer
 * @param bufSize Size of output buffer
 * @return Required buffer size, or -1/-2 on error
 */
__declspec(dllexport) int JSR_GetPulserReceiverInfoItem(JSRSDKManagerHandle mgr, const char *model, const char *serial, int idxPR, int index, char *outBuf, int bufSize);

/**
 * @brief Sets the current pulser/receiver by model, serial, and index
 * @param mgr Manager handle
 * @param model Instrument model name
 * @param serial Instrument serial number
 * @param idxPR Pulser/receiver index
 * @return 0 on success, -1 on error
 */
__declspec(dllexport) int JSR_SetCurrentPulserReceiverByModel(JSRSDKManagerHandle mgr, const char *model, const char *serial, int idxPR);

// ============================================================================
// Pulser Properties
// ============================================================================

/**
 * @brief Sets a pulser property value
 * @param mgr Manager handle
 * @param propName Property name
 * @param value Property value as string
 * @return 0 on success, -1 on error
 */
__declspec(dllexport) int JSR_SetPulserPropertyValue(JSRSDKManagerHandle mgr, const char *propName, const char *value);

/**
 * @brief Sets a pulser property value with a specific role
 * @param mgr Manager handle
 * @param propName Property name
 * @param role Property role (from C_PULSER_PROPERTY_ROLE enum)
 * @param value Property value as string
 * @return 0 on success, -1 on error
 */
__declspec(dllexport) int JSR_SetPulserPropertyValueWithRole(JSRSDKManagerHandle mgr, const char *propName, int role, const char *value);

/**
 * @brief Gets the number of maximum PRF values
 * @param mgr Manager handle
 * @return Number of PRF values, or -1 on error
 */
__declspec(dllexport) int JSR_GetPulserMaxPRFsCount(JSRSDKManagerHandle mgr);

/**
 * @brief Gets a maximum PRF value by index
 * @param mgr Manager handle
 * @param index PRF index
 * @return PRF value, or -1.0 on error
 */
__declspec(dllexport) double JSR_GetPulserMaxPRF(JSRSDKManagerHandle mgr, int index);

/**
 * @brief Gets the number of pulse repetition frequency values
 * @param mgr Manager handle
 * @return Number of frequency values, or -1 on error
 */
__declspec(dllexport) int JSR_GetPulseRepetitionFrequencyValuesCount(JSRSDKManagerHandle mgr);

/**
 * @brief Gets a pulse repetition frequency value by index
 * @param mgr Manager handle
 * @param index Frequency index
 * @return Frequency value in Hz, or -1.0 on error
 */
__declspec(dllexport) double JSR_GetPulseRepetitionFrequencyValue(JSRSDKManagerHandle mgr, int index);

// ============================================================================
// OEM Data (Binary)
// ============================================================================

/**
 * @brief Gets the size of receiver OEM data
 * @param mgr Manager handle
 * @return Data size in bytes, or -1 on error
 */
__declspec(dllexport) int JSR_GetReceiverOEMDataSize(JSRSDKManagerHandle mgr);

/**
 * @brief Copies receiver OEM data to buffer
 * @param mgr Manager handle
 * @param dest Destination buffer (or NULL to query size)
 * @param destSize Size of destination buffer
 * @return Bytes copied, or total size if dest is NULL, or -1 on error
 */
__declspec(dllexport) int JSR_CopyReceiverOEMData(JSRSDKManagerHandle mgr, unsigned char *dest, int destSize);

/**
 * @brief Gets the size of pulser OEM data
 * @param mgr Manager handle
 * @return Data size in bytes, or -1 on error
 */
__declspec(dllexport) int JSR_GetPulserOEMDataSize(JSRSDKManagerHandle mgr);

/**
 * @brief Copies pulser OEM data to buffer
 * @param mgr Manager handle
 * @param dest Destination buffer (or NULL to query size)
 * @param destSize Size of destination buffer
 * @return Bytes copied, or total size if dest is NULL, or -1 on error
 */
__declspec(dllexport) int JSR_CopyPulserOEMData(JSRSDKManagerHandle mgr, unsigned char *dest, int destSize);

// ============================================================================
// Device Information
// ============================================================================

/**
 * @brief Gets the unit model name
 * @param mgr Manager handle
 * @param outBuf Output buffer
 * @param bufSize Size of output buffer
 * @return Required buffer size, or -1 on error
 */
__declspec(dllexport) int JSR_GetUnitModelName(JSRSDKManagerHandle mgr, char *outBuf, int bufSize);

/**
 * @brief Gets the unit serial number
 * @param mgr Manager handle
 * @param outBuf Output buffer
 * @param bufSize Size of output buffer
 * @return Required buffer size, or -1 on error
 */
__declspec(dllexport) int JSR_GetUnitSerialNum(JSRSDKManagerHandle mgr, char *outBuf, int bufSize);

/**
 * @brief Gets the pulser model name
 * @param mgr Manager handle
 * @param outBuf Output buffer
 * @param bufSize Size of output buffer
 * @return Required buffer size, or -1 on error
 */
__declspec(dllexport) int JSR_GetPulserModelName(JSRSDKManagerHandle mgr, char *outBuf, int bufSize);

/**
 * @brief Gets the pulser serial number
 * @param mgr Manager handle
 * @param outBuf Output buffer
 * @param bufSize Size of output buffer
 * @return Required buffer size, or -1 on error
 */
__declspec(dllexport) int JSR_GetPulserSerialNum(JSRSDKManagerHandle mgr, char *outBuf, int bufSize);

/**
 * @brief Gets the receiver model name
 * @param mgr Manager handle
 * @param outBuf Output buffer
 * @param bufSize Size of output buffer
 * @return Required buffer size, or -1 on error
 */
__declspec(dllexport) int JSR_GetReceiverModelName(JSRSDKManagerHandle mgr, char *outBuf, int bufSize);

/**
 * @brief Gets the receiver serial number
 * @param mgr Manager handle
 * @param outBuf Output buffer
 * @param bufSize Size of output buffer
 * @return Required buffer size, or -1 on error
 */
__declspec(dllexport) int JSR_GetReceiverSerialNum(JSRSDKManagerHandle mgr, char *outBuf, int bufSize);

// ============================================================================
// System Control
// ============================================================================

/**
 * @brief Forces detachment of all devices
 * @param mgr Manager handle
 * @return 0 on success, -1 on error
 */
__declspec(dllexport) int JSR_ForceDetach(JSRSDKManagerHandle mgr);

/**
 * @brief Shuts down the SDK manager
 * @param mgr Manager handle
 * @return 0 on success, -1 on error
 * @note After shutdown, the manager should be destroyed
 */
__declspec(dllexport) int JSR_Shutdown(JSRSDKManagerHandle mgr);

/**
 * @brief Gets the current manager state
 * @param mgr Manager handle
 * @return State value (C_MANAGER_STATE enum), or -1 on error
 */
__declspec(dllexport) int JSR_GetManagerState(JSRSDKManagerHandle mgr);

#ifdef __cplusplus
}
#endif

#endif // JSRSDKMANAGER_C_H
