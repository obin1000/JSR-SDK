#pragma once

enum class ManagerState {
  NOT_STARTED,
  PLUGINS_LOADED,
  RUNNING,
  SHUTTING_DOWN,
  UNKNOWN,
};
