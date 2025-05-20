#pragma once

#include "InstrumentID.h"
#include <string>


class PulserReceiverID {
public:
  InstrumentID InstrumentId;
  int PulserReceiverIndex;

  std::string GetDescription() const {
    std::string description = "Pulser/Receiver: " + std::to_string(PulserReceiverIndex) + "\n";
    return description;
  }
  std::string ToString() { return GetDescription(); }
};