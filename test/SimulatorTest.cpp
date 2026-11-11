// SimulatorTest.cpp
// Tests for the C++ interface (JSRSDKManager.hpp) against the SDK's simulated
// pulser-receivers: discovery, selection, settings, events and errors. They
// need no hardware and run in CI.
#include "pch.h"
#include <gtest/gtest.h>

#include "JSR-SDK/JSRSDKManager.hpp"
#include "SdkError.hpp"
#include "SimulatorSupport.h"

#include <atomic>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

std::vector<std::string>
readStrings(const std::function<size_t(char **, size_t, size_t)> &read) {
  constexpr size_t kLength = 256;
  const size_t count = read(nullptr, 0, 0);
  std::vector<std::vector<char>> storage(count, std::vector<char>(kLength));
  std::vector<char *> buffers;
  for (auto &s : storage) {
    buffers.push_back(s.data());
  }
  read(buffers.data(), buffers.size(), kLength);
  return std::vector<std::string>(buffers.begin(), buffers.end());
}

std::string readString(const std::function<size_t(char *, size_t)> &read) {
  std::vector<char> buffer(read(nullptr, 0) + 1);
  read(buffer.data(), buffer.size());
  return buffer.data();
}

bool contains(const std::vector<std::string> &values, const std::string &v) {
  for (const auto &value : values) {
    if (value == v) {
      return true;
    }
  }
  return false;
}

/// Shares one manager with a simulated pulser-receiver across the suite:
/// discovery takes a few seconds.
class SimulatorTest : public ::testing::Test {
protected:
  static JSRSDKManager *manager;
  static PulserReceiverID device;
  static bool deviceFound;
  static sim::EventLog<StatusChangedEvent> *statusEvents;
  static sim::EventLog<NotifyEvent> *notifyEvents;

  static void SetUpTestCase() {
    statusEvents = new sim::EventLog<StatusChangedEvent>();
    notifyEvents = new sim::EventLog<NotifyEvent>();
    manager = CreateJSRSDKManager();
    manager->replaceStatusChangeEventHandler(
        [](const StatusChangedEvent &e) { statusEvents->add(e); });
    manager->replaceNotifyEventHandler(
        [](const NotifyEvent &e) { notifyEvents->add(e); });

    manager->loadPluginsFromBinaryDir();
    // Only manage the simulator, so discovery doesn't probe serial ports.
    manager->AddManagedPlugin(sim::kPlugin);
    manager->AddPluginOpenOption(sim::kPlugin, "SHOW_SIMULATOR_WINDOWS", "NO");
    manager->AddPluginOpenOption(sim::kPlugin, "CONNECT_ON_OPEN", "YES");
    manager->SetDiscoveryEnable(true);

    deviceFound = sim::waitFor([] {
      std::vector<PulserReceiverID> ids(
          manager->GetPulserReceivers(nullptr, 0));
      ids.resize(manager->GetPulserReceivers(ids.data(), ids.size()));
      for (const auto &id : ids) {
        if (std::strcmp(id.InstrumentId.SerialNum, sim::kSerial) == 0 &&
            id.PulserReceiverIndex == 0) {
          device = id;
          return true;
        }
      }
      return false;
    });
  }

  static void TearDownTestCase() {
    DestroyJSRSDKManager(manager);
    manager = nullptr;
    delete statusEvents;
    delete notifyEvents;
  }

  void SetUp() override {
    ASSERT_TRUE(deviceFound) << "Discovery found no simulated pulser-receiver";
    manager->SetCurrentPulserReceiver(device);
    statusEvents->clear();
  }
};

JSRSDKManager *SimulatorTest::manager = nullptr;
PulserReceiverID SimulatorTest::device;
bool SimulatorTest::deviceFound = false;
sim::EventLog<StatusChangedEvent> *SimulatorTest::statusEvents = nullptr;
sim::EventLog<NotifyEvent> *SimulatorTest::notifyEvents = nullptr;

} // namespace

