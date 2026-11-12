// CApiSimulatorTest.cpp
// Tests for the C API callbacks and the jsr::Manager wrapper against the
// SDK's simulated pulser-receivers: events arrive with their data, the user
// pointer comes back, callbacks can be replaced and removed, and a throwing
// C++ callback doesn't stop later events, while a crashing one ends the
// process. No hardware needed.
#include <gtest/gtest.h>

#include "CrashSupport.h"
#include "JSR-SDK/jsr_sdk.h"
#include "JSR-SDK/jsr_sdk.hpp"
#include "SimulatorSupport.h"

#include <atomic>
#include <chrono>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>

namespace {

std::string lastError() {
  std::vector<char> buffer(JSR_GetLastErrorMessage(nullptr, 0));
  JSR_GetLastErrorMessage(buffer.data(), buffer.size());
  return buffer.data();
}

/// Loads the simulator plugin, discovers and selects "SIM 00" channel 0.
bool selectSimulator(JSR_Manager *manager) {
  if (JSR_LoadPluginsFromBinaryDir(manager) != JSR_OK ||
      JSR_AddManagedPlugin(manager, sim::kPlugin) != JSR_OK ||
      JSR_AddPluginOpenOption(manager, sim::kPlugin, "SHOW_SIMULATOR_WINDOWS",
                              "NO") != JSR_OK ||
      JSR_AddPluginOpenOption(manager, sim::kPlugin, "CONNECT_ON_OPEN",
                              "YES") != JSR_OK ||
      JSR_SetDiscoveryEnable(manager, 1) != JSR_OK) {
    return false;
  }
  return sim::waitFor([&] {
    return JSR_SetCurrentPulserReceiverByModel(manager, sim::kModel,
                                               sim::kSerial, 0) == JSR_OK;
  });
}

// C callbacks: user points at the log the event goes to.
void JSR_CALL logStatus(const JSR_StatusChangedEvent *event, void *user) {
  static_cast<sim::EventLog<JSR_StatusChangedEvent> *>(user)->add(*event);
}

void JSR_CALL logNotify(const JSR_NotifyEvent *event, void *user) {
  static_cast<sim::EventLog<JSR_NotifyEvent> *>(user)->add(*event);
}

bool isGainIndexEvent(const JSR_StatusChangedEvent &e) {
  return std::strcmp(e.pulserProperty, "GainIndex") == 0;
}

class CApiSimulatorTest : public ::testing::Test {
protected:
  static JSR_Manager *manager;
  static bool ready;

  static void SetUpTestCase() {
    ready = JSR_Create(&manager) == JSR_OK && selectSimulator(manager);
  }

  static void TearDownTestCase() {
    JSR_Destroy(manager);
    manager = nullptr;
  }

  sim::EventLog<JSR_StatusChangedEvent> statusEvents;
  sim::EventLog<JSR_NotifyEvent> notifyEvents;

  void SetUp() override {
    ASSERT_TRUE(ready) << "Could not select a simulated pulser-receiver: "
                       << lastError();
    ASSERT_EQ(JSR_SetCurrentPulserReceiverByModel(manager, sim::kModel,
                                                  sim::kSerial, 0),
              JSR_OK);
    ASSERT_EQ(JSR_SetStatusChangeCallback(manager, logStatus, &statusEvents),
              JSR_OK);
    ASSERT_EQ(JSR_SetNotifyCallback(manager, logNotify, &notifyEvents), JSR_OK);
  }

  void TearDown() override {
    // The logs are members: remove the callbacks before they go away.
    JSR_SetStatusChangeCallback(manager, nullptr, nullptr);
    JSR_SetNotifyCallback(manager, nullptr, nullptr);
  }

  int32_t gainIndex() {
    int32_t value = -1;
    EXPECT_EQ(JSR_GetGainIndex(manager, &value), JSR_OK) << lastError();
    return value;
  }
};

JSR_Manager *CApiSimulatorTest::manager = nullptr;
bool CApiSimulatorTest::ready = false;

} // namespace

