/*
 * dlls_elsewhere_check.c
 * The DLLs (JSR-SDK.dll, JSRDotNETSDK.dll and the plugins) sit in a folder of
 * their own, found through PATH, not next to this program. JSR-SDK.dll must
 * then still find its .NET assemblies next to itself, as cargo test and
 * other languages need. With 1.0.0 it couldn't: .NET only looked next to the
 * program, and JSR_Create failed.
 *
 * Selects a simulated pulser-receiver and changes a setting, so the SDK and
 * the plugins' own dependencies both have to load.
 */
#include "JSR-SDK/jsr_sdk.h"

#include <stdio.h>
#include <windows.h>

static const char *const kPlugin = "JSRDotNET_DiagLib";

static int fail(const char *what) {
  char message[1024];
  JSR_GetLastErrorMessage(message, sizeof message);
  fprintf(stderr, "FAILED: %s: %s\n", what, message);
  return 1;
}

int main(void) {
  JSR_Manager *manager = NULL;
  int attempt;

  if (JSR_Create(&manager) != JSR_OK) {
    return fail("JSR_Create");
  }
  if (JSR_LoadPluginsFromBinaryDir(manager) != JSR_OK ||
      JSR_AddManagedPlugin(manager, kPlugin) != JSR_OK ||
      JSR_AddPluginOpenOption(manager, kPlugin, "SHOW_SIMULATOR_WINDOWS",
                              "NO") != JSR_OK ||
      JSR_AddPluginOpenOption(manager, kPlugin, "CONNECT_ON_OPEN", "YES") !=
          JSR_OK ||
      JSR_SetDiscoveryEnable(manager, 1) != JSR_OK) {
    return fail("loading the simulator plugin");
  }
  for (attempt = 0; attempt < 200; ++attempt) {
    if (JSR_SetCurrentPulserReceiverByModel(manager, "SIM", "00", 0) ==
        JSR_OK) {
      break;
    }
    Sleep(100);
  }
  if (attempt == 200) {
    return fail("selecting the simulated pulser-receiver");
  }
  if (JSR_SetGainIndex(manager, 5) != JSR_OK) {
    return fail("JSR_SetGainIndex");
  }
  JSR_Destroy(manager);
  printf("OK: the DLLs work from a folder of their own\n");
  return 0;
}
