#pragma once

#include "JSR-SDK/enums/DiscoveryStateFlags.h"
#include "JSR-SDK/enums/NotifyType.h"
#include "JSR-SDK/enums/PulserPropertyDataType.h"
#include "JSR-SDK/enums/PulserReceiverState.h"
#include "JSR-SDK/enums/StatusChange.h"
#include "JSR-SDK/PulserReceiverID.h"
#include "JSR-SDK/InstrumentID.h"

#include <string>
#include <vector>

class StatusChangedEvent {
public:
  std::string model;

  PulserReceiverState state;

  // object newValue;

  std::string propertyName;

  bool wasSelected;

  std::string exceptionTypeInfo;

  std::string errorMsg;

  std::string errorText;

  double maxFrequency;

  std::vector<std::string> info;

  int PRIndex;

  PulserReceiverID PulserReceiverId;
  std::string Serial;

  DiscoveryStateFlags DiscoverState;

  InstrumentID instrumentId;

  std::string pluginName;

  NotifyType notifyType;

  PulserPropertyDataType DataType;
};