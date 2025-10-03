#pragma once

enum class StatusChange {
  INSTRUMENT_DISCONNECT = 0,
  PROPERTY_CHANGE = 1,
  OPERATIONAL_FAULT = 2,
  STATE_CHANGE = 3,
};
