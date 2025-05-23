#pragma once

/**
 * @brief Represents the various states of a pulser receiver.
 * Mirrors JSRDotNETSDK::PulserReceiverState
 */
enum class PulserReceiverStateC {
  DETECTED = 0,     // The pulser receiver has been detected.
  INITIALIZING = 1, // The pulser receiver is in the process of initializing.
  READY = 2,        // The pulser receiver is ready for operation.
  ERRORC = 3,        // The pulser receiver has encountered an error.
  NO_PULSER = 4,    // No pulser is connected to the receiver.
  DETACHED = 5,     // The pulser receiver has been detached.
  UNKNOWN = 6,       // The state of the pulser receiver is unknown.
};
