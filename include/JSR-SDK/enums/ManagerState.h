#pragma once
/**
 * Represents the state of the manager.
 * Mirrors JSRDotNETSDK::JSRSDKManager:MANAGER_STATE.
 **/
enum class ManagerState {
  NOT_STARTED = 0,    // Manager has not started.
  PLUGINS_LOADED = 1, // Plugins have been loaded.
  RUNNING = 2,        // Manager is running.
  SHUTTING_DOWN = 3,  // Manager is shutting down.
  UNKNOWN = 4,        // Manager state is unknown.
};
