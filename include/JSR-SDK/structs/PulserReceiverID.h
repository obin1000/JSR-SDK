#pragma once

#include "InstrumentID.h"
#include <string>

/**
 * @struct PulserReceiverID
 * @brief Identifies a specific pulser/receiver channel within an instrument.
 * 
 * Many ultrasonic testing instruments contain multiple pulser/receiver channels.
 * This structure combines instrument identification with a channel index to
 * uniquely identify a specific pulser/receiver unit.
 * 
 * For example, a single physical device might have 4 independent pulser/receiver
 * channels, each accessible via a different index (0-3).
 * 
 * Mirrors JSRDotNETSDK::IPulserReceiverIdentity interface.
 * 
 * @note This is a standard-layout structure with optional C++ helpers.
 */
struct PulserReceiverID {
  /**
   * @brief The instrument that contains this pulser/receiver.
   * 
   * Contains model name, serial number, port, and plugin information
   * for the physical device.
   */
  InstrumentID InstrumentId;

  /**
   * @brief Zero-based index of the pulser/receiver channel.
   * 
   * For instruments with multiple channels, this identifies which specific
   * channel this ID refers to. Single-channel devices typically use index 0.
   * 
   * @note Valid range depends on the specific instrument model.
   */
  int PulserReceiverIndex;

  /**
   * @brief Default constructor - initializes to index 0.
   * 
   * The InstrumentId is default-constructed (all fields empty).
   * The PulserReceiverIndex is set to 0.
   */
  PulserReceiverID() : PulserReceiverIndex(0) {}

#ifdef __cplusplus
  /**
   * @brief Constructs a detailed description of the pulser/receiver.
   *
   * Creates a multi-line string containing the channel index and all
   * instrument identification information.
   *
   * @return A formatted string with the pulser/receiver description.
   * 
   * @example
   * @code
   * PulserReceiverID pr;
   * std::string desc = pr.GetDescription();
   * // Output:
   * // Pulser/Receiver: 0
   * // Model: JSR-DPR300
   * // Serial Number: JB0104
   * // Port: COM3
   * // Plugin Name: JSR-DPR
   * @endcode
   */
  std::string GetDescription() const {
    std::string description =
        "Pulser/Receiver: " + std::to_string(PulserReceiverIndex) + "\n" +
        InstrumentId.GetDescription();
    return description;
  }

  /**
   * @brief Converts the pulser/receiver details to a string representation.
   *
   * Convenience method that calls GetDescription().
   *
   * @return A string containing the pulser/receiver's description.
   */
  std::string ToString() const { return GetDescription(); }
#endif
};
