#pragma once

#include "JSR-SDK/enums/PulserPropertyDataTypeC.h"
#include "JSR-SDK/enums/PulserReceiverStateC.h"
#include "JSR-SDK/enums/StatusChange.h"

#include "JSR-SDK/ExceptionJSRSDK.h"
#include "JSR-SDK/PulserReceiverID.h"

#include <string>

class StatusChangedEvent {
public:
  std::string pulserProperty;

  PulserReceiverStateC pulserState;

  // object NewValue;

  PulserPropertyDataTypeC dataType;

  StatusChange changeType;

  PulserReceiverID pulserReceiverId;

  std::string errorMessage;

  ErrorCode errorCode;

  ExceptionJSRSDK thrownException;
};