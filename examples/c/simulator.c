/*
 * simulator.c - drives a simulated pulser-receiver with the C API in
 * jsr_sdk.h, so it needs no hardware.
 *
 * The SDK's diagnostics plugin (JSRDotNET_DiagLib) simulates pulser-receivers.
 * This program discovers and selects one, reads and changes its gain and
 * receiver mode, and prints the status events the changes raise. Driving real
 * hardware works the same way, with that hardware's plugin.
 *
 * Exits non-zero when anything fails.
 */
#include "JSR-SDK/jsr_sdk.h"

#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h> /* Sleep() */

#define SIMULATOR_PLUGIN "JSRDotNET_DiagLib"
#define SIMULATOR_MODEL "SIM"
#define SIMULATOR_SERIAL "00"
#define TIMEOUT_MS 20000
#define MAX_GAINS 512

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

/* Called for every status change. It may run on a thread of the SDK, so it
   only prints: callbacks must be thread-safe, return quickly and never
   longjmp out. `user` is the pointer given to JSR_SetStatusChangeCallback(). */
static void JSR_CALL onStatusChange(const JSR_StatusChangedEvent *event,
                                    void *user) {
  (void)user;
  printf("  [event] %s %s is now %s\n",
         event->pulserReceiverId.InstrumentId.ModelName, event->pulserProperty,
         event->newValue);
  fflush(stdout);
}

static const char *receiverModeName(int32_t mode) {
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

int main(void) {
  JSR_Manager *manager = NULL;
  double gains[MAX_GAINS];
  size_t gainCount = 0;
  int32_t gainIndex = -1;
  int32_t gainIndexMax = -1;
  int32_t newIndex;
  int32_t mode = -1;
  double gain = 0.0;
  int waitedMs;
  int32_t rc;
  int exitCode = 1;

  CHECK(JSR_Create(&manager));
  CHECK(JSR_LoadPluginsFromBinaryDir(manager));

  /* Discovery only searches managed plugins. The open options keep the
     simulator's windows hidden and connect to the device as soon as it is
     opened. */
  CHECK(JSR_AddManagedPlugin(manager, SIMULATOR_PLUGIN));
  CHECK(JSR_AddPluginOpenOption(manager, SIMULATOR_PLUGIN,
                                "SHOW_SIMULATOR_WINDOWS", "NO"));
  CHECK(JSR_AddPluginOpenOption(manager, SIMULATOR_PLUGIN, "CONNECT_ON_OPEN",
                                "YES"));
  CHECK(JSR_SetStatusChangeCallback(manager, onStatusChange, NULL));
  CHECK(JSR_SetDiscoveryEnable(manager, 1));

  /* Discovery runs in the background: retry selecting channel 0 of the
     simulated device until discovery has found it. */
  printf("Waiting for the simulated pulser-receiver...\n");
  for (waitedMs = 0;; waitedMs += 100) {
    rc = JSR_SetCurrentPulserReceiverByModel(manager, SIMULATOR_MODEL,
                                             SIMULATOR_SERIAL, 0);
    if (rc == JSR_OK) {
      break;
    }
    if (waitedMs >= TIMEOUT_MS) {
      succeeded(rc, "JSR_SetCurrentPulserReceiverByModel()");
      goto done;
    }
    Sleep(100);
  }
  printf("Selected %s, serial %s, channel 0\n", SIMULATOR_MODEL,
         SIMULATOR_SERIAL);

  /* Gain: the index selects one of the gain values (in dB). */
  CHECK(JSR_GetGainValues(manager, gains, MAX_GAINS, &gainCount));
  CHECK(JSR_GetGainIndexMax(manager, &gainIndexMax));
  CHECK(JSR_GetGainIndex(manager, &gainIndex));
  CHECK(JSR_GetGain(manager, &gain));
  printf("Gain index %d of 0..%d: %.1f dB (%u gain values)\n", (int)gainIndex,
         (int)gainIndexMax, gain, (unsigned)gainCount);

  /* The setter raises a status event, often before it returns. */
  newIndex = gainIndex == 4 ? 5 : 4;
  printf("Setting the gain index to %d\n", (int)newIndex);
  CHECK(JSR_SetGainIndex(manager, newIndex));
  CHECK(JSR_GetGainIndex(manager, &gainIndex));
  CHECK(JSR_GetGain(manager, &gain));
  printf("Gain index %d: %.1f dB\n", (int)gainIndex, gain);
  if (gainIndex != newIndex) {
    fprintf(stderr, "The gain index did not change\n");
    goto done;
  }

  /* Receiver mode: an enum, passed as int32_t. */
  CHECK(JSR_GetReceiverMode(manager, &mode));
  printf("Receiver mode: %s\n", receiverModeName(mode));
  printf("Setting the receiver mode to through\n");
  CHECK(JSR_SetReceiverMode(manager, JSR_RECEIVER_MODE_THRU));
  CHECK(JSR_GetReceiverMode(manager, &mode));
  printf("Receiver mode: %s\n", receiverModeName(mode));
  if (mode != JSR_RECEIVER_MODE_THRU) {
    fprintf(stderr, "The receiver mode did not change\n");
    goto done;
  }

  /* Remove the callback before what its user data points to goes away. Here
     it has none; JSR_Destroy() would remove it as well. */
  CHECK(JSR_SetStatusChangeCallback(manager, NULL, NULL));
  printf("OK\n");
  exitCode = 0;

done:
  /* JSR_Destroy() accepts NULL, so it is safe after any failure above. */
  if (!succeeded(JSR_Destroy(manager), "JSR_Destroy(manager)")) {
    exitCode = 1;
  }
  return exitCode;
}
