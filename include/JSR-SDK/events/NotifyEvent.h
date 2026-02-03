#pragma once

#include "JSR-SDK/enums/C_DISCOVERY_STATE_FLAGS.h"
#include "JSR-SDK/enums/C_NOTIFY_TYPE.h"
#include "JSR-SDK/enums/C_PULSER_PROPERTY_DATA_TYPE.h"
#include "JSR-SDK/enums/C_PULSER_RECEIVER_STATE.h"
#include "JSR-SDK/enums/C_STATUS_CHANGE.h"
#include "JSR-SDK/structs/PulserReceiverID.h"
#include "JSR-SDK/structs/InstrumentID.h"

#include <functional>
#include <cstring>

struct NotifyEvent {
  static constexpr size_t MAX_STRING_LENGTH = 256;
  static constexpr size_t MAX_INFO_ARRAY = 32;

  char model[MAX_STRING_LENGTH];
  C_PULSER_RECEIVER_STATE state;
  char newValue[MAX_STRING_LENGTH];
  char propertyName[MAX_STRING_LENGTH];
  bool wasSelected;
  char exceptionTypeInfo[MAX_STRING_LENGTH];
  char errorMsg[MAX_STRING_LENGTH];
  char errorText[MAX_STRING_LENGTH];
  double maxFrequency;

  char info[MAX_INFO_ARRAY][MAX_STRING_LENGTH];
  size_t infoCount;

  int prIndex;
  PulserReceiverID pulserReceiverId;
  char serial[MAX_STRING_LENGTH];
  C_DISCOVERY_STATE_FLAGS discoverState;
  InstrumentID instrumentId;
  char pluginName[MAX_STRING_LENGTH];
  C_NOTIFY_TYPE notifyType;
  C_PULSER_PROPERTY_DATA_TYPE dataType;

  NotifyEvent()
      : state{}, wasSelected(false), maxFrequency(0.0), infoCount(0),
        prIndex(0), discoverState{}, notifyType{}, dataType{} {
    model[0] = '\0';
    newValue[0] = '\0';
    propertyName[0] = '\0';
    exceptionTypeInfo[0] = '\0';
    errorMsg[0] = '\0';
    errorText[0] = '\0';
    serial[0] = '\0';
    pluginName[0] = '\0';
  }
};

using NotifyCallback = std::function<void(const NotifyEvent &)>;
