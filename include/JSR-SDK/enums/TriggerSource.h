#pragma once
/**
 * Represents the source of the trigger.
 * Mirrors JSRDotNETSDK::TRIGGER_SOURCE.
 **/
enum class TriggerSource {
  INTERNAL = 0, // Internal trigger source.
  EXTERNAL = 1, // External trigger source.
  SLAVE = 2,    // Slave trigger source.
  LAST_VAL = 3, // Last valid trigger source.
  UNKNOWN = 4,  // Unknown trigger source.
};
