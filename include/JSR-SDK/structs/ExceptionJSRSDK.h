#pragma once

#include "JSR-SDK/enums/C_ERROR_CODE.h"
#include "JSR-SDK/boundary/CString.h"

#include <string>

class ExceptionJSRSDK {
private:
  C_ERROR_CODE errorCode;
  CString msg;
  CString innerMSG;

public:
  ExceptionJSRSDK() : errorCode(C_ERROR_CODE{}), msg(), innerMSG() {}

  ExceptionJSRSDK(C_ERROR_CODE errorCode, const std::string &message) {
    this->errorCode = errorCode;
    this->msg = CString::from_std_string(message);
    this->innerMSG = CString::from_std_string("None");
  }

  ExceptionJSRSDK(C_ERROR_CODE errorCode, const std::string &message,
                  const std::string &innerMessage) {
    this->errorCode = errorCode;
    this->msg = CString::from_std_string(message);
    this->innerMSG = CString::from_std_string(innerMessage);
  }

  ~ExceptionJSRSDK() {
    msg.free_cstring();
    innerMSG.free_cstring();
  }

  ExceptionJSRSDK(const ExceptionJSRSDK &other) {
    errorCode = other.errorCode;
    msg = CString::from_std_string(other.msg.to_std_string());
    innerMSG = CString::from_std_string(other.innerMSG.to_std_string());
  }

  ExceptionJSRSDK &operator=(const ExceptionJSRSDK &other) {
    if (this != &other) {
      msg.free_cstring();
      innerMSG.free_cstring();
      errorCode = other.errorCode;
      msg = CString::from_std_string(other.msg.to_std_string());
      innerMSG = CString::from_std_string(other.innerMSG.to_std_string());
    }
    return *this;
  }

  ExceptionJSRSDK(ExceptionJSRSDK &&other) noexcept
      : errorCode(other.errorCode), msg(other.msg), innerMSG(other.innerMSG) {
    other.msg = CString();
    other.innerMSG = CString();
  }

  ExceptionJSRSDK &operator=(ExceptionJSRSDK &&other) noexcept {
    if (this != &other) {
      msg.free_cstring();
      innerMSG.free_cstring();
      errorCode = other.errorCode;
      msg = other.msg;
      innerMSG = other.innerMSG;
      other.msg = CString();
      other.innerMSG = CString();
    }
    return *this;
  }

  C_ERROR_CODE getErrorCode() { return errorCode; }
  std::string getMessage() { return msg.to_std_string(); }
  std::string getInnerMessage() { return innerMSG.to_std_string(); }
};
