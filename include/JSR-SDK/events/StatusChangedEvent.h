#pragma once

#include "JSR-SDK/enums/PulserPropertyDataType.h"
#include "JSR-SDK/enums/PulserReceiverState.h"
#include "JSR-SDK/enums/StatusChange.h"

#include "JSR-SDK/ExceptionJSRSDK.h"
#include "JSR-SDK/PulserReceiverID.h"

#include <string>

class StatusChangedEvent {
public:
  std::string pulserProperty;

  PulserReceiverState pulserState;

  // object NewValue;

  PulserPropertyDataType dataType;

  StatusChange changeType;

  PulserReceiverID pulserReceiverId;

  std::string errorMessage;

  ErrorCode errorCode;

  ExceptionJSRSDK thrownException;
};