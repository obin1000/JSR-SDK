// JsrManagerIntegrationTest.cpp
// Integration tests for jsr::Manager (jsr_sdk.hpp), the wrapper over the C
// API. Like JSRSDKManagerIntegrationTest, they need a physical
// pulser-receiver and fail without one. CI filters them out; run them by hand
// with a device attached:
//   capi_tests --gtest_filter=*IntegrationTest.*
#include <gtest/gtest.h>

#include "JSR-SDK/jsr_sdk.hpp"

#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

// googletest 1.8 can't skip a test: report it and return instead.
#define SKIP_TEST(reason)                                                      \
  do {                                                                         \
    std::cout << "[  SKIPPED ] " << reason << std::endl;                       \
    return;                                                                    \
  } while (0)

namespace {

bool isTestDevice(const jsr::PulserReceiverID &id) {
  const std::string serial(id.InstrumentId.SerialNum);
  const std::string model(id.InstrumentId.ModelName);
  if (model == "SIM") {
    return false;
  }
  return serial == "JB0104" || // HB3-2 (Loaner)
         serial == "JB0118" || // HB3-1
         serial == "JC0100";   // HB4
}

const char *const kNoDevice = "No real device connected - test requires "
                              "physical device (JB0104, JB0118, or JC0100)";

/// Turns discovery on, waits up to 20 s for a test device and selects it.
bool selectTestDevice(jsr::Manager &manager, jsr::PulserReceiverID &device) {
  manager.SetDiscoveryEnable(true);
  for (int attempt = 0; attempt < 20; ++attempt) {
    for (const auto &id : manager.GetPulserReceivers()) {
      if (isTestDevice(id)) {
        device = id;
        manager.SetCurrentPulserReceiver(device);
        return true;
      }
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  return false;
}

class JsrManagerIntegrationTest : public ::testing::Test {
protected:
  std::unique_ptr<jsr::Manager> manager;
  jsr::PulserReceiverID device{};
  std::atomic<int> statusEvents{0};
  std::atomic<int> notifyEvents{0};

  void SetUp() override {
    manager = std::make_unique<jsr::Manager>();
    manager->loadPluginsFromBinaryDir();
    manager->replaceStatusChangeEventHandler(
        [this](const jsr::StatusChangedEvent &) { ++statusEvents; });
    manager->replaceNotifyEventHandler(
        [this](const jsr::NotifyEvent &) { ++notifyEvents; });

    if (!selectTestDevice(*manager, device)) {
      FAIL() << kNoDevice;
    }
  }

  void TearDown() override { manager.reset(); }
};

} // namespace

TEST_F(JsrManagerIntegrationTest, Discovery_RaisesNotifyEvents) {
  EXPECT_GT(notifyEvents.load(), 0);
}

TEST_F(JsrManagerIntegrationTest, CurrentDevice_IsSelected) {
  EXPECT_TRUE(manager->getIsPulserReceiverSelected());
  const auto id = manager->getId();
  EXPECT_STREQ(id.InstrumentId.SerialNum, device.InstrumentId.SerialNum);
  EXPECT_EQ(id.PulserReceiverIndex, device.PulserReceiverIndex);
}

TEST_F(JsrManagerIntegrationTest, Info_IsNotEmpty) {
  EXPECT_FALSE(manager->getInfo().empty());
  EXPECT_FALSE(manager->GetPulserReceiverInfo(device).empty());
}

TEST_F(JsrManagerIntegrationTest, GainIndex_RoundTrips) {
  if (!manager->getGainIndexSupported()) {
    SKIP_TEST("Gain index not supported");
  }
  const int original = manager->getGainIndex();
  const int target = original == 0 ? 1 : 0;
  manager->setGainIndex(target);
  EXPECT_EQ(manager->getGainIndex(), target);
  manager->setGainIndex(original);
  EXPECT_EQ(manager->getGainIndex(), original);
}

TEST_F(JsrManagerIntegrationTest, ReceiverMode_RoundTrips) {
  const JSR_ReceiverMode original = manager->getReceiverMode();
  const JSR_ReceiverMode target = original == JSR_RECEIVER_MODE_ECHO
                                      ? JSR_RECEIVER_MODE_THRU
                                      : JSR_RECEIVER_MODE_ECHO;
  if ((target == JSR_RECEIVER_MODE_THRU &&
       !manager->getReceiverModeThruSupported()) ||
      (target == JSR_RECEIVER_MODE_ECHO &&
       !manager->getReceiverModeEchoSupported())) {
    SKIP_TEST("Receiver mode not supported");
  }
  manager->setReceiverMode(target);
  EXPECT_EQ(manager->getReceiverMode(), target);
  manager->setReceiverMode(original);
}

TEST_F(JsrManagerIntegrationTest, PropertyChange_RaisesStatusEvent) {
  if (!manager->getGainIndexSupported()) {
    SKIP_TEST("Gain index not supported");
  }
  const int before = statusEvents.load();
  const int original = manager->getGainIndex();
  manager->setGainIndex(original == 0 ? 1 : 0);
  manager->setGainIndex(original);
  for (int i = 0; i < 50 && statusEvents.load() == before; ++i) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  EXPECT_GT(statusEvents.load(), before);
}

//--------------------------------------------------------------------------
//  #13: destroying a manager releases the device
//--------------------------------------------------------------------------
TEST(JsrManagerLifetimeIntegrationTest, RecreatedManager_ReopensDevice) {
  // Before #13 was fixed, destroying a manager never shut the .NET manager
  // down, so it kept the device open and the next manager couldn't open it.
  // Twice: create a manager, open the device, use it and destroy it.
  std::atomic<bool> destroyed{false};
  std::atomic<int> callsAfterDestroy{0};

  for (int round = 1; round <= 2; ++round) {
    SCOPED_TRACE("round " + std::to_string(round));
    destroyed = false;
    {
      jsr::Manager manager;
      manager.loadPluginsFromBinaryDir();
      manager.replaceStatusChangeEventHandler(
          [&](const jsr::StatusChangedEvent &) {
            if (destroyed) {
              ++callsAfterDestroy;
            }
          });
      manager.replaceNotifyEventHandler([&](const jsr::NotifyEvent &) {
        if (destroyed) {
          ++callsAfterDestroy;
        }
      });

      jsr::PulserReceiverID device{};
      ASSERT_TRUE(selectTestDevice(manager, device))
          << (round == 1 ? kNoDevice
                         : "The device wasn't found again after the first "
                           "manager was destroyed (#13)");
      std::cout << "Round " << round << ": opened "
                << device.InstrumentId.ModelName << " "
                << device.InstrumentId.SerialNum << std::endl;
      EXPECT_TRUE(manager.getIsPulserReceiverSelected());
      EXPECT_FALSE(manager.getInfo().empty());

      // Talk to the device, so it is really open.
      if (manager.getGainIndexSupported()) {
        const int original = manager.getGainIndex();
        const int target = original == 0 ? 1 : 0;
        manager.setGainIndex(target);
        EXPECT_EQ(manager.getGainIndex(), target);
        manager.setGainIndex(original);
      }
    } // Destroys the manager.
    destroyed = true;

    // Give a manager that wasn't shut down time to raise late events.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    EXPECT_EQ(callsAfterDestroy.load(), 0)
        << "a callback ran after its manager was destroyed (#13)";
  }
}

//--------------------------------------------------------------------------
//  #14: replacing callbacks while the device raises events
//--------------------------------------------------------------------------
namespace {

/// User data of one callback in the test below. Its destructor clears the
/// canary, so a call after the data was freed can be detected.
struct CallbackLog {
  static constexpr int kAlive = 0x5AFE;
  int canary = kAlive;
  ~CallbackLog() { canary = 0; }
};

std::atomic<int> callsOnFreedLog{0};
std::atomic<int> statusCalls{0};
std::atomic<int> notifyCalls{0};
std::atomic<int> callsFromOtherThreads{0};
std::thread::id testThread;

void recordCall(void *user, std::atomic<int> &calls) {
  const auto *log = static_cast<const CallbackLog *>(user);
  // Count a call on freed data rather than crash, so the test fails with a
  // message about #14.
  if (log->canary != CallbackLog::kAlive) {
    ++callsOnFreedLog;
    return;
  }
  ++calls;
  if (std::this_thread::get_id() != testThread) {
    ++callsFromOtherThreads;
  }
}

void JSR_CALL recordStatus(const JSR_StatusChangedEvent *, void *user) {
  recordCall(user, statusCalls);
}

void JSR_CALL recordNotify(const JSR_NotifyEvent *, void *user) {
  recordCall(user, notifyCalls);
}

} // namespace

TEST_F(JsrManagerIntegrationTest,
       ReplacingCallbacksWhileDeviceRaisesEvents_IsSafe) {
  // #14: once a callback is replaced or removed, it is neither running nor
  // called again, so its user data may be freed at once. A real device also
  // raises events on the SDK's threads, which the simulator can't show.
  // For 15 s this replaces or removes both callbacks in a loop, freeing the
  // old user data immediately, while changing the gain and restarting
  // discovery so the device keeps raising events. Unplugging and replugging
  // the device during the test adds more.
  JSR_Manager *const handle = manager->handle();
  callsOnFreedLog = 0;
  statusCalls = 0;
  notifyCalls = 0;
  callsFromOtherThreads = 0;
  testThread = std::this_thread::get_id();

  const bool gainSupported = manager->getGainIndexSupported();
  const int originalGain = gainSupported ? manager->getGainIndex() : 0;
  const int otherGain = originalGain == 0 ? 1 : 0;

  std::unique_ptr<CallbackLog> status;
  std::unique_ptr<CallbackLog> notify;
  int replacements = 0;
  int gainChanges = 0;
  const auto end = std::chrono::steady_clock::now() + std::chrono::seconds(15);
  auto nextDiscovery = std::chrono::steady_clock::now();
  auto nextGain = std::chrono::steady_clock::now();

  while (std::chrono::steady_clock::now() < end) {
    ++replacements;
    // Every fifth time, remove the callbacks instead of replacing them.
    std::unique_ptr<CallbackLog> nextStatus;
    std::unique_ptr<CallbackLog> nextNotify;
    if (replacements % 5 != 0) {
      nextStatus = std::make_unique<CallbackLog>();
      nextNotify = std::make_unique<CallbackLog>();
    }
    ASSERT_EQ(JSR_SetStatusChangeCallback(handle,
                                          nextStatus ? recordStatus : nullptr,
                                          nextStatus.get()),
              JSR_OK);
    ASSERT_EQ(JSR_SetNotifyCallback(handle, nextNotify ? recordNotify : nullptr,
                                    nextNotify.get()),
              JSR_OK);
    status = std::move(nextStatus); // Frees the previous user data right away.
    notify = std::move(nextNotify);

    // Change the gain every 20 ms, while a status callback is set.
    if (gainSupported && status &&
        std::chrono::steady_clock::now() >= nextGain &&
        manager->getIsPulserReceiverSelected()) {
      manager->setGainIndex(++gainChanges % 2 == 0 ? originalGain : otherGain);
      nextGain =
          std::chrono::steady_clock::now() + std::chrono::milliseconds(20);
    }
    if (std::chrono::steady_clock::now() >= nextDiscovery) {
      manager->SetDiscoveryEnable(false);
      manager->SetDiscoveryEnable(true);
      nextDiscovery =
          std::chrono::steady_clock::now() + std::chrono::milliseconds(500);
    }
  }
  ASSERT_EQ(JSR_SetStatusChangeCallback(handle, nullptr, nullptr), JSR_OK);
  ASSERT_EQ(JSR_SetNotifyCallback(handle, nullptr, nullptr), JSR_OK);
  status.reset();
  notify.reset();
  if (gainSupported && manager->getIsPulserReceiverSelected()) {
    manager->setGainIndex(originalGain);
  }

  std::cout << replacements << " replacements, " << gainChanges
            << " gain changes; " << statusCalls.load() << " status and "
            << notifyCalls.load() << " notify calls, "
            << callsFromOtherThreads.load() << " of them on other threads"
            << std::endl;
  EXPECT_EQ(callsOnFreedLog.load(), 0)
      << "a replaced or removed callback ran after the call that replaced it "
         "returned (#14)";
  EXPECT_GT(replacements, 100);
  if (gainChanges > 0) {
    EXPECT_GT(statusCalls.load(), 0)
        << "changing the gain raised no status events";
  }
  EXPECT_GT(statusCalls.load() + notifyCalls.load(), 0)
      << "the device raised no events during the test";
}
