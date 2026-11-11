/*
 * Smoke test for the C API in jsr_sdk.h, built as C99 by every compiler.
 *
 * Needs no hardware: it starts the .NET runtime inside JSR-SDK.dll, loads the
 * plugins next to the program, reads strings, lists and structs, sets and
 * removes callbacks, and checks that failures come back as error codes with
 * a message. Then it discovers, selects and configures a simulated
 * pulser-receiver.
 */
#include "JSR-SDK/jsr_sdk.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

static int failures = 0;

#define CHECK(cond)                                                            \
  do {                                                                         \
    if (!(cond)) {                                                             \
      fprintf(stderr, "FAILED %s:%d: %s\n", __FILE__, __LINE__, #cond);        \
      ++failures;                                                              \
    }                                                                          \
  } while (0)

#define CHECK_OK(call)                                                         \
  do {                                                                         \
    int32_t rc_ = (call);                                                      \
    if (rc_ != JSR_OK) {                                                       \
      char message_[512];                                                      \
      JSR_GetLastErrorMessage(message_, sizeof message_);                      \
      fprintf(stderr, "FAILED %s:%d: %s returned %d: %s\n", __FILE__,          \
              __LINE__, #call, (int)rc_, message_);                            \
      ++failures;                                                              \
    }                                                                          \
  } while (0)

#define MAX_PLUGINS 16

static void JSR_CALL onStatusChange(const JSR_StatusChangedEvent *event,
                                    void *user) {
  (void)event;
  (void)user;
}

static void JSR_CALL onNotify(const JSR_NotifyEvent *event, void *user) {
  (void)event;
  (void)user;
}

/* About 33 KB, so not on the stack. */
static JSR_LibMetadata metadata;

/* --- Simulated pulser-receiver ------------------------------------------- */

#define SIM_PLUGIN "JSRDotNET_DiagLib"

static int simUser;
static volatile int gainEvents = 0;
static volatile int wrongUser = 0;
static char lastGainValue[JSR_STRING_LENGTH];

static void JSR_CALL onSimStatus(const JSR_StatusChangedEvent *event,
                                 void *user) {
  if (user != &simUser) {
    ++wrongUser;
  }
  if (strcmp(event->pulserProperty, "GainIndex") == 0) {
    memcpy(lastGainValue, event->newValue, sizeof lastGainValue);
    ++gainEvents;
  }
}

/*
 * Discovers a pulser-receiver simulated by the SDK's diagnostics plugin,
 * selects it, configures it and receives the status event: the job of a real
 * program, with the simulator standing in for hardware. Passes structs,
 * doubles, enums and long argument lists across the DLL boundary.
 */
static void runSimulator(JSR_Manager *manager) {
  JSR_PulserReceiverID ids[8];
  char infoStorage[4][128];
  char *infoBuffers[4];
  double gains[512];
  size_t count = 0;
  size_t infoCount = 0;
  size_t gainCount = 0;
  size_t i;
  int32_t index = -1;
  int32_t mode = -1;
  double gain = 0.0;
  time_t deadline;

  CHECK_OK(JSR_AddManagedPlugin(manager, SIM_PLUGIN));
  CHECK_OK(JSR_AddPluginOpenOption(manager, SIM_PLUGIN,
                                   "SHOW_SIMULATOR_WINDOWS", "NO"));
  CHECK_OK(
      JSR_AddPluginOpenOption(manager, SIM_PLUGIN, "CONNECT_ON_OPEN", "YES"));
  CHECK_OK(JSR_SetStatusChangeCallback(manager, onSimStatus, &simUser));
  CHECK_OK(JSR_SetDiscoveryEnable(manager, 1));

  deadline = time(NULL) + 20;
  while (count == 0 && time(NULL) < deadline) {
    JSR_GetPulserReceivers(manager, NULL, 0, &count);
  }
  printf("Simulator: %u pulser-receiver(s)\n", (unsigned)count);
  CHECK(count > 0);
  if (count == 0) {
    return;
  }

  /* A struct list, then a struct passed by pointer. */
  CHECK_OK(JSR_GetPulserReceivers(manager, ids, 8, &count));
  CHECK(strcmp(ids[0].InstrumentId.ModelName, "SIM") == 0);
  CHECK(strcmp(ids[0].InstrumentId.PluginName, SIM_PLUGIN) == 0);
  CHECK_OK(JSR_SetCurrentPulserReceiverById(manager, &ids[0]));

  /* Eight arguments: on x86 this checks the calling convention. */
  for (i = 0; i < 4; ++i) {
    infoBuffers[i] = infoStorage[i];
  }
  CHECK_OK(JSR_GetPulserReceiverInfoByModel(
      manager, ids[0].InstrumentId.ModelName, ids[0].InstrumentId.SerialNum,
      ids[0].PulserReceiverIndex, infoBuffers, 4, 128, &infoCount));
  CHECK(infoCount > 0);

  /* Integers and doubles: the gain index selects a value from the list. */
  CHECK_OK(JSR_SetGainIndex(manager, 4));
  CHECK_OK(JSR_GetGainIndex(manager, &index));
  CHECK(index == 4);
  CHECK_OK(JSR_GetGainValues(manager, gains, 512, &gainCount));
  CHECK(gainCount > 4);
  CHECK_OK(JSR_GetGain(manager, &gain));
  CHECK(gainCount > 4 && gain == gains[4]);

  /* Enums. */
  CHECK_OK(JSR_SetReceiverMode(manager, JSR_RECEIVER_MODE_THRU));
  CHECK_OK(JSR_GetReceiverMode(manager, &mode));
  CHECK(mode == JSR_RECEIVER_MODE_THRU);

  /* The SDK raises status events inside the setter, on this thread. */
  printf("Simulator: gain index %d (%.2f dB), %d status event(s)\n", (int)index,
         gain, gainEvents);
  CHECK(gainEvents > 0);
  CHECK(strcmp(lastGainValue, "4") == 0);
  CHECK(wrongUser == 0);

  CHECK_OK(JSR_SetStatusChangeCallback(manager, NULL, NULL));
}

int main(void) {
  JSR_Manager *manager = NULL;
  char names[MAX_PLUGINS][JSR_STRING_LENGTH];
  char *buffers[MAX_PLUGINS];
  char message[512];
  char path[1024];
  char tiny[4];
  size_t count = 0;
  size_t required = 0;
  size_t i;
  int32_t loaded = 0;
  int32_t rc;

  /* Calls without a manager fail cleanly. */
  CHECK(JSR_GetPluginNames(NULL, NULL, 0, 0, &count) == JSR_ERROR_NULL_POINTER);
  JSR_GetLastErrorMessage(message, sizeof message);
  CHECK(strstr(message, "manager") != NULL);
  CHECK(JSR_Destroy(NULL) == JSR_OK);

  CHECK_OK(JSR_Create(&manager));
  if (manager == NULL) {
    fprintf(stderr, "JSR_Create gave no manager\n");
    return 1;
  }

  CHECK_OK(JSR_LoadPluginsFromBinaryDir(manager));
  CHECK_OK(JSR_GetPluginsLoaded(manager, &loaded));
  CHECK(loaded == 1);

  /* A list: query the count, then read. */
  CHECK_OK(JSR_GetPluginNames(manager, NULL, 0, 0, &count));
  CHECK(count > 0 && count <= MAX_PLUGINS);
  for (i = 0; i < MAX_PLUGINS; ++i) {
    buffers[i] = names[i];
    names[i][0] = '\0';
  }
  CHECK_OK(JSR_GetPluginNames(manager, buffers, MAX_PLUGINS, JSR_STRING_LENGTH,
                              &count));
  printf("Loaded %u plugin(s)\n", (unsigned)count);
  for (i = 0; i < count && i < MAX_PLUGINS; ++i) {
    printf("  %s\n", names[i]);
    CHECK(names[i][0] != '\0');
  }

  /* A struct result. */
  if (count > 0) {
    CHECK_OK(JSR_GetPluginLibraryMetadata(manager, names[0], &metadata));
    printf("Metadata of %s: name '%s', version '%s', %u model(s)\n", names[0],
           metadata.Name, metadata.Version,
           (unsigned)metadata.SupportedModelsCount);
    CHECK(metadata.Name[0] != '\0');
    CHECK(metadata.SupportedModelsCount <= JSR_MAX_ARRAY_SIZE);
    CHECK(metadata.ConnectionTypeCount <= JSR_MAX_ARRAY_SIZE);
    CHECK(metadata.OpenOptionsCount <= JSR_MAX_ARRAY_SIZE);
  }

  /* A string: query the size, then read. */
  CHECK_OK(JSR_GetPluginPath(manager, NULL, 0, &required));
  CHECK(required >= 1 && required <= sizeof path);
  CHECK_OK(JSR_GetPluginPath(manager, path, sizeof path, &required));
  CHECK(strlen(path) + 1 == required);
  printf("Plugin path: %s\n", path);

  /* Callbacks can be set and removed. */
  CHECK_OK(JSR_SetStatusChangeCallback(manager, onStatusChange, &failures));
  CHECK_OK(JSR_SetNotifyCallback(manager, onNotify, &failures));
  CHECK_OK(JSR_SetStatusChangeCallback(manager, NULL, NULL));
  CHECK_OK(JSR_SetNotifyCallback(manager, NULL, NULL));

  /* An SDK failure comes back as an error code and a message. */
  rc = JSR_SetCurrentPulserReceiverByModel(manager, "NO-SUCH-MODEL",
                                           "NO-SUCH-SERIAL", 0);
  CHECK(rc < 0);
  JSR_GetLastErrorMessage(message, sizeof message);
  printf("Expected error %d: %s\n", (int)rc, message);
  CHECK(strstr(message, "NO-SUCH-MODEL") != NULL);

  /* A NULL string argument is an error, not a crash. */
  CHECK(JSR_LoadPlugins(manager, NULL) == JSR_ERROR_NULL_POINTER);

  /* A short buffer is truncated, and the full size still comes back. */
  CHECK(JSR_GetLastErrorMessage(tiny, sizeof tiny) > sizeof tiny);
  CHECK(strlen(tiny) == sizeof tiny - 1);

  runSimulator(manager);

  CHECK_OK(JSR_Destroy(manager));

  if (failures != 0) {
    fprintf(stderr, "%d check(s) failed\n", failures);
    return 1;
  }
  printf("OK\n");
  return 0;
}
