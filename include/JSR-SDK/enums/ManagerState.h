#pragma once

enum class ManagerState {
  NOT_STARTED = 0,
  PLUGINS_LOADED = 1,
  RUNNING = 2,
  SHUTTING_DOWN = 3,
  UNKNOWN = 4,
};
