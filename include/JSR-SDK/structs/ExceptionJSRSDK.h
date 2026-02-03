#pragma once

#include "JSR-SDK/enums/C_ERROR_CODE.h"
#include <string>
#include <cstring>

class ExceptionJSRSDK {
private:
  static constexpr size_t MAX_STRING_LENGTH = 512;

  C_ERROR_CODE errorCode;
  char msg[MAX_STRING_LENGTH];
  char innerMSG[MAX_STRING_LENGTH];

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
  ExceptionJSRSDK() : errorCode(C_ERROR_CODE{}) {
    msg[0] = '\0';
    innerMSG[0] = '\0';
  }

  ExceptionJSRSDK(C_ERROR_CODE errorCode, const std::string &message) {
    this->errorCode = errorCode;
    safeCopy(msg, message.c_str(), MAX_STRING_LENGTH);
    safeCopy(innerMSG, "None", MAX_STRING_LENGTH);
  }

  ExceptionJSRSDK(C_ERROR_CODE errorCode, const std::string &message,
                  const std::string &innerMessage) {
    this->errorCode = errorCode;
    safeCopy(msg, message.c_str(), MAX_STRING_LENGTH);
    safeCopy(innerMSG, innerMessage.c_str(), MAX_STRING_LENGTH);
  }

  C_ERROR_CODE getErrorCode() const { return errorCode; }
  std::string getMessage() const { return std::string(msg); }
  std::string getInnerMessage() const { return std::string(innerMSG); }
};
