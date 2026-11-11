// Smoke test for the C++ wrapper in jsr_sdk.hpp, built by every compiler.
//
// Needs no hardware: it loads the plugins, reads strings, lists and structs,
// sets and removes callbacks, checks that failures throw jsr::Error with a
// code and a message, and moves a manager.
#include "JSR-SDK/jsr_sdk.hpp"

#include <cstdio>
#include <cstring>
#include <exception>
#include <string>
#include <utility>

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

  const auto names = manager.GetPluginNames();
  std::printf("Loaded %zu plugin(s)\n", names.size());
  CHECK(!names.empty());
  for (const auto &name : names) {
    std::printf("  %s\n", name.c_str());
    CHECK(!name.empty());
  }

  if (!names.empty()) {
    const auto metadata = manager.GetPluginLibraryMetadata(names.front());
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
    manager.SetCurrentPulserReceiver("NO-SUCH-MODEL", "NO-SUCH-SERIAL", 0);
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
  CHECK(!moved.GetPluginNames().empty());
}

} // namespace

int main() {
  try {
    run();
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
