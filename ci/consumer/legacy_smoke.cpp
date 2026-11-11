// Smoke test for the C++ interface in JSRSDKManager.hpp.
//
// Needs no hardware: it starts the .NET runtime inside JSR-SDK.dll, loads the
// plugins that sit next to the program, checks that an SDK error reaches the
// caller as a C++ exception and shuts the manager down again.
#include "JSR-SDK/JSRSDKManager.hpp"

#include <cstdio>
#include <cstring>
#include <exception>
#include <string>
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

} // namespace

int main() {
  JSRSDKManager *manager = CreateJSRSDKManager();
  if (manager == nullptr) {
    std::fprintf(stderr, "CreateJSRSDKManager returned null\n");
    return 1;
  }

  try {
    run(*manager);
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
