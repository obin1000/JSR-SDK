// Smoke test for the C++ interface in JSRSDKManager.hpp.
//
// Needs no hardware: it starts the .NET runtime inside JSR-SDK.dll, loads the
// plugins that sit next to the program, checks that an SDK error reaches the
// caller as a C++ exception, then discovers, selects and configures a
// simulated pulser-receiver and shuts the manager down again.
#include "JSR-SDK/JSRSDKManager.hpp"

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <exception>
#include <string>
#include <thread>
#include <vector>

namespace {

int failures = 0;

#define CHECK(cond)                                                            \
  do {                                                                         \
    if (!(cond)) {                                                             \
      std::fprintf(stderr, "FAILED %s:%d: %s\n", __FILE__, __LINE__, #cond);   \
      ++failures;                                                              \
    }                                                                          \
  } while (0)

std::vector<std::string> pluginNames(JSRSDKManager &manager) {
  constexpr size_t kLength = 256;
  const size_t count = manager.GetPluginNames(nullptr, 0, 0);
  std::vector<std::vector<char>> storage(count, std::vector<char>(kLength));
  std::vector<char *> buffers;
  for (auto &s : storage) {
    buffers.push_back(s.data());
  }
  const size_t written =
      manager.GetPluginNames(buffers.data(), buffers.size(), kLength);
  CHECK(written == count);

  std::vector<std::string> names;
  for (const auto *b : buffers) {
    names.emplace_back(b);
  }
  return names;
}

void run(JSRSDKManager &manager) {
  manager.loadPluginsFromBinaryDir();
  CHECK(manager.getPluginsLoaded());

  const auto names = pluginNames(manager);
  std::printf("Loaded %zu plugin(s)\n", names.size());
  CHECK(!names.empty());
  for (const auto &name : names) {
    std::printf("  %s\n", name.c_str());
    CHECK(!name.empty());
  }

  // Callbacks can be set and removed again.
  manager.replaceStatusChangeEventHandler([](const StatusChangedEvent &) {});
  manager.replaceNotifyEventHandler([](const NotifyEvent &) {});
  manager.removeStatusChangeEventHandler();
  manager.removeNotifyEventHandler();

  // An SDK failure reaches the caller as a C++ exception with a message.
  bool threw = false;
  try {
    manager.SetCurrentPulserReceiver("NO-SUCH-MODEL", "NO-SUCH-SERIAL", 0);
  } catch (const std::exception &e) {
    threw = true;
    std::printf("Expected error: %s\n", e.what());
    CHECK(std::strlen(e.what()) > 0);
  }
  CHECK(threw);
}

// Discovers a simulated pulser-receiver (the SDK's JSRDotNET_DiagLib plugin),
// selects it, changes a setting and waits for the status event. This is the
// job a real program does, with the simulator standing in for hardware.
void runSimulator(JSRSDKManager &manager) {
  const char *plugin = "JSRDotNET_DiagLib";
  std::atomic<int> gainEvents{0};
  manager.replaceStatusChangeEventHandler([&](const StatusChangedEvent &e) {
    if (std::strcmp(e.pulserProperty, "GainIndex") == 0) {
      ++gainEvents;
    }
  });

  manager.AddManagedPlugin(plugin);
  manager.AddPluginOpenOption(plugin, "SHOW_SIMULATOR_WINDOWS", "NO");
  manager.AddPluginOpenOption(plugin, "CONNECT_ON_OPEN", "YES");
  manager.SetDiscoveryEnable(true);

  size_t found = 0;
  for (int i = 0; i < 200 && found == 0; ++i) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    found = manager.GetPulserReceivers(nullptr, 0);
  }
  std::printf("Simulator: %zu pulser-receiver(s)\n", found);
  CHECK(found > 0);
  if (found == 0) {
    return;
  }

  std::vector<PulserReceiverID> ids(found);
  ids.resize(manager.GetPulserReceivers(ids.data(), ids.size()));
  manager.SetCurrentPulserReceiver(ids.front());
  CHECK(manager.getIsPulserReceiverSelected());
  CHECK(std::strcmp(manager.getId().InstrumentId.ModelName, "SIM") == 0);

  manager.setGainIndex(4);
  CHECK(manager.getGainIndex() == 4);
  for (int i = 0; i < 100 && gainEvents == 0; ++i) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  std::printf("Simulator: gain index %d, %d status event(s)\n",
              manager.getGainIndex(), gainEvents.load());
  CHECK(gainEvents > 0);
  manager.removeStatusChangeEventHandler();
}

} // namespace

int main() {
  JSRSDKManager *manager = CreateJSRSDKManager();
  if (manager == nullptr) {
    std::fprintf(stderr, "CreateJSRSDKManager returned null\n");
    return 1;
  }

  try {
    run(*manager);
    runSimulator(*manager);
  } catch (const std::exception &e) {
    std::fprintf(stderr, "Unexpected exception: %s\n", e.what());
    ++failures;
  }

  DestroyJSRSDKManager(manager);

  if (failures != 0) {
    std::fprintf(stderr, "%d check(s) failed\n", failures);
    return 1;
  }
  std::printf("OK\n");
  return 0;
}