//--------------------------------------------------------------------------
//  Discovery and selection
//--------------------------------------------------------------------------
TEST_F(SimulatorTest, Discovery_FindsOnlySimulatedChannels) {
  std::vector<PulserReceiverID> ids(manager->GetPulserReceivers(nullptr, 0));
  ids.resize(manager->GetPulserReceivers(ids.data(), ids.size()));
  ASSERT_GE(ids.size(), 2u);
  for (const auto &id : ids) {
    EXPECT_STREQ(id.InstrumentId.ModelName, sim::kModel);
    EXPECT_STREQ(id.InstrumentId.PluginName, sim::kPlugin);
  }
}

TEST_F(SimulatorTest, Discovery_RaisesNotifyEvents) {
  NotifyEvent discovered;
  EXPECT_TRUE(notifyEvents->waitFor(
      [](const NotifyEvent &e) {
        return e.notifyType == C_NOTIFY_TYPE::PULSER_RCVR_DISCOVERED;
      },
      &discovered));
  EXPECT_STREQ(discovered.model, sim::kModel);
  EXPECT_STREQ(discovered.pluginName, sim::kPlugin);
  EXPECT_TRUE(notifyEvents->waitFor([](const NotifyEvent &e) {
    return e.notifyType == C_NOTIFY_TYPE::DISCOVERY_COMPLETE;
  }));
}

TEST_F(SimulatorTest, Instruments_ListsTheSimulatedInstruments) {
  std::vector<InstrumentID> instruments(
      manager->GetInstruments(sim::kPlugin, nullptr, 0));
  instruments.resize(manager->GetInstruments(sim::kPlugin, instruments.data(),
                                             instruments.size()));
  ASSERT_FALSE(instruments.empty());
  for (const auto &instrument : instruments) {
    EXPECT_STREQ(instrument.ModelName, sim::kModel);
  }
}

TEST_F(SimulatorTest, SelectById_SetsCurrentDevice) {
  EXPECT_TRUE(manager->getIsPulserReceiverSelected());
  const PulserReceiverID id = manager->getId();
  EXPECT_STREQ(id.InstrumentId.SerialNum, device.InstrumentId.SerialNum);
  EXPECT_EQ(id.PulserReceiverIndex, device.PulserReceiverIndex);
}

TEST_F(SimulatorTest, SelectByModel_SetsCurrentDeviceAndNotifies) {
  notifyEvents->clear();
  manager->SetCurrentPulserReceiver(sim::kModel, sim::kSerial, 1);
  EXPECT_EQ(manager->getId().PulserReceiverIndex, 1);
  EXPECT_TRUE(notifyEvents->waitFor([](const NotifyEvent &e) {
    return e.notifyType == C_NOTIFY_TYPE::CURRENT_PULSER_RCVR_CHANGED;
  }));
}

TEST_F(SimulatorTest, PulserReceiverInfo_IsTheSameForModelAndId) {
  const auto byId = readStrings([&](char **b, size_t n, size_t len) {
    return manager->GetPulserReceiverInfo(device, b, n, len);
  });
  const auto byModel = readStrings([&](char **b, size_t n, size_t len) {
    return manager->GetPulserReceiverInfo(sim::kModel, sim::kSerial, 0, b, n,
                                          len);
  });
  EXPECT_FALSE(byId.empty());
  EXPECT_EQ(byId, byModel);
}

