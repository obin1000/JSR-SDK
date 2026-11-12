// Built through find_package(JSR-SDK): checks that the target links, the
// headers are found and the DLLs were copied next to the program.
#include "JSR-SDK/jsr_sdk.hpp"

#include <iostream>

int main() {
  try {
    jsr::Manager manager;
    manager.loadPluginsFromBinaryDir();
    std::cout << "JSR-SDK " << jsr::libraryVersion() << ", API version "
              << jsr::apiVersion() << ", "
              << manager.GetPluginNames().size() << " plugins\n";
    return manager.GetPluginNames().empty() ? 1 : 0;
  } catch (const jsr::Error &e) {
    std::cerr << "JSR-SDK error " << e.code() << ": " << e.what() << "\n";
    return 1;
  }
}
