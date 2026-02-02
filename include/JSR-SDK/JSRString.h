/**
 * @file JSRString.h
 * @brief Simple C-compatible fixed-size string type
 * 
 * This provides a lightweight string type that works in both C and C++,
 * is safe across DLL boundaries, and doesn't require dynamic memory allocation.
 */

#pragma once

#ifndef JSRSTRING_H
#define JSRSTRING_H

#include <stddef.h>
#include <string.h>

#ifdef __cplusplus
#include <string>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Default maximum string length for JSR SDK strings
 */
#define JSR_STRING_MAX_LENGTH 256

/**
 * @brief Simple fixed-size string type for C/C++ interop
 * 
 * This type provides a safe, fixed-size string buffer that can be used
 * across DLL boundaries without memory allocation issues.
 */
typedef struct {
    char data[JSR_STRING_MAX_LENGTH];
} JSRString;

/**
 * @brief Large fixed-size string for longer content (e.g., stack traces)
 */
typedef struct {
    char data[JSR_STRING_MAX_LENGTH * 4];
} JSRStringLarge;

/**
 * @brief Initialize a JSRString to empty
 * @param str Pointer to JSRString to initialize
 */
static inline void JSRString_Init(JSRString* str) {
    if (str) {
        str->data[0] = '\0';
    }
}

/**
 * @brief Set JSRString from C string
 * @param str Pointer to JSRString
 * @param value C string to copy
 */
static inline void JSRString_Set(JSRString* str, const char* value) {
    if (!str) return;
    if (!value) {
        str->data[0] = '\0';
        return;
    }
    
    size_t len = strlen(value);
    if (len >= JSR_STRING_MAX_LENGTH) {
        len = JSR_STRING_MAX_LENGTH - 1;
    }
    
    memcpy(str->data, value, len);
    str->data[len] = '\0';
}

/**
 * @brief Get C string from JSRString
 * @param str Pointer to JSRString
 * @return Pointer to internal C string buffer
 */
static inline const char* JSRString_Get(const JSRString* str) {
    return str ? str->data : "";
}

/**
 * @brief Set JSRStringLarge from C string
 * @param str Pointer to JSRStringLarge
 * @param value C string to copy
 */
static inline void JSRStringLarge_Set(JSRStringLarge* str, const char* value) {
    if (!str) return;
    if (!value) {
        str->data[0] = '\0';
        return;
    }
    
    size_t len = strlen(value);
    size_t maxLen = sizeof(str->data) - 1;
    if (len >= maxLen) {
        len = maxLen;
    }
    
    memcpy(str->data, value, len);
    str->data[len] = '\0';
}

#ifdef __cplusplus
}

// C++ convenience functions
namespace JSR {

/**
 * @brief Create JSRString from std::string (C++ only)
 */
inline JSRString MakeString(const std::string& str) {
    JSRString result;
    JSRString_Set(&result, str.c_str());
    return result;
}

/**
 * @brief Convert JSRString to std::string (C++ only)
 */
inline std::string ToString(const JSRString& str) {
    return std::string(str.data);
}

/**
 * @brief Convert JSRStringLarge to std::string (C++ only)
 */
inline std::string ToString(const JSRStringLarge& str) {
    return std::string(str.data);
}

} // namespace JSR

#endif // __cplusplus

#endif // JSRSTRING_H
