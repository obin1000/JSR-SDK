#pragma once

enum class DiscoveryStateFlags {
  NONE = 0,
  ACTIVE = 1,
  PENDING = 2,
  STOPPED_ON_ERR = 4,
};