//--------------------------------------------------------------------------
//  Settings and status events
//--------------------------------------------------------------------------
TEST_F(SimulatorTest, GainIndex_RoundTripsAndRaisesStatusEvent) {
  ASSERT_TRUE(manager->getGainIndexSupported());
  const int original = manager->getGainIndex();
  const int target = original == 5 ? 6 : 5;

  manager->setGainIndex(target);
  EXPECT_EQ(manager->getGainIndex(), target);

  // The event carries the property, its new value and the device.
  StatusChangedEvent event;
  ASSERT_TRUE(statusEvents->waitFor(
      [](const StatusChangedEvent &e) {
        return std::strcmp(e.pulserProperty, "GainIndex") == 0;
      },
      &event));
  EXPECT_STREQ(event.newValue, std::to_string(target).c_str());
  EXPECT_EQ(event.changeType, C_STATUS_CHANGE::PROPERTY_CHANGE);
  EXPECT_STREQ(event.pulserReceiverId.InstrumentId.SerialNum, sim::kSerial);
  EXPECT_EQ(event.pulserReceiverId.PulserReceiverIndex, 0);

  manager->setGainIndex(original);
}

TEST_F(SimulatorTest, GainIndex_SelectsValueFromGainValues) {
  std::vector<double> values(manager->getGainValues(nullptr, 0));
  values.resize(manager->getGainValues(values.data(), values.size()));
  ASSERT_FALSE(values.empty());
  const int original = manager->getGainIndex();

  for (int index : {0, 3, static_cast<int>(values.size()) - 1}) {
    manager->setGainIndex(index);
    EXPECT_DOUBLE_EQ(manager->getGain(), values[index]) << "index " << index;
  }
  manager->setGainIndex(original);
}

TEST_F(SimulatorTest, ReplacingCallbackWhileEventsFire_NeverCallsOldOne) {
  // Each callback gets a probe. Once replace...() returns, the old callback
  // must neither be running nor be called again: it may capture things the
  // caller frees right after. The probes all stay alive, so a late call is
  // counted instead of touching freed memory.
  struct Probe {
    std::atomic<bool> retired{false};
  };
  std::vector<std::unique_ptr<Probe>> probes;
  std::atomic<int> calls{0};
  std::atomic<int> lateCalls{0};
  const int original = manager->getGainIndex();

  for (int i = 0; i < 200; ++i) {
    probes.push_back(std::make_unique<Probe>());
    Probe *probe = probes.back().get();
    manager->replaceStatusChangeEventHandler(
        [probe, &calls, &lateCalls](const StatusChangedEvent &) {
          ++calls;
          if (probe->retired) {
            ++lateCalls;
          }
          // Stay in the callback a little, as a real one might.
          std::this_thread::sleep_for(std::chrono::microseconds(200));
        });
    if (probes.size() > 1) {
      probes[probes.size() - 2]->retired = true;
    }
    manager->setGainIndex(i % 10);
  }
  manager->removeStatusChangeEventHandler();
  probes.back()->retired = true;
  std::this_thread::sleep_for(std::chrono::seconds(1));

  EXPECT_GT(calls.load(), 0);
  EXPECT_EQ(lateCalls.load(), 0);

  // Restore the suite's logging callback.
  manager->replaceStatusChangeEventHandler(
      [](const StatusChangedEvent &e) { statusEvents->add(e); });
  manager->setGainIndex(original);
}

TEST_F(SimulatorTest, ThrowingCallback_DoesNotStopEvents) {
  std::atomic<int> calls{0};
  manager->replaceStatusChangeEventHandler([&](const StatusChangedEvent &) {
    ++calls;
    throw std::runtime_error("callback failure");
  });
  const int original = manager->getGainIndex();
  manager->setGainIndex(original == 1 ? 2 : 1);
  manager->setGainIndex(original);
  EXPECT_TRUE(sim::waitFor([&] { return calls.load() >= 2; }));

  manager->replaceStatusChangeEventHandler(
      [](const StatusChangedEvent &e) { statusEvents->add(e); });
}

TEST_F(SimulatorTest, ReceiverMode_RoundTrips) {
  const C_RECEIVER_MODE original = manager->getReceiverMode();
  for (C_RECEIVER_MODE mode : {C_RECEIVER_MODE::THRU, C_RECEIVER_MODE::ECHO}) {
    manager->setReceiverMode(mode);
    EXPECT_EQ(manager->getReceiverMode(), mode);
  }
  manager->setReceiverMode(original);
}

