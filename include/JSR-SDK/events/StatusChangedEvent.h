#pragma once

#include "JSR-SDK/enums/C_PULSER_PROPERTY_DATA_TYPE.h"
#include "JSR-SDK/enums/C_PULSER_RECEIVER_STATE.h"
#include "JSR-SDK/enums/C_STATUS_CHANGE.h"
#include "JSR-SDK/enums/C_ERROR_CODE.h"

#include "JSR-SDK/structs/ExceptionJSRSDK.h"
#include "JSR-SDK/structs/PulserReceiverID.h"

#include <functional>
#include <string>

class StatusChangedEvent {
public:
  StatusChangedEvent() {}

  std::string pulserProperty;

  C_PULSER_RECEIVER_STATE pulserState;

  std::string newValue;

  C_PULSER_PROPERTY_DATA_TYPE dataType;

  C_STATUS_CHANGE changeType;

  PulserReceiverID pulserReceiverId;

  std::string errorMessage;

  C_ERROR_CODE errorCode;

  ExceptionJSRSDK thrownException;
};

using StatusChangeCallback = std::function<void(const StatusChangedEvent &)>;
