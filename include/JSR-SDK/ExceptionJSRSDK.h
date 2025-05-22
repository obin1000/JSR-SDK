#pragma once

#include "JSR-SDK/enums/ErrorCode.h"

#include <string>

class ExceptionJSRSDK {
private:
  ErrorCode errorCode;
  std::string msg;
  std::string innerMSG;

public:
  ExceptionJSRSDK(ErrorCode errorCode, std::string msg) {
    this->errorCode = errorCode;
    this->msg = msg;
    this->innerMSG = "None";
  }
  ExceptionJSRSDK(ErrorCode errorCode, std::string msg, std::string innerMSG) {
    this->errorCode = errorCode;
    this->msg = msg;
    this->innerMSG = innerMSG;
  }

  ErrorCode getErrorCode() { return errorCode; }
  std::string getMessage() { return msg; }
  std::string getInnerMessage() { return innerMSG; }
};
