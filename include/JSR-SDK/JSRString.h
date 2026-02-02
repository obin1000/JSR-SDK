/**
 * @file JSRString.h
 * @brief Simple C-compatible fixed-size string type
 * 
 * This provides a lightweight string type that works in both C and C++,
 * is safe across DLL boundaries, and doesn't require dynamic memory allocation.
 * 
 * @section security Security Notes
 * - All strings are limited to 255 BYTES (not characters)
 * - UTF-8 multibyte characters count as multiple bytes
 * - Strings longer than 255 bytes are silently truncated
 * - All functions are thread-safe (no shared state)
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
 * @brief Default maximum string length for JSR SDK strings (in bytes)
 * @warning This is BYTES, not characters. UTF-8 may use multiple bytes per character.
 */
#define JSR_STRING_MAX_LENGTH 256

/**
 * @brief Simple fixed-size string type for C/C++ interop
 * 
 * This type provides a safe, fixed-size string buffer that can be used
 * across DLL boundaries without memory allocation issues.
 * 
 * @note The data array is always null-terminated
 * @note Maximum usable length is JSR_STRING_MAX_LENGTH - 1 (255 bytes)
 */
typedef struct JSRString {
    char data[JSR_STRING_MAX_LENGTH];
} JSRString;

/**
 * @brief Large fixed-size string for longer content (e.g., stack traces)
 */
typedef struct JSRStringLarge {
    char data[JSR_STRING_MAX_LENGTH * 4];
} JSRStringLarge;

/**
 * @brief Initialize a JSRString to empty
 * @param str Pointer to JSRString to initialize
 * @note Always call this before first use
 */
static inline void JSRString_Init(JSRString* str) {
    if (str) {
        str->data[0] = '\0';
    }
}

/**
 * @brief Set JSRString from C string with maximum length limit
 * @param str Pointer to JSRString
 * @param value C string to copy (may or may not be null-terminated)
 * @param maxLen Maximum number of bytes to read from value
 * @return 0 on success, -1 if str is NULL
 * 
 * @note This function is safe even if value is not null-terminated
 * @note If value contains more than maxLen bytes before null terminator,
 *       only maxLen bytes are copied
 * @note Result is always null-terminated
 * 
 * @code
 * char buffer[1000];
 * recv(socket, buffer, sizeof(buffer), 0);  // May not be null-terminated
 * JSRString str;
 * JSRString_SetN(&str, buffer, sizeof(buffer));  // Safe!
 * @endcode
 */
static inline int JSRString_SetN(JSRString* str, const char* value, size_t maxLen) {
    if (!str) return -1;
    if (!value) {
        str->data[0] = '\0';
        return 0;
    }
    
    // Safely find string length without reading beyond maxLen
    size_t len = 0;
    while (len < maxLen && value[len] != '\0') {
        len++;
    }
    
    // Cap at buffer size
    if (len >= JSR_STRING_MAX_LENGTH) {
        len = JSR_STRING_MAX_LENGTH - 1;
    }
    
    memcpy(str->data, value, len);
    str->data[len] = '\0';
    return 0;
}

/**
 * @brief Set JSRString from C string
 * @param str Pointer to JSRString
 * @param value C string to copy (must be null-terminated)
 * 
 * @warning value MUST be null-terminated. For untrusted input, use JSRString_SetN()
 * @note Strings longer than 255 bytes are truncated
 * @note Result is always null-terminated
 * @note UTF-8 strings: limit is 255 bytes, not characters
 * 
 * @code
 * JSRString str;
 * JSRString_Set(&str, "Hello");           // ? Safe
 * JSRString_Set(&str, userInput);         // ?? Only if userInput is trusted and null-terminated
 * JSRString_SetN(&str, userInput, 1024);  // ? Safer for untrusted input
 * @endcode
 */
static inline void JSRString_Set(JSRString* str, const char* value) {
    if (!str) return;
    if (!value) {
        str->data[0] = '\0';
        return;
    }
    
    // Use bounded length check for safety
    // Assume max reasonable string length to prevent reading corrupted memory
    const size_t MAX_SAFE_STRLEN = 1024 * 1024;  // 1 MB should be enough for any string
    
    size_t len = 0;
    while (len < MAX_SAFE_STRLEN && value[len] != '\0') {
        len++;
    }
    
    if (len >= JSR_STRING_MAX_LENGTH) {
        len = JSR_STRING_MAX_LENGTH - 1;
    }
    
    memcpy(str->data, value, len);
    str->data[len] = '\0';
}

/**
 * @brief Get C string from JSRString
 * @param str Pointer to JSRString
 * @return Pointer to internal C string buffer (always null-terminated)
 * @note Returns empty string if str is NULL (never returns NULL)
 * @note The returned pointer is valid as long as str exists
 * @warning Do not modify the returned string directly
 */
static inline const char* JSRString_Get(const JSRString* str) {
    return str ? str->data : "";
}

/**
 * @brief Initialize a JSRStringLarge to empty
 * @param str Pointer to JSRStringLarge to initialize
 */
static inline void JSRStringLarge_Init(JSRStringLarge* str) {
    if (str) {
        str->data[0] = '\0';
    }
}

/**
 * @brief Set JSRStringLarge from C string
 * @param str Pointer to JSRStringLarge
 * @param value C string to copy (must be null-terminated)
 * @note Maximum length is (JSR_STRING_MAX_LENGTH * 4) - 1 bytes (1023 bytes)
 */
static inline void JSRStringLarge_Set(JSRStringLarge* str, const char* value) {
    if (!str) return;
    if (!value) {
        str->data[0] = '\0';
        return;
    }
    
    const size_t maxLen = sizeof(str->data) - 1;
    const size_t MAX_SAFE_STRLEN = 1024 * 1024;
    
    size_t len = 0;
    while (len < MAX_SAFE_STRLEN && value[len] != '\0') {
        len++;
    }
    
    if (len > maxLen) {
        len = maxLen;
    }
    
    memcpy(str->data, value, len);
    str->data[len] = '\0';
}

/**
 * @brief Get C string from JSRStringLarge
 * @param str Pointer to JSRStringLarge
 * @return Pointer to internal C string buffer (always null-terminated)
 */
static inline const char* JSRStringLarge_Get(const JSRStringLarge* str) {
    return str ? str->data : "";
}

#ifdef __cplusplus
}

// C++ convenience functions
namespace JSR {

/**
 * @brief Create JSRString from std::string (C++ only)
 * @param str std::string to convert
 * @return JSRString containing the string data (truncated if > 255 bytes)
 */
inline JSRString MakeString(const std::string& str) {
    JSRString result;
    JSRString_Set(&result, str.c_str());
    return result;
}

/**
 * @brief Convert JSRString to std::string (C++ only)
 * @param str JSRString to convert
 * @return std::string containing a copy of the data
 */
inline std::string ToString(const JSRString& str) {
    return std::string(str.data);
}

/**
 * @brief Convert JSRStringLarge to std::string (C++ only)
 * @param str JSRStringLarge to convert
 * @return std::string containing a copy of the data
 */
inline std::string ToString(const JSRStringLarge& str) {
    return std::string(str.data);
}

} // namespace JSR

#endif // __cplusplus

#endif // JSRSTRING_H
