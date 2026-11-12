#pragma once
/**
 * Represents the mode of the receiver.
 * Mirrors JSRDotNETSDK::RECEIVER_MODE.
 **/
enum class C_RECEIVER_MODE {
  ECHO = 0,     // Echo mode.
  THRU = 1,     // Through mode.
  BOTH = 2,     // Both modes.
  LAST_VAL = 3, // Last valid mode.
};
