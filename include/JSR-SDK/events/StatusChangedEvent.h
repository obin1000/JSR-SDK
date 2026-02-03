#pragma once

#include "JSR-SDK/enums/C_PULSER_PROPERTY_DATA_TYPE.h"
#include "JSR-SDK/enums/C_PULSER_RECEIVER_STATE.h"
#include "JSR-SDK/enums/C_STATUS_CHANGE.h"
#include "JSR-SDK/enums/C_ERROR_CODE.h"

#include "JSR-SDK/structs/ExceptionJSRSDK.h"
#include "JSR-SDK/structs/PulserReceiverID.h"
#include "JSR-SDK/boundary/CString.h"

#include <functional>
#include <string>

class StatusChangedEvent {
public:
  StatusChangedEvent()
      : pulserState{}, dataType{}, changeType{}, errorCode{} {}

  CString pulserProperty;

  C_PULSER_RECEIVER_STATE pulserState;

  CString newValue;

  C_PULSER_PROPERTY_DATA_TYPE dataType;

  C_STATUS_CHANGE changeType;

  PulserReceiverID pulserReceiverId;

  CString errorMessage;

  C_ERROR_CODE errorCode;

  ExceptionJSRSDK thrownException;

  ~StatusChangedEvent() {
    pulserProperty.free_cstring();
    newValue.free_cstring();
    errorMessage.free_cstring();
  }

  StatusChangedEvent(const StatusChangedEvent &) = delete;
  StatusChangedEvent &operator=(const StatusChangedEvent &) = delete;

  StatusChangedEvent(StatusChangedEvent &&other) noexcept
      : pulserProperty(other.pulserProperty), pulserState(other.pulserState),
        newValue(other.newValue), dataType(other.dataType),
        changeType(other.changeType),
        pulserReceiverId(std::move(other.pulserReceiverId)),
        errorMessage(other.errorMessage), errorCode(other.errorCode),
        thrownException(std::move(other.thrownException)) {
    other.pulserProperty = CString();
    other.newValue = CString();
    other.errorMessage = CString();
  }

  StatusChangedEvent &operator=(StatusChangedEvent &&other) noexcept {
    if (this != &other) {
      pulserProperty.free_cstring();
      newValue.free_cstring();
      errorMessage.free_cstring();

      pulserProperty = other.pulserProperty;
      pulserState = other.pulserState;
      newValue = other.newValue;
      dataType = other.dataType;
      changeType = other.changeType;
      pulserReceiverId = std::move(other.pulserReceiverId);
      errorMessage = other.errorMessage;
      errorCode = other.errorCode;
      thrownException = std::move(other.thrownException);

      other.pulserProperty = CString();
      other.newValue = CString();
      other.errorMessage = CString();
    }
    return *this;
  }
};

using StatusChangeCallback = std::function<void(const StatusChangedEvent &)>;
