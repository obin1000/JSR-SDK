#pragma once
/**
 * Represents the mode of the receiver.
 * Mirrors JSRDotNETSDK::RECEIVER_MODE.
 **/
enum class RecieverMode {
  ECHO = 0,     // Echo mode.
  THRU = 1,     // Through mode.
  BOTH = 2,     // Both modes.
  LAST_VAL = 3, // Last valid mode.
  UNKNOWN = 4,  // Unknown mode.
};
