#pragma once

#include "JSR-SDK/enums/C_ERROR_CODE.h"

#include <string>

class ExceptionJSRSDK {
private:
  C_ERROR_CODE errorCode;
  std::string msg;
  std::string innerMSG;

public:
  ExceptionJSRSDK() = default;

  ExceptionJSRSDK(C_ERROR_CODE errorCode, std::string msg) {
    this->errorCode = errorCode;
    this->msg = msg;
    this->innerMSG = "None";
  }
  ExceptionJSRSDK(C_ERROR_CODE errorCode, std::string msg,
                  std::string innerMSG) {
    this->errorCode = errorCode;
    this->msg = msg;
    this->innerMSG = innerMSG;
  }

  C_ERROR_CODE getErrorCode() { return errorCode; }
  std::string getMessage() { return msg; }
  std::string getInnerMessage() { return innerMSG; }
};
