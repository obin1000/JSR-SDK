#pragma once
/**
 * Indicates whether the system is currently pulsing.
 * Mirrors JSRDotNETSDK::IS_PLUSING.
 **/
enum class C_IS_PULSING {
  INACTIVE = 0, // Pulsing is inactive.
  ACTIVE = 1,   // Pulsing is active.
  UNKNOWN = 2,  // Pulsing state is unknown.
};
