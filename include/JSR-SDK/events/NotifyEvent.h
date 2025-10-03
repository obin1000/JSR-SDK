#pragma once

#include "JSR-SDK/enums/C_DISCOVERY_STATE_FLAGS.h"
#include "JSR-SDK/enums/C_NOTIFY_TYPE.h"
#include "JSR-SDK/enums/C_PULSER_PROPERTY_DATA_TYPE.h"
#include "JSR-SDK/enums/C_PULSER_RECEIVER_STATE.h"
#include "JSR-SDK/enums/C_STATUS_CHANGE.h"
#include "JSR-SDK/structs/PulserReceiverID.h"
#include "JSR-SDK/structs/InstrumentID.h"

#include <functional>
#include <string>
#include <vector>


class NotifyEvent {
public:
  std::string model;

  C_PULSER_RECEIVER_STATE state;

  std::string newValue;

  std::string propertyName;

  bool wasSelected;

  std::string exceptionTypeInfo;

  std::string errorMsg;

  std::string errorText;

  double maxFrequency;

  std::vector<std::string> info;

  int prIndex;

  PulserReceiverID pulserReceiverId;
  std::string serial;

  C_DISCOVERY_STATE_FLAGS discoverState;

  InstrumentID instrumentId;

  std::string pluginName;

  C_NOTIFY_TYPE notifyType;

  C_PULSER_PROPERTY_DATA_TYPE dataType;
};

using NotifyCallback = std::function<void(const NotifyEvent &)>;
