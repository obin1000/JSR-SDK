#pragma once

#include "JSR-SDK/enums/C_PULSER_PROPERTY_DATA_TYPE.h"
#include "JSR-SDK/enums/C_PULSER_RECEIVER_STATE.h"
#include "JSR-SDK/enums/C_STATUS_CHANGE.h"
#include "JSR-SDK/enums/C_ERROR_CODE.h"

#include "JSR-SDK/structs/ExceptionJSRSDK.h"
#include "JSR-SDK/structs/PulserReceiverID.h"

#include <functional>
#include <string>
#include <cstring>

struct StatusChangedEvent {
  static constexpr size_t MAX_STRING_LENGTH = 256;

  char pulserProperty[MAX_STRING_LENGTH];
  C_PULSER_RECEIVER_STATE pulserState;
  char newValue[MAX_STRING_LENGTH];
  C_PULSER_PROPERTY_DATA_TYPE dataType;
  C_STATUS_CHANGE changeType;
  PulserReceiverID pulserReceiverId;
  char errorMessage[MAX_STRING_LENGTH];
  C_ERROR_CODE errorCode;
  ExceptionJSRSDK thrownException;

  StatusChangedEvent()
      : pulserState{}, dataType{}, changeType{}, errorCode{} {
    pulserProperty[0] = '\0';
    newValue[0] = '\0';
    errorMessage[0] = '\0';
  }
};

using StatusChangeCallback = std::function<void(const StatusChangedEvent &)>;
