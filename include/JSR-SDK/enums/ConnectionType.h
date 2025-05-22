#pragma once

enum class ConnectionType {
  SOFTWARE = 0,
  SERIALPORT = 1,
  PCI = 2,
  USB = 3,
  ETHERNET = 4,
  FIREWIRE = 5,
  BLUETOOTH = 6,
  UNKNOWN = 7,
};
