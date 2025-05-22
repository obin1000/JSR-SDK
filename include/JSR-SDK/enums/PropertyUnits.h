#pragma once
/**
 * Represents the units of a property.
 * Mirrors JSRDotNETSDK::PROPERTY_UNIT.
 **/
enum class PropertyUnits {
  UNIT_NONE = 0,        // No unit.
  UNIT_HERTZ = 1,       // Hertz.
  UNIT_KILOHERTZ = 2,   // Kilohertz.
  UNIT_MEGAHERTZ = 3,   // Megahertz.
  UNIT_MICROJOULES = 4, // Microjoules.
  UNIT_JOULES = 5,      // Joules.
  UNIT_WATTS = 6,       // Watts.
  UNIT_DB = 7,          // Decibels.
  UNIT_VOLTS = 8,       // Volts.
  UNIT_OHMS = 9,        // Ohms.
  UNIT_PICOFARADS = 10, // Picofarads.
  UNKNOWN = 11,         // Unknown unit.
};
