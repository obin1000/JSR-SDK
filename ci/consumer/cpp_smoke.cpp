// Smoke test for the C++ wrapper in jsr_sdk.hpp, built by every compiler.
//
// Needs no hardware: it loads the plugins, reads strings, lists and structs,
// sets and removes callbacks, checks that failures throw jsr::Error with a
// code and a message, and moves a manager. Then it discovers, selects and
// configures a simulated pulser-receiver.
#include "JSR-SDK/jsr_sdk.hpp"

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <exception>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
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

void run() {
  jsr::Manager manager;
  manager.loadPluginsFromBinaryDir();
  CHECK(manager.getPluginsLoaded());

  const auto names = manager.getPluginNames();
  std::printf("Loaded %zu plugin(s)\n", names.size());
  CHECK(!names.empty());
  for (const auto &name : names) {
    std::printf("  %s\n", name.c_str());
    CHECK(!name.empty());
  }

  if (!names.empty()) {
    const auto metadata = manager.getPluginLibraryMetadata(names.front());
    std::printf("Metadata of %s: name '%s', version '%s'\n",
                names.front().c_str(), metadata->Name, metadata->Version);
    CHECK(metadata->Name[0] != '\0');
  }

  const std::string path = manager.getPluginPath();
  std::printf("Plugin path: %s\n", path.c_str());
  CHECK(!path.empty());

  // Callbacks can be set, replaced and removed.
  manager.replaceStatusChangeEventHandler(
      [](const jsr::StatusChangedEvent &) {});
  manager.replaceStatusChangeEventHandler(
      [](const jsr::StatusChangedEvent &) {});
  manager.replaceNotifyEventHandler([](const jsr::NotifyEvent &) {});
  manager.removeStatusChangeEventHandler();
  manager.removeNotifyEventHandler();

  // An SDK failure throws jsr::Error with a code and the SDK's message.
  bool threw = false;
  try {
    manager.setCurrentPulserReceiver("NO-SUCH-MODEL", "NO-SUCH-SERIAL", 0);
  } catch (const jsr::Error &e) {
    threw = true;
    std::printf("Expected error %d: %s\n", static_cast<int>(e.code()),
                e.what());
    CHECK(e.code() < 0);
    CHECK(std::strstr(e.what(), "NO-SUCH-MODEL") != nullptr);
  }
  CHECK(threw);

  // A moved-to manager keeps working; the moved-from one is empty.
  jsr::Manager moved = std::move(manager);
  CHECK(moved.handle() != nullptr);
  CHECK(!moved.getPluginNames().empty());
}

// Discovers a pulser-receiver simulated by the SDK's diagnostics plugin,
// selects it, configures it and receives status events: the job of a real
// program, with the simulator standing in for hardware.
void runSimulator() {
  const std::string plugin = "JSRDotNET_DiagLib";
  jsr::Manager manager;
  manager.loadPluginsFromBinaryDir();
  manager.addManagedPlugin(plugin);
  manager.addPluginOpenOption(plugin, "SHOW_SIMULATOR_WINDOWS", "NO");
  manager.addPluginOpenOption(plugin, "CONNECT_ON_OPEN", "YES");

  std::atomic<int> gainEvents{0};
  manager.replaceStatusChangeEventHandler(
      [&](const jsr::StatusChangedEvent &e) {
        if (std::strcmp(e.pulserProperty, "GainIndex") == 0) {
          ++gainEvents;
        }
        // Exceptions from callbacks are dropped; the setter must not fail.
        throw std::runtime_error("ignored");
      });

  manager.setDiscoveryEnable(true);
  std::vector<jsr::PulserReceiverID> ids;
  for (int i = 0; i < 200 && ids.empty(); ++i) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ids = manager.getPulserReceivers();
  }
  std::printf("Simulator: %zu pulser-receiver(s)\n", ids.size());
  CHECK(!ids.empty());
  if (ids.empty()) {
    return;
  }
  CHECK(std::strcmp(ids.front().InstrumentId.ModelName, "SIM") == 0);

  manager.setCurrentPulserReceiver(ids.front());
  CHECK(std::strcmp(manager.getId().InstrumentId.SerialNum,
                    ids.front().InstrumentId.SerialNum) == 0);
  CHECK(!manager
             .getPulserReceiverInfo(ids.front().InstrumentId.ModelName,
                                    ids.front().InstrumentId.SerialNum,
                                    ids.front().PulserReceiverIndex)
             .empty());

  const auto gains = manager.getGainValues();
  CHECK(gains.size() > 4);
  manager.setGainIndex(4);
  CHECK(manager.getGainIndex() == 4);
  CHECK(gains.size() > 4 && manager.getGain() == gains[4]);

  manager.setReceiverMode(JSR_RECEIVER_MODE_THRU);
  CHECK(manager.getReceiverMode() == JSR_RECEIVER_MODE_THRU);

  std::printf("Simulator: gain index %d (%.2f dB), %d status event(s)\n",
              manager.getGainIndex(), manager.getGain(), gainEvents.load());
  CHECK(gainEvents > 0);
}

} // namespace

int main() {
  try {
    run();
    runSimulator();
  } catch (const std::exception &e) {
    std::fprintf(stderr, "Unexpected exception: %s\n", e.what());
    ++failures;
  }

  if (failures != 0) {
    std::fprintf(stderr, "%d check(s) failed\n", failures);
    return 1;
  }
  std::printf("OK\n");
  return 0;
}