//--------------------------------------------------------------------------
//  C API callbacks
//--------------------------------------------------------------------------
TEST_F(CApiSimulatorTest, StatusEvent_CarriesPropertyValueAndDevice) {
  const int32_t original = gainIndex();
  const int32_t target = original == 5 ? 6 : 5;
  ASSERT_EQ(JSR_SetGainIndex(manager, target), JSR_OK) << lastError();

  JSR_StatusChangedEvent event{};
  ASSERT_TRUE(statusEvents.waitFor(isGainIndexEvent, &event));
  EXPECT_STREQ(event.newValue, std::to_string(target).c_str());
  EXPECT_EQ(event.changeType, JSR_STATUS_CHANGE_PROPERTY_CHANGE);
  EXPECT_EQ(event.errorCode, JSR_OK);
  EXPECT_STREQ(event.pulserReceiverId.InstrumentId.ModelName, sim::kModel);
  EXPECT_STREQ(event.pulserReceiverId.InstrumentId.SerialNum, sim::kSerial);
  EXPECT_STREQ(event.pulserReceiverId.InstrumentId.PluginName, sim::kPlugin);
  EXPECT_EQ(event.pulserReceiverId.PulserReceiverIndex, 0);

  JSR_SetGainIndex(manager, original);
}

TEST_F(CApiSimulatorTest, NotifyEvent_ReportsSelectionChange) {
  ASSERT_EQ(JSR_SetCurrentPulserReceiverByModel(manager, sim::kModel,
                                                sim::kSerial, 1),
            JSR_OK);
  JSR_NotifyEvent event{};
  ASSERT_TRUE(notifyEvents.waitFor(
      [](const JSR_NotifyEvent &e) {
        return e.notifyType == JSR_NOTIFY_TYPE_CURRENT_PULSER_RCVR_CHANGED;
      },
      &event));
  EXPECT_STREQ(event.model, sim::kModel);
  EXPECT_STREQ(event.serial, sim::kSerial);
  EXPECT_STREQ(event.pluginName, sim::kPlugin);
  EXPECT_STREQ(event.pulserReceiverId.InstrumentId.SerialNum, sim::kSerial);
  EXPECT_LE(event.infoCount, static_cast<size_t>(JSR_MAX_INFO_COUNT));
}

TEST_F(CApiSimulatorTest, NotifyEvent_ReportsPropertyChange) {
  const int32_t original = gainIndex();
  ASSERT_EQ(JSR_SetGainIndex(manager, original == 3 ? 4 : 3), JSR_OK);
  JSR_NotifyEvent event{};
  ASSERT_TRUE(notifyEvents.waitFor(
      [](const JSR_NotifyEvent &e) {
        return e.notifyType == JSR_NOTIFY_TYPE_PROPERTY_CHANGE &&
               std::strcmp(e.propertyName, "GainIndex") == 0;
      },
      &event));
  EXPECT_STREQ(event.model, sim::kModel);
  JSR_SetGainIndex(manager, original);
}

TEST_F(CApiSimulatorTest, ReplacedCallback_IsNoLongerCalled) {
  sim::EventLog<JSR_StatusChangedEvent> replacement;
  ASSERT_EQ(JSR_SetStatusChangeCallback(manager, logStatus, &replacement),
            JSR_OK);

  const int32_t original = gainIndex();
  ASSERT_EQ(JSR_SetGainIndex(manager, original == 2 ? 3 : 2), JSR_OK);
  EXPECT_TRUE(replacement.waitFor(isGainIndexEvent));
  EXPECT_TRUE(statusEvents.snapshot().empty());

  JSR_SetStatusChangeCallback(manager, nullptr, nullptr);
  JSR_SetGainIndex(manager, original);
}

TEST_F(CApiSimulatorTest, RemovedCallback_IsNoLongerCalled) {
  ASSERT_EQ(JSR_SetStatusChangeCallback(manager, nullptr, nullptr), JSR_OK);
  const int32_t original = gainIndex();
  ASSERT_EQ(JSR_SetGainIndex(manager, original == 2 ? 3 : 2), JSR_OK);
  JSR_SetGainIndex(manager, original);

  // Give stray events time to arrive; none may.
  EXPECT_FALSE(sim::waitFor([&] { return !statusEvents.snapshot().empty(); },
                            std::chrono::milliseconds(1500)));
}

namespace {

/// User data of one notify callback in the stress test below.
struct StressLog {
  static constexpr int kAlive = 0x5AFE;
  int canary = kAlive;
  std::atomic<int> *calls = nullptr;
  ~StressLog() { canary = 0; }
};

/// Calls that found their user data already freed.
std::atomic<int> callsOnFreedLog{0};

void JSR_CALL countNotify(const JSR_NotifyEvent *, void *user) {
  auto *log = static_cast<StressLog *>(user);
  // A call after the log was freed reads the canary its destructor cleared
  // (unless the memory was reused in the meantime). Count it, rather than
  // crash, so the test fails with a message about #14.
  if (log->canary != StressLog::kAlive) {
    ++callsOnFreedLog;
    return;
  }
  ++*log->calls;
}

} // namespace

