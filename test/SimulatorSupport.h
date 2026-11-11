// SimulatorSupport.h
// Helpers for tests that use the SDK's simulated pulser-receivers.
//
// The JSRDotNET_DiagLib plugin simulates instruments (model "SIM", serials
// "00" and "01", two pulser-receivers each), so these tests need no hardware
// and run in CI.
#pragma once

#include <chrono>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

namespace sim {

/// Name of the SDK plugin that simulates pulser-receivers.
constexpr const char *kPlugin = "JSRDotNET_DiagLib";
/// Model name of a simulated instrument.
constexpr const char *kModel = "SIM";
/// Serial number of the first simulated instrument.
constexpr const char *kSerial = "00";
/// How long discovery and events may take.
constexpr std::chrono::seconds kTimeout{20};

/// Polls until condition() is true or the timeout passes.
inline bool waitFor(const std::function<bool()> &condition,
                    std::chrono::milliseconds timeout = kTimeout) {
  const auto deadline = std::chrono::steady_clock::now() + timeout;
  while (!condition()) {
    if (std::chrono::steady_clock::now() > deadline) {
      return false;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
  return true;
}

/// Thread-safe log of events that arrive on library threads.
template <class Event> class EventLog {
public:
  void add(const Event &event) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_events.push_back(event);
  }

  void clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_events.clear();
  }

  std::vector<Event> snapshot() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_events;
  }

  /// Waits for an event that matches predicate and copies it to *found.
  bool waitFor(const std::function<bool(const Event &)> &predicate,
               Event *found = nullptr) const {
    return sim::waitFor([&] {
      std::lock_guard<std::mutex> lock(m_mutex);
      for (const auto &event : m_events) {
        if (predicate(event)) {
          if (found != nullptr) {
            *found = event;
          }
          return true;
        }
      }
      return false;
    });
  }

private:
  mutable std::mutex m_mutex;
  std::vector<Event> m_events;
};

} // namespace sim
