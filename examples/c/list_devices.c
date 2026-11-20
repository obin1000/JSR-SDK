/*
 * list_devices.c - lists the plugins JSR-SDK loads and the pulser-receivers
 * they find, with the C API in jsr_sdk.h.
 *
 * Shows the basics: checking the API version, creating a manager, reading
 * string and struct lists, and discovery. Every call returns an error code;
 * on failure, JSR_GetLastErrorMessage() says what went wrong.
 *
 * Exits 0 when no pulser-receiver is attached.
 */
#include "JSR-SDK/jsr_sdk.h"

#include <stdio.h>
#include <string.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h> /* Sleep() */

#define MAX_PLUGINS 32
#define MAX_DEVICES 32
#define DISCOVERY_SECONDS 20

/* The diagnostics plugin simulates pulser-receivers; see simulator.c. */
#define SIMULATOR_PLUGIN "JSRDotNET_DiagLib"

/* Returns 1 when rc is JSR_OK. Otherwise prints the call that failed and the
   SDK's message, and returns 0. */
static int succeeded(int32_t rc, const char *call) {
  char message[512];
  if (rc == JSR_OK) {
    return 1;
  }
  JSR_GetLastErrorMessage(message, sizeof message);
  fprintf(stderr, "%s failed with error %d: %s\n", call, (int)rc, message);
  return 0;
}

/* Runs a JSR_* call and jumps to the cleanup at `done` when it fails. */
#define CHECK(call)                                                            \
  do {                                                                         \
    if (!succeeded((call), #call)) {                                           \
      goto done;                                                               \
    }                                                                          \
  } while (0)

/* About 33 KB, so not on the stack. */
static JSR_LibMetadata metadata;

int main(void) {
  JSR_Manager *manager = NULL;
  char version[64];
  int32_t apiVersion = 0;
  /* A string list is read into buffers the caller owns. */
  char names[MAX_PLUGINS][JSR_STRING_LENGTH];
  char *nameBuffers[MAX_PLUGINS];
  size_t pluginCount = 0;
  JSR_PulserReceiverID devices[MAX_DEVICES];
  size_t deviceCount = 0;
  size_t lastCount = 0;
  int quietSeconds = 0;
  int second;
  size_t i;
  int exitCode = 1;

  /* The header and the loaded DLL must implement the same API version. */
  CHECK(JSR_GetApiVersion(&apiVersion));
  CHECK(JSR_GetLibraryVersion(version, sizeof version, NULL));
  printf("JSR-SDK %s, API version %d\n", version, (int)apiVersion);
  if (apiVersion != JSR_API_VERSION) {
    fprintf(stderr, "This program needs API version %d\n", JSR_API_VERSION);
    return 1;
  }

  /* The first manager starts the .NET runtime inside JSR-SDK.dll. */
  CHECK(JSR_Create(&manager));

  /* Loads the plugin DLLs that jsr_sdk_copy_runtime() put next to the
     program. */
  CHECK(JSR_LoadPluginsFromBinaryDir(manager));

  for (i = 0; i < MAX_PLUGINS; ++i) {
    nameBuffers[i] = names[i];
  }
  CHECK(JSR_GetPluginNames(manager, nameBuffers, MAX_PLUGINS, JSR_STRING_LENGTH,
                           &pluginCount));
  if (pluginCount > MAX_PLUGINS) {
    pluginCount = MAX_PLUGINS; /* Only the first MAX_PLUGINS were written. */
  }

  printf("%u plugin(s):\n", (unsigned)pluginCount);
  for (i = 0; i < pluginCount; ++i) {
    CHECK(JSR_GetPluginLibraryMetadata(manager, names[i], &metadata));
    printf("  %-28s %s %s\n", names[i], metadata.FriendlyName,
           metadata.Version);

    /* Discovery only searches managed plugins, and none are managed by
       default: without this, nothing is found. Leave the simulator out so
       only real hardware is listed. */
    if (strcmp(names[i], SIMULATOR_PLUGIN) != 0) {
      CHECK(JSR_AddManagedPlugin(manager, names[i]));
    }
  }

  /* Discovery runs in the background. Poll the list of pulser-receivers until
     some were found and no new ones appeared for 3 s, or until
     DISCOVERY_SECONDS have passed. */
  CHECK(JSR_SetDiscoveryEnable(manager, 1));
  printf("Searching for pulser-receivers for up to %d s...\n",
         DISCOVERY_SECONDS);
  for (second = 0; second < DISCOVERY_SECONDS && quietSeconds < 3; ++second) {
    Sleep(1000);
    /* A NULL buffer only asks for the count. */
    CHECK(JSR_GetPulserReceivers(manager, NULL, 0, &deviceCount));
    quietSeconds =
        deviceCount > 0 && deviceCount == lastCount ? quietSeconds + 1 : 0;
    lastCount = deviceCount;
  }

  CHECK(JSR_GetPulserReceivers(manager, devices, MAX_DEVICES, &deviceCount));
  if (deviceCount > MAX_DEVICES) {
    deviceCount = MAX_DEVICES;
  }

  if (deviceCount == 0) {
    printf("No pulser-receivers found. Is one connected and powered on?\n");
  } else {
    printf("%u pulser-receiver(s):\n", (unsigned)deviceCount);
  }
  for (i = 0; i < deviceCount; ++i) {
    const JSR_InstrumentID *id = &devices[i].InstrumentId;
    printf("  %s, serial %s, channel %d, port %s, plugin %s\n", id->ModelName,
           id->SerialNum, (int)devices[i].PulserReceiverIndex, id->Port,
           id->PluginName);
  }
  exitCode = 0;

done:
  /* JSR_Destroy() accepts NULL, so it is safe after any failure above. */
  if (!succeeded(JSR_Destroy(manager), "JSR_Destroy(manager)")) {
    exitCode = 1;
  }
  return exitCode;
}