TEST_F(CApiSimulatorTest, ReplacingNotifyCallbackDuringDiscovery_IsSafe) {
  // #14: once JSR_SetNotifyCallback() returns, the old callback is neither
  // running nor called again, so its user data may be freed at once.
  // Discovery raises notify events on library threads while this loop keeps
  // replacing the callback and freeing the previous user data immediately.
  std::atomic<int> calls{0};
  callsOnFreedLog = 0;
  std::unique_ptr<StressLog> current;
  int replacements = 0;
  const auto end = std::chrono::steady_clock::now() + std::chrono::seconds(4);
  auto nextDiscovery = std::chrono::steady_clock::now();

  while (std::chrono::steady_clock::now() < end) {
    auto next = std::make_unique<StressLog>();
    next->calls = &calls;
    ASSERT_EQ(JSR_SetNotifyCallback(manager, countNotify, next.get()), JSR_OK);
    current = std::move(next); // Frees the previous user data right away.
    ++replacements;

    // Restart discovery now and then, so events keep coming.
    if (std::chrono::steady_clock::now() >= nextDiscovery) {
      ASSERT_EQ(JSR_SetDiscoveryEnable(manager, 0), JSR_OK);
      ASSERT_EQ(JSR_SetDiscoveryEnable(manager, 1), JSR_OK);
      nextDiscovery =
          std::chrono::steady_clock::now() + std::chrono::milliseconds(300);
    }
  }
  ASSERT_EQ(JSR_SetNotifyCallback(manager, nullptr, nullptr), JSR_OK);
  current.reset();

  EXPECT_EQ(callsOnFreedLog.load(), 0)
      << "a replaced notify callback ran after JSR_SetNotifyCallback() "
         "returned (#14)";
  EXPECT_GT(replacements, 100);
  EXPECT_GT(calls.load(), 0) << "discovery raised no events during the test";
}

//--------------------------------------------------------------------------
//  Exceptions and crashes in callbacks (#36)
//--------------------------------------------------------------------------
namespace {

void JSR_CALL throwOnStatus(const JSR_StatusChangedEvent *, void *user) {
  ++*static_cast<std::atomic<int> *>(user);
  throw std::runtime_error("callback failure");
}

void JSR_CALL crashOnStatus(const JSR_StatusChangedEvent *, void *) {
  crash::accessViolation();
}

void JSR_CALL crashOnNotify(const JSR_NotifyEvent *, void *) {
  crash::accessViolation();
}

} // namespace

TEST_F(CApiSimulatorTest, ThrowingCallback_IsDroppedAndEventsContinue) {
  // A C++ exception from a callback is stopped: the call that raised the
  // event succeeds, and later events still arrive.
  std::atomic<int> calls{0};
  ASSERT_EQ(JSR_SetStatusChangeCallback(manager, throwOnStatus, &calls),
            JSR_OK);
  const int32_t original = gainIndex();
  EXPECT_EQ(JSR_SetGainIndex(manager, original == 5 ? 6 : 5), JSR_OK)
      << lastError();
  EXPECT_EQ(JSR_SetGainIndex(manager, original), JSR_OK) << lastError();
  EXPECT_TRUE(sim::waitFor([&] { return calls.load() >= 2; }));
  ASSERT_EQ(JSR_SetStatusChangeCallback(manager, nullptr, nullptr), JSR_OK);
}

#if GTEST_HAS_DEATH_TEST
// A crash is not an exception: after one, memory may be corrupt and a lock
// may be held, so it must end the process where it happened instead of
// being dropped (#36). Each death test runs in a child process; the .NET
// runtime reports the crash as an unhandled AccessViolationException.
using CApiSimulatorDeathTest = CApiSimulatorTest;

TEST_F(CApiSimulatorDeathTest, CrashingStatusCallback_EndsTheProcess) {
  EXPECT_DEATH(
      {
        crash::quietly();
        JSR_SetStatusChangeCallback(manager, crashOnStatus, nullptr);
        JSR_SetGainIndex(manager, 5);
        JSR_SetGainIndex(manager, 6);
        // The event may come from another thread: give it time.
        std::this_thread::sleep_for(std::chrono::seconds(10));
      },
      "AccessViolationException");
}

