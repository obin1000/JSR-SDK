#pragma once

#include "InstrumentID.h"
#include <string>

/**
 * @class PulserReceiverID
 * @brief Represents a Pulser/Receiver with an associated instrument and index.
 *
 * This class provides functionality to describe a Pulser/Receiver by combining
 * its index and the description of the associated instrument.
 * Mirrors JSRDotNETSDK::PulserReceiverIdentity
 */
class PulserReceiverID {
public:
  /**
   * @brief The instrument associated with this Pulser/Receiver.
   */
  InstrumentID InstrumentId;

  /**
   * @brief The index of the Pulser/Receiver.
   */
  int PulserReceiverIndex;

  /**
   * @brief Constructs a detailed description of the Pulser/Receiver.
   *
   * The description includes the Pulser/Receiver index and the description
   * of the associated instrument.
   *
   * @return A string containing the Pulser/Receiver description.
   */
  std::string GetDescription() const {
    std::string description =
        "Pulser/Receiver: " + std::to_string(PulserReceiverIndex) + "\n" +
        InstrumentId.GetDescription();
    return description;
  }

  /**
   * @brief Converts the Pulser/Receiver details to a string representation.
   *
   * This method is an alias for GetDescription().
   *
   * @return A string containing the Pulser/Receiver's description.
   */
  std::string ToString() { return GetDescription(); }
};
