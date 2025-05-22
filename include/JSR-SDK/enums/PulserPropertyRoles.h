#pragma once
/**
 * Defines roles for pulser properties.
 * Mirrors JSRDotNETSDK::PULSER_PROPERTY_ROLES.
 **/
enum class PulserPropertyRoles {
  DIRECT = 0,                 // Direct role.
  DIRECTSUPPORTED = 1,        // Direct role is supported.
  SPECIFICVALUESUPPORTED = 2, // Specific values are supported.
  DIRECTVALUES = 3,           // Direct values.
  STEPSIZE = 4,               // Step size.
  STEPSIZESUPPORTED = 5,      // Step size is supported.
  INDEX = 6,                  // Index role.
  INDEXMAX = 7,               // Maximum index.
  INDEXSUPPORTED = 8,         // Index is supported.
  MAX = 9,                    // Maximum value.
  MIN = 10,                   // Minimum value.
  INDEXVALUES = 11,           // Index values.
  NUMERATOR = 12,             // Numerator role.
  OTHER = 13,                 // Other role.
  UKNOWN = 14,                // Unknown role.
};