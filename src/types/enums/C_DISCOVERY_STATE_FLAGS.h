#pragma once
/**
 * Discovery state. Mirrors JSRDotNETSDK::DiscoveryStateFlags.
 *
 * These are bit flags: a value can combine several of them (for example
 * ACTIVE | PENDING). Test a flag with a bitwise AND on the integer value.
 **/
enum class C_DISCOVERY_STATE_FLAGS {
  NONE = 0,
  ACTIVE = 1,
  PENDING = 2,
  STOPPED_ON_ERR = 4,
};
