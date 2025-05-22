#pragma once
/**
 * Represents the impedance of the trigger.
 * Mirrors JSRDotNETSDK::TRIGGER_IMPEDANCE.
 **/
enum class TriggerImpedance {
  LOW_Z = 0,    // Low impedance.
  HIGH_Z = 1,   // High impedance.
  LAST_VAL = 2, // Last valid impedance value.
  UNKNOWN = 3,  // Unknown impedance.
};