#pragma once

#include "JSR-SDK/enums/PulserPropertyDataTypeC.h"
#include "JSR-SDK/enums/PulserReceiverStateC.h"
#include "JSR-SDK/enums/StatusChange.h"

#include "JSR-SDK/ExceptionJSRSDK.h"
#include "JSR-SDK/PulserReceiverID.h"

#include <functional>
#include <string>

class StatusChangedEvent {
public:
  StatusChangedEvent() {}

  std::string pulserProperty;

  PulserReceiverStateC pulserState;

  std::string newValue;

  PulserPropertyDataTypeC dataType;

  StatusChange changeType;

  PulserReceiverID pulserReceiverId;

  std::string errorMessage;

  ErrorCode errorCode;

  ExceptionJSRSDK thrownException;
};

using StatusChangeCallback = std::function<void(const StatusChangedEvent &)>;