TEST_F(SimulatorTest, SettingsArePerChannel) {
  const int original = manager->getGainIndex();
  manager->setGainIndex(7);
  manager->SetCurrentPulserReceiver(sim::kModel, sim::kSerial, 1);
  const int otherChannel = manager->getGainIndex();
  manager->SetCurrentPulserReceiver(device);
  EXPECT_EQ(manager->getGainIndex(), 7);
  EXPECT_NE(otherChannel, 7);
  manager->setGainIndex(original);
}

//--------------------------------------------------------------------------
//  Errors
//--------------------------------------------------------------------------
TEST_F(SimulatorTest, UnknownDevice_ThrowsSdkErrorNotFound) {
  try {
    manager->SetCurrentPulserReceiver("NO-SUCH-MODEL", "NO-SUCH-SERIAL", 0);
    FAIL() << "expected an exception";
  } catch (const jsr::detail::SdkError &e) {
    // The DLL's SdkError reaches the caller with the SDK's code.
    EXPECT_EQ(e.code(), C_ERROR_CODE::NOT_FOUND);
    EXPECT_NE(std::string(e.what()).find("NO-SUCH-MODEL"), std::string::npos);
  }
}

TEST_F(SimulatorTest, UnsupportedProperty_ThrowsRuntimeError) {
  // The simulator has no firmware version, not even its "supported" flag.
  EXPECT_THROW(manager->getPulserFirmwareVerSupported(), std::runtime_error);
  EXPECT_THROW(readString([&](char *b, size_t n) {
                 return manager->getPulserFirmwareVer(b, n);
               }),
               std::runtime_error);
}

//--------------------------------------------------------------------------
//  Open criteria
//--------------------------------------------------------------------------
TEST_F(SimulatorTest, OpenOption_AddReadRemove) {
  // Only options the plugin declares in its metadata are accepted.
  EXPECT_FALSE(manager->AddOpenOption(sim::kPlugin, "NO_SUCH_OPTION", "1"));

  ASSERT_TRUE(manager->AddOpenOption(sim::kPlugin, "NUM_PULSERS_PER_SIM", "2"));
  EXPECT_EQ(readString([&](char *b, size_t n) {
              return manager->GetOpenOption(sim::kPlugin, "NUM_PULSERS_PER_SIM",
                                            b, n);
            }),
            "2");
  const auto names = readStrings([&](char **b, size_t n, size_t len) {
    return manager->GetOpenOptionNames(sim::kPlugin, b, n, len);
  });
  EXPECT_TRUE(contains(names, "NUM_PULSERS_PER_SIM"));
  EXPECT_TRUE(contains(names, "CONNECT_ON_OPEN"));

  EXPECT_TRUE(manager->RemoveOpenOption(sim::kPlugin, "NUM_PULSERS_PER_SIM"));
  EXPECT_FALSE(contains(readStrings([&](char **b, size_t n, size_t len) {
                          return manager->GetOpenOptionNames(sim::kPlugin, b, n,
                                                             len);
                        }),
                        "NUM_PULSERS_PER_SIM"));
}

TEST_F(SimulatorTest, Ports_IncludeAndExcludeLists) {
  manager->AddPortToExclude(sim::kPlugin, "COM98");
  manager->AddPortToInclude(sim::kPlugin, "COM99");
  EXPECT_TRUE(contains(readStrings([&](char **b, size_t n, size_t len) {
                         return manager->GetPortsToExclude(sim::kPlugin, b, n,
                                                           len);
                       }),
                       "COM98"));
  EXPECT_TRUE(contains(readStrings([&](char **b, size_t n, size_t len) {
                         return manager->GetPortsToInclude(sim::kPlugin, b, n,
                                                           len);
                       }),
                       "COM99"));
}
