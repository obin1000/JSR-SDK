#pragma once

/**
 * Represents the type of connection used by the system.
 * Mirrors JSRDotNETSDK::CONNECTION_TYPE.
 **/
enum class ConnectionType {
  SOFTWARE = 0,   // Software-based connection.
  SERIALPORT = 1, // Serial port connection.
  PCI = 2,        // PCI connection.
  USB = 3,        // USB connection.
  ETHERNET = 4,   // Ethernet connection.
  FIREWIRE = 5,   // FireWire connection.
  BLUETOOTH = 6,  // Bluetooth connection.
  UNKNOWN = 7,    // Unknown connection type.
};