TEST_F(CApiSimulatorDeathTest, CrashingNotifyCallback_EndsTheProcess) {
  EXPECT_DEATH(
      {
        crash::quietly();
        JSR_SetNotifyCallback(manager, crashOnNotify, nullptr);
        // Restarting discovery raises notify events on library threads.
        JSR_SetDiscoveryEnable(manager, 0);
        JSR_SetDiscoveryEnable(manager, 1);
        std::this_thread::sleep_for(sim::kTimeout);
      },
      "AccessViolationException");
}
#endif

//--------------------------------------------------------------------------
//  Typed values through the C API
//--------------------------------------------------------------------------
TEST_F(CApiSimulatorTest, EnumSetting_RoundTrips) {
  int32_t original = -1;
  ASSERT_EQ(JSR_GetReceiverMode(manager, &original), JSR_OK);
  for (int32_t mode : {JSR_RECEIVER_MODE_THRU, JSR_RECEIVER_MODE_ECHO}) {
    ASSERT_EQ(JSR_SetReceiverMode(manager, mode), JSR_OK) << lastError();
    int32_t value = -1;
    ASSERT_EQ(JSR_GetReceiverMode(manager, &value), JSR_OK);
    EXPECT_EQ(value, mode);
  }
  JSR_SetReceiverMode(manager, original);
}

TEST_F(CApiSimulatorTest, InvalidEnumSetting_IsRejectedAndChangesNothing) {
  // #15: an invalid value used to switch the device to a different, valid
  // mode and report success.
  int32_t original = -1;
  ASSERT_EQ(JSR_GetReceiverMode(manager, &original), JSR_OK);
  for (int32_t invalid :
       {7, -1, static_cast<int32_t>(JSR_RECEIVER_MODE_LAST_VAL)}) {
    SCOPED_TRACE(invalid);
    EXPECT_EQ(JSR_SetReceiverMode(manager, invalid),
              JSR_ERROR_INVALID_ARGUMENT);
    EXPECT_NE(lastError().find("RECEIVER_MODE"), std::string::npos);
    int32_t value = -1;
    ASSERT_EQ(JSR_GetReceiverMode(manager, &value), JSR_OK);
    EXPECT_EQ(value, original);
  }
}

TEST_F(CApiSimulatorTest, PowerLimit_UnknownIsNotWithinLimit) {
  // The simulator doesn't report a power limit; that used to read as
  // WITHIN_LIMIT.
  int32_t value = -1;
  ASSERT_EQ(JSR_GetPulserPowerLimitStatus(manager, &value), JSR_OK);
  EXPECT_EQ(value, JSR_POWER_LIMIT_UNKNOWN);
}

TEST_F(CApiSimulatorTest, DoubleList_MatchesIndexedValue) {
  size_t count = 0;
  ASSERT_EQ(JSR_GetGainValues(manager, nullptr, 0, &count), JSR_OK);
  ASSERT_GT(count, 3u);
  std::vector<double> values(count);
  ASSERT_EQ(JSR_GetGainValues(manager, values.data(), values.size(), &count),
            JSR_OK);

  // A short buffer gets the first values and the full count.
  double first[2] = {};
  size_t available = 0;
  ASSERT_EQ(JSR_GetGainValues(manager, first, 2, &available), JSR_OK);
  EXPECT_EQ(available, count);
  EXPECT_DOUBLE_EQ(first[1], values[1]);

  const int32_t original = gainIndex();
  ASSERT_EQ(JSR_SetGainIndex(manager, 3), JSR_OK);
  double gain = 0.0;
  ASSERT_EQ(JSR_GetGain(manager, &gain), JSR_OK);
  EXPECT_DOUBLE_EQ(gain, values[3]);
  JSR_SetGainIndex(manager, original);
}

