#pragma once
/**
 * Indicates whether the system is within or over the power limit.
 * Mirrors JSRDotNETSDK::POWER_LIMIT.
 **/
enum class PowerLimit {
  WITHIN_LIMIT = 0, // Power is within the limit.
  OVER_LIMIT = 1,   // Power is over the limit.
  UNKNOWN = 2,      // Power limit state is unknown.
};