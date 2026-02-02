/**
 * @file ExceptionJSRSDK.h
 * @brief C-compatible exception structure for JSR SDK
 * 
 * This structure can be used from both C and C++ code and is safe
 * to pass across DLL boundaries.
 */

#pragma once

#include "JSR-SDK/enums/C_ERROR_CODE.h"
#include "JSR-SDK/JSRString.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Exception information structure
 * 
 * Contains error code and descriptive messages about an exception.
 * Safe to use across DLL boundaries.
 */
typedef struct ExceptionJSRSDK {
    /** @brief Error code identifying the type of error */
    C_ERROR_CODE ErrorCode;
    
    /** @brief Primary error message */
    JSRString Message;
    
    /** @brief Inner/nested error message */
    JSRString InnerMessage;
    
    /** @brief Stack trace (if available) */
    JSRStringLarge StackTrace;
    
    /** @brief HResult code (Windows-specific) */
    int HResult;
} ExceptionJSRSDK;

/**
 * @brief Initialize an exception structure to default values
 * @param ex Pointer to exception structure
 */
static inline void ExceptionJSRSDK_Init(ExceptionJSRSDK* ex) {
    if (!ex) return;
    ex->ErrorCode = C_ERROR_CODE::UNKNOWN;
    JSRString_Set(&ex->Message, "");
    JSRString_Set(&ex->InnerMessage, "None");
    JSRStringLarge_Set(&ex->StackTrace, "");
    ex->HResult = 0;
}

/**
 * @brief Create an exception with error code and message
 * @param errorCode The error code
 * @param message The error message
 * @return Initialized exception structure
 */
static inline ExceptionJSRSDK ExceptionJSRSDK_Create(C_ERROR_CODE errorCode, const char* message) {
    ExceptionJSRSDK ex;
    ex.ErrorCode = errorCode;
    JSRString_Set(&ex.Message, message);
    JSRString_Set(&ex.InnerMessage, "None");
    JSRStringLarge_Set(&ex.StackTrace, "");
    ex.HResult = 0;
    return ex;
}

/**
 * @brief Create an exception with error code, message, and inner message
 * @param errorCode The error code
 * @param message The error message
 * @param innerMessage The inner/nested error message
 * @return Initialized exception structure
 */
static inline ExceptionJSRSDK ExceptionJSRSDK_CreateWithInner(C_ERROR_CODE errorCode, 
                                                               const char* message,
                                                               const char* innerMessage) {
    ExceptionJSRSDK ex;
    ex.ErrorCode = errorCode;
    JSRString_Set(&ex.Message, message);
    JSRString_Set(&ex.InnerMessage, innerMessage);
    JSRStringLarge_Set(&ex.StackTrace, "");
    ex.HResult = 0;
    return ex;
}

#ifdef __cplusplus
}

// C++ convenience methods
#include <string>

namespace JSR {

/**
 * @brief Create exception from std::string (C++ only)
 */
inline ExceptionJSRSDK MakeException(C_ERROR_CODE errorCode, const std::string& message) {
    return ExceptionJSRSDK_Create(errorCode, message.c_str());
}

/**
 * @brief Create exception with inner message from std::string (C++ only)
 */
inline ExceptionJSRSDK MakeException(C_ERROR_CODE errorCode, 
                                     const std::string& message,
                                     const std::string& innerMessage) {
    return ExceptionJSRSDK_CreateWithInner(errorCode, message.c_str(), innerMessage.c_str());
}

/**
 * @brief Get error message as std::string (C++ only)
 */
inline std::string GetMessage(const ExceptionJSRSDK& ex) {
    return std::string(ex.Message.data);
}

/**
 * @brief Get inner message as std::string (C++ only)
 */
inline std::string GetInnerMessage(const ExceptionJSRSDK& ex) {
    return std::string(ex.InnerMessage.data);
}

/**
 * @brief Get stack trace as std::string (C++ only)
 */
inline std::string GetStackTrace(const ExceptionJSRSDK& ex) {
    return std::string(ex.StackTrace.data);
}

} // namespace JSR

#endif // __cplusplus
