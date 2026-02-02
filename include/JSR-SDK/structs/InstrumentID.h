/**
 * @file InstrumentID.h
 * @brief C-compatible instrument identification structure
 * 
 * Represents an instrument with identifying details such as model name,
 * serial number, port, and plugin name.
 * Mirrors JSRDotNETSDK::IInstrumentIdentity
 */

#pragma once

#include "JSR-SDK/JSRString.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Instrument identification structure
 * 
 * Contains all identifying information for a hardware instrument.
 * Safe to use across DLL boundaries.
 */
typedef struct {
    /** @brief The model name of the instrument */
    JSRString ModelName;
    
    /** @brief The serial number of the instrument */
    JSRString SerialNum;
    
    /** @brief The port associated with the instrument (e.g., "COM3", "USB0") */
    JSRString Port;
    
    /** @brief The name of the plugin associated with the instrument */
    JSRString PluginName;
} InstrumentID;

/**
 * @brief Initialize an InstrumentID to default (empty) values
 * @param id Pointer to InstrumentID structure
 */
static inline void InstrumentID_Init(InstrumentID* id) {
    if (!id) return;
    JSRString_Init(&id->ModelName);
    JSRString_Init(&id->SerialNum);
    JSRString_Init(&id->Port);
    JSRString_Init(&id->PluginName);
}

#ifdef __cplusplus
}

// C++ convenience functions
#include <string>
#include <sstream>

namespace JSR {

/**
 * @brief Get description string from InstrumentID (C++ only)
 * @param id The instrument ID
 * @return Formatted description string
 */
inline std::string GetDescription(const InstrumentID& id) {
    std::ostringstream oss;
    oss << "Model: " << id.ModelName.data << "\n"
        << "Serial Number: " << id.SerialNum.data << "\n"
        << "Port: " << id.Port.data << "\n"
        << "Plugin Name: " << id.PluginName.data << "\n";
    return oss.str();
}

/**
 * @brief Convert InstrumentID to string (C++ only)
 * @param id The instrument ID
 * @return String representation
 */
inline std::string ToString(const InstrumentID& id) {
    return GetDescription(id);
}

} // namespace JSR

#endif // __cplusplus
