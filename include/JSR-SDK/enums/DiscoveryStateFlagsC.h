#pragma once

enum class DiscoveryStateFlagsC {
  NONE = 0,
  ACTIVE = 1,
  PENDING = 2,
  STOPPED_ON_ERR = 4,
  UNKNOWN = 8,
};
