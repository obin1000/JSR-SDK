#pragma once

#include "JSR-SDK/enums/DiscoveryStateFlagsC.h"
#include "JSR-SDK/enums/NotifyType.h"
#include "JSR-SDK/enums/PulserPropertyDataTypeC.h"
#include "JSR-SDK/enums/PulserReceiverStateC.h"
#include "JSR-SDK/enums/StatusChange.h"
#include "JSR-SDK/PulserReceiverID.h"
#include "JSR-SDK/InstrumentID.h"

#include <functional>
#include <string>
#include <vector>


class NotifyEvent {
public:
  std::string model;

  PulserReceiverStateC state;

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

  DiscoveryStateFlagsC discoverState;

  InstrumentID instrumentId;

  std::string pluginName;

  NotifyType notifyType;

  PulserPropertyDataTypeC dataType;
};

using NotifyCallback = std::function<void(const NotifyEvent &)>;
