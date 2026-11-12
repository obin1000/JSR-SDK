// shutdown_check.cpp
// A .NET (/clr) program for ManagerShutdownTest (#13). Twice, it creates a
// manager, selects a simulated pulser-receiver, changes a setting and
// destroys the manager; then it returns.
//
// Before #13 was fixed, destroying a manager never shut the .NET manager
// down. Its threads lived on, and a .NET program waits for its threads when
// it exits, so this program hung. It also fails if a callback runs after
// JSR_Destroy() returned.
#include "JSR-SDK/jsr_sdk.h"

#include <atomic>
#include <chrono>
#include <cstdio>
#include <thread>

namespace {

const char *const kPlugin = "JSRDotNET_DiagLib";

std::atomic<bool> destroyed{false};
std::atomic<int> callsAfterDestroy{0};

void JSR_CALL onStatus(const JSR_StatusChangedEvent *, void *) {
  if (destroyed) {
    ++callsAfterDestroy;
  }
}

void JSR_CALL onNotify(const JSR_NotifyEvent *, void *) {
  if (destroyed) {
    ++callsAfterDestroy;
  }
}

int fail(int round, const char *what) {
  char message[512];
  JSR_GetLastErrorMessage(message, sizeof message);
  std::fprintf(stderr, "round %d: %s failed: %s\n", round, what, message);
  return 1;
}

int useManager(int round) {
  JSR_Manager *manager = nullptr;
  if (JSR_Create(&manager) != JSR_OK) {
    return fail(round, "JSR_Create");
  }
  destroyed = false;
  if (JSR_SetStatusChangeCallback(manager, onStatus, nullptr) != JSR_OK ||
      JSR_SetNotifyCallback(manager, onNotify, nullptr) != JSR_OK ||
      JSR_LoadPluginsFromBinaryDir(manager) != JSR_OK ||
      JSR_AddManagedPlugin(manager, kPlugin) != JSR_OK ||
      JSR_AddPluginOpenOption(manager, kPlugin, "SHOW_SIMULATOR_WINDOWS",
                              "NO") != JSR_OK ||
      JSR_AddPluginOpenOption(manager, kPlugin, "CONNECT_ON_OPEN", "YES") !=
          JSR_OK ||
      JSR_SetDiscoveryEnable(manager, 1) != JSR_OK) {
    return fail(round, "setting up the simulator");
  }

  bool selected = false;
  const auto deadline =
      std::chrono::steady_clock::now() + std::chrono::seconds(20);
  while (!selected && std::chrono::steady_clock::now() < deadline) {
    selected =
        JSR_SetCurrentPulserReceiverByModel(manager, "SIM", "00", 0) == JSR_OK;
    if (!selected) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }
  if (!selected) {
    return fail(round, "selecting the simulated pulser-receiver");
  }
  if (JSR_SetGainIndex(manager, 3 + round) != JSR_OK) {
    return fail(round, "JSR_SetGainIndex");
  }

  if (JSR_Destroy(manager) != JSR_OK) {
    return fail(round, "JSR_Destroy");
  }
  destroyed = true;
  // Events still on their way would arrive now.
  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::printf("round %d: created, used and destroyed a manager\n", round);
  return 0;
}

} // namespace

int main() {
  for (int round = 1; round <= 2; ++round) {
    if (useManager(round) != 0) {
      return 1;
    }
  }
  if (callsAfterDestroy != 0) {
    std::fprintf(stderr, "%d callback(s) ran after JSR_Destroy returned\n",
                 callsAfterDestroy.load());
    return 1;
  }
  std::printf("OK, returning from main\n");
  return 0;
}
