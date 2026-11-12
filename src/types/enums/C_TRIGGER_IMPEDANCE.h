#pragma once
/**
 * Represents the impedance of the trigger.
 * Mirrors JSRDotNETSDK::TRIGGER_IMPEDANCE.
 **/
enum class C_TRIGGER_IMPEDANCE {
  LOW_Z = 0,    // Low impedance.
  HIGH_Z = 1,   // High impedance.
  LAST_VAL = 2, // Last valid impedance value.
};
