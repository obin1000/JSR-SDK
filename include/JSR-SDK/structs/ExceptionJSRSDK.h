#pragma once

#include "JSR-SDK/enums/C_ERROR_CODE.h"
#include <string>
#include <cstring>

/**
 * @class ExceptionJSRSDK
 * @brief Represents an exception that occurred within the JSR SDK.
 * 
 * This class encapsulates error information including an error code,
 * error message, and optionally an inner exception message. It provides
 * a safe way to pass exception information across DLL boundaries without
 * dynamic memory allocation.
 * 
 * All string data is stored in fixed-size character arrays to ensure
 * ABI stability and compatibility with C code.
 * 
 * @note This is a POD-like structure that can be safely passed across
 *       module boundaries and used in C/C++ interop scenarios.
 */
class ExceptionJSRSDK {
private:
  /// Maximum length for error message strings (including null terminator)
  static constexpr size_t MAX_STRING_LENGTH = 512;

  /// Error code categorizing the type of error
  C_ERROR_CODE errorCode;
  
  /// Primary error message
  char msg[MAX_STRING_LENGTH];
  
  /// Inner exception message (if this exception wraps another)
  char innerMSG[MAX_STRING_LENGTH];

  /**
   * @brief Safely copies a string to a fixed-size buffer.
   * 
   * Ensures the destination buffer is properly null-terminated and
   * truncates the source if necessary to fit in the destination.
   * 
   * @param dest Destination buffer
   * @param src Source string (null-terminated)
   * @param destSize Size of destination buffer
   */
  static void safeCopy(char *dest, const char *src, size_t destSize) {
    if (dest == nullptr || destSize == 0)
      return;

    size_t len = std::strlen(src);
    if (len >= destSize) {
      len = destSize - 1;
    }

    if (len > 0) {
      std::memcpy(dest, src, len);
    }
    dest[len] = '\0';
  }

public:
  /**
   * @brief Default constructor - creates an exception with no error.
   * 
   * Initializes all fields to safe defaults:
   * - errorCode: default C_ERROR_CODE (typically NO_ERROR)
   * - msg: empty string
   * - innerMSG: empty string
   */
  ExceptionJSRSDK() : errorCode(C_ERROR_CODE{}) {
    msg[0] = '\0';
    innerMSG[0] = '\0';
  }

  /**
   * @brief Constructs an exception with an error code and message.
   * 
   * @param errorCode The error code categorizing this exception
   * @param message The error message describing what went wrong
   */
  ExceptionJSRSDK(C_ERROR_CODE errorCode, const std::string &message) {
    this->errorCode = errorCode;
    safeCopy(msg, message.c_str(), MAX_STRING_LENGTH);
    safeCopy(innerMSG, "None", MAX_STRING_LENGTH);
  }

  /**
   * @brief Constructs an exception with error code, message, and inner exception.
   * 
   * Use this constructor when wrapping another exception or providing
   * additional context about a nested error.
   * 
   * @param errorCode The error code categorizing this exception
   * @param message The primary error message
   * @param innerMessage Message from the inner/wrapped exception
   */
  ExceptionJSRSDK(C_ERROR_CODE errorCode, const std::string &message,
                  const std::string &innerMessage) {
    this->errorCode = errorCode;
    safeCopy(msg, message.c_str(), MAX_STRING_LENGTH);
    safeCopy(innerMSG, innerMessage.c_str(), MAX_STRING_LENGTH);
  }

  /**
   * @brief Gets the error code.
   * @return The C_ERROR_CODE value indicating the type of error
   */
  C_ERROR_CODE getErrorCode() const { return errorCode; }
  
  /**
   * @brief Gets the error message.
   * @return The primary error message as a std::string
   */
  std::string getMessage() const { return std::string(msg); }
  
  /**
   * @brief Gets the inner exception message.
   * @return The inner exception message, or "None" if not applicable
   */
  std::string getInnerMessage() const { return std::string(innerMSG); }
};
