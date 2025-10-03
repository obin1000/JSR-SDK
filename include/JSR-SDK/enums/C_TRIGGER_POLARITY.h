#pragma once
/**
 * Represents the polarity of the trigger.
 * Mirrors JSRDotNETSDK::TRIGGER_POLARITY.
 **/
enum class C_TRIGGER_POLARITY {
  RISING = 0,   // Rising edge.
  FALLING = 1,  // Falling edge.
  LAST_VAL = 2, // Last valid polarity.
};
