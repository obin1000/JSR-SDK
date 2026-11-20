// simulator.cpp - drives a simulated pulser-receiver with the C++ wrapper in
// jsr_sdk.hpp, so it needs no hardware.
//
// The SDK's diagnostics plugin (JSRDotNET_DiagLib) simulates pulser-receivers.
// This program discovers and selects one, reads and changes its gain and
// receiver mode, and prints the status events the changes raise. Driving real
// hardware works the same way, with that hardware's plugin.
//
// Exits non-zero when anything fails.
#include "JSR-SDK/jsr_sdk.hpp"

#include <chrono>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace {

const std::string kSimulatorPlugin = "JSRDotNET_DiagLib";
const std::string kSimulatorModel = "SIM";
const std::string kSimulatorSerial = "00";
constexpr auto kTimeout = std::chrono::seconds(20);

const char *receiverModeName(JSR_ReceiverMode mode) {
  switch (mode) {
  case JSR_RECEIVER_MODE_ECHO:
    return "echo";
  case JSR_RECEIVER_MODE_THRU:
    return "through";
  case JSR_RECEIVER_MODE_BOTH:
    return "both";
  default:
    return "unknown";
  }
}

// Waits until discovery has found the simulated device, then selects its
// channel 0.
void selectSimulator(jsr::Manager &manager) {
  const auto deadline = std::chrono::steady_clock::now() + kTimeout;
  for (;;) {
    for (const jsr::PulserReceiverID &id : manager.getPulserReceivers()) {
      if (id.InstrumentId.ModelName == kSimulatorModel &&
          id.InstrumentId.SerialNum == kSimulatorSerial) {
        manager.setCurrentPulserReceiver(kSimulatorModel, kSimulatorSerial, 0);
        return;
      }
    }
    if (std::chrono::steady_clock::now() >= deadline) {
      throw std::runtime_error("Discovery did not find the simulator");
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

void run() {
  jsr::Manager manager;
  manager.loadPluginsFromBinaryDir();

  // Discovery only searches managed plugins. The open options keep the
  // simulator's windows hidden and connect to the device as soon as it is
  // opened.
  manager.addManagedPlugin(kSimulatorPlugin);
  manager.addPluginOpenOption(kSimulatorPlugin, "SHOW_SIMULATOR_WINDOWS", "NO");
  manager.addPluginOpenOption(kSimulatorPlugin, "CONNECT_ON_OPEN", "YES");

  // Called for every status change, possibly on a thread of the SDK: it only
  // prints. std::printf writes a line at once, so lines from two threads don't
  // interleave mid-line. See events.cpp for sharing state with a callback.
  manager.replaceStatusChangeEventHandler(
      [](const jsr::StatusChangedEvent &event) {
        std::printf("  [event] %s %s is now %s\n",
                    event.pulserReceiverId.InstrumentId.ModelName,
                    event.pulserProperty, event.newValue);
        std::fflush(stdout);
      });

  manager.setDiscoveryEnable(true);
  std::cout << "Waiting for the simulated pulser-receiver..." << std::endl;
  selectSimulator(manager);
  std::cout << "Selected " << kSimulatorModel << ", serial " << kSimulatorSerial
            << ", channel 0" << std::endl;

  // Gain: the index selects one of the gain values (in dB).
  const std::vector<double> gains = manager.getGainValues();
  std::cout << "Gain index " << manager.getGainIndex() << " of 0.."
            << manager.getGainIndexMax() << ": " << manager.getGain() << " dB ("
            << gains.size() << " gain values)" << std::endl;

  // The setter raises a status event, often before it returns.
  const int newIndex = manager.getGainIndex() == 4 ? 5 : 4;
  std::cout << "Setting the gain index to " << newIndex << std::endl;
  manager.setGainIndex(newIndex);
  std::cout << "Gain index " << manager.getGainIndex() << ": "
            << manager.getGain() << " dB" << std::endl;
  if (manager.getGainIndex() != newIndex) {
    throw std::runtime_error("The gain index did not change");
  }

  // Receiver mode: an enum.
  std::cout << "Receiver mode: " << receiverModeName(manager.getReceiverMode())
            << "\nSetting the receiver mode to through" << std::endl;
  manager.setReceiverMode(JSR_RECEIVER_MODE_THRU);
  std::cout << "Receiver mode: " << receiverModeName(manager.getReceiverMode())
            << std::endl;
  if (manager.getReceiverMode() != JSR_RECEIVER_MODE_THRU) {
    throw std::runtime_error("The receiver mode did not change");
  }
}

} // namespace

int main() {
  try {
    run();
    std::cout << "OK" << std::endl;
    return 0;
  } catch (const jsr::Error &e) {
    // code() is one of the JSR_ERROR_* values, what() the SDK's message.
    std::cerr << "JSR-SDK error " << e.code() << ": " << e.what() << "\n";
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
  return 1;
}
