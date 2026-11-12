// TestDevices.h
// The pulser-receivers the *IntegrationTest suites accept (#19).
//
// Set JSR_TEST_DEVICE_SERIALS to a comma-separated list of serial numbers
// to test with other devices, for example
//   set JSR_TEST_DEVICE_SERIALS=JB0104,JC0100
// Without it (or when it is empty), the lab's devices are accepted.
// Simulated devices (model "SIM") never are.
#pragma once

#include <cstdlib>
#include <string>
#include <vector>

namespace testdevices {

/// The lab's devices: JB0104 (HB3-2, loaner), JB0118 (HB3-1), JC0100 (HB4).
constexpr const char *kDefaultSerials = "JB0104,JB0118,JC0100";

/// The environment variable that overrides kDefaultSerials.
constexpr const char *kVariable = "JSR_TEST_DEVICE_SERIALS";

namespace detail {

inline std::string environment(const char *name) {
#ifdef _MSC_VER
  // MSVC deprecates getenv.
  char *value = nullptr;
  size_t length = 0;
  std::string result;
  if (_dupenv_s(&value, &length, name) == 0 && value != nullptr) {
    result = value;
  }
  std::free(value);
  return result;
#else
  const char *value = std::getenv(name);
  return value != nullptr ? value : "";
#endif
}

inline std::vector<std::string> split(const std::string &list) {
  std::vector<std::string> items;
  size_t start = 0;
  while (start <= list.size()) {
    size_t end = list.find(',', start);
    if (end == std::string::npos) {
      end = list.size();
    }
    std::string item = list.substr(start, end - start);
    const size_t first = item.find_first_not_of(" \t");
    const size_t last = item.find_last_not_of(" \t");
    if (first != std::string::npos) {
      items.push_back(item.substr(first, last - first + 1));
    }
    start = end + 1;
  }
  return items;
}

} // namespace detail

/// Serial numbers of the accepted devices.
inline std::vector<std::string> serials() {
  std::vector<std::string> list = detail::split(detail::environment(kVariable));
  return list.empty() ? detail::split(kDefaultSerials) : list;
}

/// The accepted serial numbers, for messages: "JB0104, JB0118, JC0100".
inline std::string describe() {
  std::string text;
  for (const std::string &serial : serials()) {
    text += (text.empty() ? "" : ", ") + serial;
  }
  return text;
}

/// Whether serial is one of the accepted serial numbers.
inline bool isAcceptedSerial(const std::string &serial) {
  for (const std::string &accepted : serials()) {
    if (accepted == serial) {
      return true;
    }
  }
  return false;
}

/// Whether a discovered device is one the tests may use.
inline bool accepts(const char *model, const char *serial) {
  return std::string(model) != "SIM" && isAcceptedSerial(serial);
}

/// The message for a test that found none of the accepted devices.
inline std::string noDeviceMessage() {
  return "No test device connected: the integration tests need one of " +
         describe() + ". Set " + kVariable +
         " to the serial numbers of your devices to use others.";
}

} // namespace testdevices
