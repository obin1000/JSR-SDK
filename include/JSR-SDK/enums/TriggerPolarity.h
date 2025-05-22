#pragma once
/**
 * Represents the polarity of the trigger.
 * Mirrors JSRDotNETSDK::TRIGGER_POLARITY.
 **/
enum class TriggerPolarity {
  RISING = 0,   // Rising edge.
  FALLING = 1,  // Falling edge.
  LAST_VAL = 2, // Last valid polarity.
  UNKNOWN = 3,  // Unknown polarity.
};