TEST_F(CApiSimulatorTest, SelectById_UsesTheStruct) {
  size_t count = 0;
  ASSERT_EQ(JSR_GetPulserReceivers(manager, nullptr, 0, &count), JSR_OK);
  std::vector<JSR_PulserReceiverID> ids(count);
  ASSERT_EQ(JSR_GetPulserReceivers(manager, ids.data(), ids.size(), &count),
            JSR_OK);
  ASSERT_GE(ids.size(), 2u);

  ASSERT_EQ(JSR_SetCurrentPulserReceiverById(manager, &ids[1]), JSR_OK)
      << lastError();
  JSR_PulserReceiverID current{};
  ASSERT_EQ(JSR_GetId(manager, &current), JSR_OK);
  EXPECT_STREQ(current.InstrumentId.SerialNum, ids[1].InstrumentId.SerialNum);
  EXPECT_EQ(current.PulserReceiverIndex, ids[1].PulserReceiverIndex);
}

TEST_F(CApiSimulatorTest, UnsupportedProperty_IsAnError) {
  char buffer[64];
  size_t required = 0;
  const int32_t rc =
      JSR_GetPulserFirmwareVer(manager, buffer, sizeof buffer, &required);
  EXPECT_LT(rc, 0);
  EXPECT_NE(lastError().find("PulserFirmwareVer"), std::string::npos);
}

//--------------------------------------------------------------------------
//  jsr::Manager
//--------------------------------------------------------------------------
TEST(JsrManagerSimulatorTest, DiscoversConfiguresAndReceivesEvents) {
  jsr::Manager manager;
  manager.loadPluginsFromBinaryDir();
  manager.AddManagedPlugin(sim::kPlugin);
  manager.AddPluginOpenOption(sim::kPlugin, "SHOW_SIMULATOR_WINDOWS", "NO");
  manager.AddPluginOpenOption(sim::kPlugin, "CONNECT_ON_OPEN", "YES");

  sim::EventLog<jsr::StatusChangedEvent> events;
  std::atomic<int> thrown{0};
  manager.replaceStatusChangeEventHandler(
      [&](const jsr::StatusChangedEvent &e) {
        events.add(e);
        // A throwing callback must not stop the events that follow.
        ++thrown;
        throw std::runtime_error("callback failure");
      });

  manager.SetDiscoveryEnable(true);
  ASSERT_TRUE(
      sim::waitFor([&] { return !manager.GetPulserReceivers().empty(); }));
  const auto ids = manager.GetPulserReceivers();
  for (const auto &id : ids) {
    EXPECT_STREQ(id.InstrumentId.ModelName, sim::kModel);
  }
  EXPECT_FALSE(manager.GetInstruments(sim::kPlugin).empty());

  manager.SetCurrentPulserReceiver(ids.front());
  EXPECT_STREQ(manager.getId().InstrumentId.SerialNum,
               ids.front().InstrumentId.SerialNum);

  manager.setReceiverMode(JSR_RECEIVER_MODE_THRU);
  EXPECT_EQ(manager.getReceiverMode(), JSR_RECEIVER_MODE_THRU);

  const auto gains = manager.getGainValues();
  ASSERT_GT(gains.size(), 6u);
  manager.setGainIndex(5);
  EXPECT_EQ(manager.getGainIndex(), 5);
  EXPECT_DOUBLE_EQ(manager.getGain(), gains[5]);
  manager.setGainIndex(6);

  EXPECT_TRUE(events.waitFor([](const jsr::StatusChangedEvent &e) {
    return std::strcmp(e.pulserProperty, "GainIndex") == 0 &&
           std::strcmp(e.newValue, "6") == 0;
  }));
  EXPECT_GE(thrown.load(), 2);

  manager.removeStatusChangeEventHandler();
}

TEST(JsrManagerSimulatorTest, UnsupportedProperty_ThrowsError) {
  jsr::Manager manager;
  manager.loadPluginsFromBinaryDir();
  manager.AddManagedPlugin(sim::kPlugin);
  manager.AddPluginOpenOption(sim::kPlugin, "SHOW_SIMULATOR_WINDOWS", "NO");
  manager.AddPluginOpenOption(sim::kPlugin, "CONNECT_ON_OPEN", "YES");
  manager.SetDiscoveryEnable(true);
  ASSERT_TRUE(
      sim::waitFor([&] { return !manager.GetPulserReceivers().empty(); }));
  manager.SetCurrentPulserReceiver(manager.GetPulserReceivers().front());

  try {
    manager.getPulserFirmwareVer();
    FAIL() << "expected jsr::Error";
  } catch (const jsr::Error &e) {
    EXPECT_LT(e.code(), 0);
    EXPECT_NE(std::string(e.what()).find("PulserFirmwareVer"),
              std::string::npos);
  }
}
