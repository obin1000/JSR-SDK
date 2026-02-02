/**
 * @file EventConverters.h
 * @brief Conversion functions for C++ std::string to C JSRString
 * 
 * These functions safely convert between std::string-based C++ data
 * and JSRString-based C structures for safe DLL boundary crossing.
 */

#pragma once

#include "JSR-SDK/events/StatusChangedEvent.h"
#include "JSR-SDK/events/NotifyEvent.h"
#include "JSR-SDK/structs/InstrumentID.h"
#include "JSR-SDK/structs/PulserReceiverID.h"
#include "JSR-SDK/structs/ExceptionJSRSDK.h"

#include <string>
#include <vector>

namespace EventConverters {

/**
 * @brief Convert std::string to JSRString
 */
inline void convertString(JSRString& dest, const std::string& src) {
    JSRString_Set(&dest, src.c_str());
}

/**
 * @brief Convert std::string to JSRStringLarge
 */
inline void convertStringLarge(JSRStringLarge& dest, const std::string& src) {
    JSRStringLarge_Set(&dest, src.c_str());
}

/**
 * @brief Copy InstrumentID fields (already using JSRString)
 */
inline void convertInstrumentID(InstrumentID& dest, const InstrumentID& src) {
    dest = src;  // Simple copy since both use JSRString
}

/**
 * @brief Copy PulserReceiverID fields (already using JSRString via InstrumentID)
 */
inline void convertPulserReceiverID(PulserReceiverID& dest, const PulserReceiverID& src) {
    dest = src;  // Simple copy since both use JSRString
}

/**
 * @brief Copy ExceptionJSRSDK fields (already using JSRString)
 */
inline void convertException(ExceptionJSRSDK& dest, const ExceptionJSRSDK& src) {
    dest = src;  // Simple copy since both use JSRString
}

/**
 * @brief Copy StatusChangedEvent fields (already using JSRString)
 */
inline void convertStatusChangedEvent(StatusChangedEvent& dest, const StatusChangedEvent& src) {
    dest = src;  // Simple copy since both use JSRString
}

/**
 * @brief Copy NotifyEvent fields (already using JSRString)
 */
inline void convertNotifyEvent(NotifyEvent& dest, const NotifyEvent& src) {
    dest = src;  // Simple copy since both use JSRString
}

// ============================================================================
// Helper functions for converting from C++ std::string-based data
// ============================================================================

/**
 * @brief Set InstrumentID from std::string values
 */
inline void setInstrumentID(InstrumentID& dest,
                           const std::string& modelName,
                           const std::string& serialNum,
                           const std::string& port,
                           const std::string& pluginName) {
    convertString(dest.ModelName, modelName);
    convertString(dest.SerialNum, serialNum);
    convertString(dest.Port, port);
    convertString(dest.PluginName, pluginName);
}

/**
 * @brief Set PulserReceiverID from std::string values and index
 */
inline void setPulserReceiverID(PulserReceiverID& dest,
                               const std::string& modelName,
                               const std::string& serialNum,
                               const std::string& port,
                               const std::string& pluginName,
                               int prIndex) {
    setInstrumentID(dest.InstrumentId, modelName, serialNum, port, pluginName);
    dest.PulserReceiverIndex = prIndex;
}

/**
 * @brief Set ExceptionJSRSDK from std::string values
 */
inline void setException(ExceptionJSRSDK& dest,
                        C_ERROR_CODE errorCode,
                        const std::string& message,
                        const std::string& innerMessage = "",
                        const std::string& stackTrace = "",
                        int hresult = 0) {
    dest.ErrorCode = errorCode;
    convertString(dest.Message, message);
    convertString(dest.InnerMessage, innerMessage.empty() ? "None" : innerMessage);
    convertStringLarge(dest.StackTrace, stackTrace);
    dest.HResult = hresult;
}

} // namespace EventConverters
