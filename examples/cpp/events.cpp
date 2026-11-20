// events.cpp - receives status and notify events with the C++ wrapper in
// jsr_sdk.hpp, and replaces and removes the callbacks.
//
// Uses a simulated pulser-receiver (see simulator.cpp), so it needs no
// hardware. What to keep in mind with callbacks:
// - They run on threads of the SDK, or inside your own call (a setter raises
//   its status event before it returns). State they share with the rest of
//   the program needs a mutex.
// - They must return quickly and must not throw: jsr::Manager drops the
//   exception, and the event with it.
// - Replacing or removing a callback waits until a running one returns, and
//   after that the old one is never called again. So never hold a lock that
//   a callback takes while you replace or remove callbacks.
//
// Exits non-zero when an expected event doesn't arrive.
#include "JSR-SDK/jsr_sdk.hpp"

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace {

const std::string kSimulatorPlugin = "JSRDotNET_DiagLib";
const std::string kSimulatorModel = "SIM";
const std::string kSimulatorSerial = "00";
constexpr auto kTimeout = std::chrono::seconds(20);

// The events the callbacks saw, as text such as "first: GainIndex = 4".
// Callbacks add to it on SDK threads while main() reads it, so every access
// holds the mutex.
class EventLog {
public:
  void add(const std::string &entry) {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::cout << "  [event] " << entry << std::endl;
    m_entries.push_back(entry);
    m_changed.notify_all();
  }

  // Waits up to kTimeout for an entry; returns whether it arrived.
  bool waitFor(const std::string &entry) {
    std::unique_lock<std::mutex> lock(m_mutex);
    return m_changed.wait_for(lock, kTimeout, [&] {
      return std::count(m_entries.begin(), m_entries.end(), entry) > 0;
    });
  }

  bool contains(const std::string &entry) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return std::count(m_entries.begin(), m_entries.end(), entry) > 0;
  }

  size_t size() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_entries.size();
  }

private:
  mutable std::mutex m_mutex;
  std::condition_variable m_changed;
  std::vector<std::string> m_entries;
};

// Throws when the log didn't get an entry in time.
void expect(EventLog &log, const std::string &entry) {
  if (!log.waitFor(entry)) {
    throw std::runtime_error("No event '" + entry + "'");
  }
}

void selectSimulator(jsr::Manager &manager) {
  manager.addManagedPlugin(kSimulatorPlugin);
  manager.addPluginOpenOption(kSimulatorPlugin, "SHOW_SIMULATOR_WINDOWS", "NO");
  manager.addPluginOpenOption(kSimulatorPlugin, "CONNECT_ON_OPEN", "YES");
  manager.setDiscoveryEnable(true);

  const auto deadline = std::chrono::steady_clock::now() + kTimeout;
  while (std::chrono::steady_clock::now() < deadline) {
    for (const jsr::PulserReceiverID &id : manager.getPulserReceivers()) {
      if (id.InstrumentId.ModelName == kSimulatorModel &&
          id.InstrumentId.SerialNum == kSimulatorSerial) {
        manager.setCurrentPulserReceiver(kSimulatorModel, kSimulatorSerial, 0);
        return;
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  throw std::runtime_error("Discovery did not find the simulator");
}

void run() {
  // Declared before the manager, so it outlives the callbacks that use it:
  // the manager's destructor removes them first.
  EventLog log;
  jsr::Manager manager;
  manager.loadPluginsFromBinaryDir();

  // Notify events report what the manager does: discovery, selection, and
  // property changes of the selected pulser-receiver.
  manager.replaceNotifyEventHandler([&log](const jsr::NotifyEvent &event) {
    if (event.notifyType == JSR_NOTIFY_TYPE_CURRENT_PULSER_RCVR_CHANGED) {
      log.add(std::string("notify: selected ") + event.model + " " +
              event.serial);
    }
  });

  // Status events report a property of a pulser-receiver that changed.
  manager.replaceStatusChangeEventHandler(
      [&log](const jsr::StatusChangedEvent &event) {
        log.add(std::string("first: ") + event.pulserProperty + " = " +
                event.newValue);
      });

  selectSimulator(manager);
  expect(log, "notify: selected SIM 00");

  const int original = manager.getGainIndex();
  const int first = original == 4 ? 5 : 4;
  const int second = first + 1;

  // The first status handler sees the change.
  manager.setGainIndex(first);
  expect(log, "first: GainIndex = " + std::to_string(first));

  // Replace it. Once replaceStatusChangeEventHandler() returns, the first
  // handler is never called again, so whatever it uses could be freed now.
  manager.replaceStatusChangeEventHandler(
      [&log](const jsr::StatusChangedEvent &event) {
        log.add(std::string("second: ") + event.pulserProperty + " = " +
                event.newValue);
      });
  manager.setGainIndex(second);
  expect(log, "second: GainIndex = " + std::to_string(second));
  if (log.contains("first: GainIndex = " + std::to_string(second))) {
    throw std::runtime_error("The replaced handler was still called");
  }

  // Remove both. No more events reach the log.
  manager.removeStatusChangeEventHandler();
  manager.removeNotifyEventHandler();
  const size_t before = log.size();
  manager.setGainIndex(original);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  if (log.size() != before) {
    throw std::runtime_error("A removed handler was still called");
  }
  std::cout << "No events after removing the handlers" << std::endl;
}

} // namespace

int main() {
  try {
    run();
    std::cout << "OK" << std::endl;
    return 0;
  } catch (const jsr::Error &e) {
    std::cerr << "JSR-SDK error " << e.code() << ": " << e.what() << "\n";
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
  return 1;
}
