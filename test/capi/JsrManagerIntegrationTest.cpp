// JsrManagerIntegrationTest.cpp
// Integration tests for jsr::Manager (jsr_sdk.hpp), the wrapper over the C
// API. Like JSRSDKManagerIntegrationTest, they need a physical
// pulser-receiver and fail without one. CI filters them out; run them by hand
// with a device attached.
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

    manager->SetDiscoveryEnable(true);
    for (int attempt = 0; attempt < 20; ++attempt) {
      for (const auto &id : manager->GetPulserReceivers()) {
        if (isTestDevice(id)) {
          device = id;
          manager->SetCurrentPulserReceiver(device);
          return;
        }
      }
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    FAIL() << "No real device connected - test requires physical device "
              "(JB0104, JB0118, or JC0100)";
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
