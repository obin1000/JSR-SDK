#pragma once
/**
 * Represents the impedance of the pulser.
 * Mirrors JSRDotNETSDK::PULSER_IMPEDANCE.
 **/
enum class PulserImpedance {
  LOW_Z = 0,    // Low impedance.
  HIGH_Z = 1,   // High impedance.
  LAST_VAL = 2, // Last valid impedance value.
  UNKNOWN = 3,  // Unknown impedance.
};