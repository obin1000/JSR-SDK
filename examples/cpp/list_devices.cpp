// list_devices.cpp - lists the plugins JSR-SDK loads and the pulser-receivers
// they find, with the C++ wrapper in jsr_sdk.hpp.
//
// jsr::Manager owns the SDK's manager, returns std::string and std::vector,
// and throws jsr::Error when a call fails, so there are no error codes to
// check.
//
// Exits 0 when no pulser-receiver is attached.
#include "JSR-SDK/jsr_sdk.hpp"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

namespace {

// The diagnostics plugin simulates pulser-receivers; see simulator.cpp.
const std::string kSimulatorPlugin = "JSRDotNET_DiagLib";
constexpr int kDiscoverySeconds = 20;

void run() {
  // Throws JSR_ERROR_VERSION_MISMATCH when the DLL doesn't match the header.
  jsr::Manager manager;
  std::cout << "JSR-SDK " << jsr::libraryVersion() << ", API version "
            << jsr::apiVersion() << "\n";

  // Loads the plugin DLLs that jsr_sdk_copy_runtime() put next to the program.
  manager.loadPluginsFromBinaryDir();

  const std::vector<std::string> plugins = manager.getPluginNames();
  std::cout << plugins.size() << " plugin(s):\n";
  for (const std::string &plugin : plugins) {
    const auto metadata = manager.getPluginLibraryMetadata(plugin);
    std::cout << "  " << plugin << ": " << metadata->FriendlyName << " "
              << metadata->Version << "\n";

    // Discovery only searches managed plugins, and none are managed by
    // default: without this, nothing is found. Leave the simulator out so
    // only real hardware is listed.
    if (plugin != kSimulatorPlugin) {
      manager.addManagedPlugin(plugin);
    }
  }

  // Discovery runs in the background. Poll the list of pulser-receivers until
  // some were found and no new ones appeared for 3 s, or until
  // kDiscoverySeconds have passed.
  manager.setDiscoveryEnable(true);
  std::cout << "Searching for pulser-receivers for up to " << kDiscoverySeconds
            << " s...\n";
  std::vector<jsr::PulserReceiverID> devices;
  int quietSeconds = 0;
  for (int second = 0; second < kDiscoverySeconds && quietSeconds < 3;
       ++second) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    const auto found = manager.getPulserReceivers();
    quietSeconds =
        !found.empty() && found.size() == devices.size() ? quietSeconds + 1 : 0;
    devices = found;
  }

  if (devices.empty()) {
    std::cout << "No pulser-receivers found. Is one connected and powered "
                 "on?\n";
    return;
  }
  std::cout << devices.size() << " pulser-receiver(s):\n";
  for (const jsr::PulserReceiverID &device : devices) {
    const jsr::InstrumentID &id = device.InstrumentId;
    std::cout << "  " << id.ModelName << ", serial " << id.SerialNum
              << ", channel " << device.PulserReceiverIndex << ", port "
              << id.Port << ", plugin " << id.PluginName << "\n";
  }
  // The manager's destructor stops discovery and releases the SDK.
}

} // namespace

int main() {
  try {
    run();
    return 0;
  } catch (const jsr::Error &e) {
    // code() is one of the JSR_ERROR_* values, what() the SDK's message.
    std::cerr << "JSR-SDK error " << e.code() << ": " << e.what() << "\n";
    return 1;
  }
}
