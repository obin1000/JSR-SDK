// CApiParityTest.cpp
// Checks that every C API getter and setter in jsr_sdk.h behaves exactly like
// the C++ interface it wraps: same value, or same error code and message.
//
// Both interfaces drive their own manager with a simulated pulser-receiver
// selected (see SimulatorSupport.h), so the values are real device values and
// a C function wired to the wrong method shows up. No hardware needed.
//
// The tests below the marker are generated from the C API's method list, like
// the functions in src/jsr_sdk_c.cpp. Setters are called with the value the
// device already has, so the simulated devices stay unchanged.
#include "pch.h"
#include <gtest/gtest.h>

#include "JSR-SDK/JSRSDKManager.hpp"
#include "JSR-SDK/jsr_sdk.h"
#include "SdkError.hpp"
#include "SimulatorSupport.h"

#include <climits>
#include <cstdio>
#include <cstring>
#include <functional>
#include <ostream>
#include <string>
#include <type_traits>
#include <vector>

namespace {

/// What a call produced: a value as text, or an error.
struct Outcome {
  bool ok = false;
  std::string value;
  std::string error;
  int code = 0;
};

std::ostream &operator<<(std::ostream &os, const Outcome &o) {
  if (o.ok) {
    return os << "value '" << o.value << "'";
  }
  return os << "error " << o.code << " '" << o.error << "'";
}

void expectSame(const Outcome &cpp, const Outcome &c) {
  EXPECT_EQ(cpp.ok, c.ok) << "C++: " << cpp << "\n  C: " << c;
  if (cpp.ok && c.ok) {
    EXPECT_EQ(cpp.value, c.value);
  } else if (!cpp.ok && !c.ok) {
    EXPECT_EQ(cpp.error, c.error);
    EXPECT_EQ(cpp.code, c.code);
  }
}

// --- Values as text ---
std::string text(bool value) { return value ? "1" : "0"; }
std::string text(int value) { return std::to_string(value); }
std::string text(double value) {
  char buffer[64];
  std::snprintf(buffer, sizeof buffer, "%.17g", value);
  return buffer;
}
template <class E, std::enable_if_t<std::is_enum_v<E>, int> = 0>
std::string text(E value) {
  return std::to_string(static_cast<int>(value));
}
std::string text(int32_t value, bool isBool) {
  return isBool ? text(value != 0) : std::to_string(value);
}
std::string join(const std::vector<std::string> &values) {
  std::string out;
  for (const auto &v : values) {
    out += v + "|";
  }
  return out;
}

// --- C++ interface ---
template <class F> Outcome legacyOutcome(F call) {
  try {
    return Outcome{true, call(), "", 0};
  } catch (const jsr::detail::SdkError &e) {
    return Outcome{false, "", e.what(), static_cast<int>(e.code())};
  } catch (const std::exception &e) {
    return Outcome{false, "", e.what(), INT_MIN};
  }
}

std::string legacyString(const std::function<size_t(char *, size_t)> &read) {
  std::vector<char> buffer(read(nullptr, 0) + 1);
  read(buffer.data(), buffer.size());
  return buffer.data();
}

std::string
legacyStrings(const std::function<size_t(char **, size_t, size_t)> &read) {
  constexpr size_t kLength = 1024;
  const size_t count = read(nullptr, 0, 0);
  std::vector<char> storage(count * kLength + 1);
  std::vector<char *> buffers(count);
  for (size_t i = 0; i < count; ++i) {
    buffers[i] = storage.data() + i * kLength;
  }
  read(buffers.data(), count, kLength);
  return join(std::vector<std::string>(buffers.begin(), buffers.end()));
}

std::string legacyDoubles(const std::function<size_t(double *, size_t)> &read) {
  std::vector<double> values(read(nullptr, 0));
  values.resize(read(values.data(), values.size()));
  std::string out;
  for (double v : values) {
    out += text(v) + "|";
  }
  return out;
}

// --- C API ---
std::string lastError() {
  std::vector<char> buffer(JSR_GetLastErrorMessage(nullptr, 0));
  JSR_GetLastErrorMessage(buffer.data(), buffer.size());
  return buffer.data();
}

Outcome cOutcome(int32_t rc, const std::function<std::string()> &value) {
  if (rc != JSR_OK) {
    return Outcome{false, "", lastError(), rc};
  }
  return Outcome{true, value(), "", 0};
}

Outcome cString(const std::function<int32_t(char *, size_t, size_t *)> &read) {
  size_t required = 0;
  int32_t rc = read(nullptr, 0, &required);
  if (rc != JSR_OK) {
    return cOutcome(rc, nullptr);
  }
  std::vector<char> buffer(required + 1);
  rc = read(buffer.data(), buffer.size(), &required);
  return cOutcome(rc, [&] { return std::string(buffer.data()); });
}

Outcome cStrings(
    const std::function<int32_t(char **, size_t, size_t, size_t *)> &read) {
  constexpr size_t kLength = 1024;
  size_t count = 0;
  int32_t rc = read(nullptr, 0, 0, &count);
  if (rc != JSR_OK) {
    return cOutcome(rc, nullptr);
  }
  std::vector<char> storage(count * kLength + 1);
  std::vector<char *> buffers(count);
  for (size_t i = 0; i < count; ++i) {
    buffers[i] = storage.data() + i * kLength;
  }
  rc = read(buffers.data(), count, kLength, &count);
  return cOutcome(rc, [&] {
    return join(std::vector<std::string>(buffers.begin(), buffers.end()));
  });
}

Outcome
cDoubles(const std::function<int32_t(double *, size_t, size_t *)> &read) {
  size_t count = 0;
  int32_t rc = read(nullptr, 0, &count);
  if (rc != JSR_OK) {
    return cOutcome(rc, nullptr);
  }
  std::vector<double> values(count);
  rc = read(values.data(), values.size(), &count);
  return cOutcome(rc, [&] {
    std::string out;
    for (double v : values) {
      out += text(v) + "|";
    }
    return out;
  });
}

/// One manager per interface, each with simulated device "SIM 00" channel 0
/// selected. Shared across the suite because discovery takes seconds.
class CApiParityTest : public ::testing::Test {
protected:
  static JSRSDKManager *legacy;
  static JSR_Manager *cManager;
  static bool ready;

  static void SetUpTestCase() {
    legacy = CreateJSRSDKManager();
    legacy->loadPluginsFromBinaryDir();
    legacy->AddManagedPlugin(sim::kPlugin);
    legacy->AddPluginOpenOption(sim::kPlugin, "SHOW_SIMULATOR_WINDOWS", "NO");
    legacy->AddPluginOpenOption(sim::kPlugin, "CONNECT_ON_OPEN", "YES");
    legacy->SetDiscoveryEnable(true);

    if (JSR_Create(&cManager) != JSR_OK ||
        JSR_LoadPluginsFromBinaryDir(cManager) != JSR_OK ||
        JSR_AddManagedPlugin(cManager, sim::kPlugin) != JSR_OK ||
        JSR_AddPluginOpenOption(cManager, sim::kPlugin,
                                "SHOW_SIMULATOR_WINDOWS", "NO") != JSR_OK ||
        JSR_AddPluginOpenOption(cManager, sim::kPlugin, "CONNECT_ON_OPEN",
                                "YES") != JSR_OK ||
        JSR_SetDiscoveryEnable(cManager, 1) != JSR_OK) {
      return;
    }

    ready = sim::waitFor([] {
      size_t legacyCount = legacy->GetPulserReceivers(nullptr, 0);
      size_t cCount = 0;
      JSR_GetPulserReceivers(cManager, nullptr, 0, &cCount);
      return legacyCount > 0 && cCount > 0;
    });
    if (ready) {
      try {
        legacy->SetCurrentPulserReceiver(sim::kModel, sim::kSerial, 0);
      } catch (const std::exception &) {
        ready = false;
      }
      ready = ready && JSR_SetCurrentPulserReceiverByModel(
                           cManager, sim::kModel, sim::kSerial, 0) == JSR_OK;
    }
  }

  static void TearDownTestCase() {
    JSR_Destroy(cManager);
    DestroyJSRSDKManager(legacy);
    cManager = nullptr;
    legacy = nullptr;
  }

  void SetUp() override {
    ASSERT_TRUE(ready) << "Could not select a simulated pulser-receiver";
  }
};

JSRSDKManager *CApiParityTest::legacy = nullptr;
JSR_Manager *CApiParityTest::cManager = nullptr;
bool CApiParityTest::ready = false;

} // namespace

//--------------------------------------------------------------------------
//  Functions with arguments
//--------------------------------------------------------------------------
TEST_F(CApiParityTest, PropertyQueries) {
  for (const char *property : {"Gain", "GainIndex", "NoSuchProperty"}) {
    SCOPED_TRACE(property);
    int32_t value = 0;
    expectSame(
        legacyOutcome([&] {
          return text(legacy->GetPulserPropertyAttributes(property));
        }),
        cOutcome(JSR_GetPulserPropertyAttributes(cManager, property, &value),
                 [&] { return text(value, false); }));
    expectSame(legacyOutcome([&] {
                 return text(legacy->GetPulserPropertyUnits(property));
               }),
               cOutcome(JSR_GetPulserPropertyUnits(cManager, property, &value),
                        [&] { return text(value, false); }));
    expectSame(
        legacyOutcome(
            [&] { return text(legacy->IsPulserSettingSupported(property)); }),
        cOutcome(JSR_IsPulserSettingSupported(cManager, property, &value),
                 [&] { return text(value, true); }));
    for (bool useShort : {false, true}) {
      expectSame(legacyOutcome([&] {
                   return legacyString([&](char *b, size_t n) {
                     return legacy->GetPulserPropertyUnitsAsString(
                         property, useShort, b, n);
                   });
                 }),
                 cString([&](char *b, size_t n, size_t *r) {
                   return JSR_GetPulserPropertyUnitsAsString(
                       cManager, property, useShort ? 1 : 0, b, n, r);
                 }));
    }
  }
}

TEST_F(CApiParityTest, PulserReceiverInfo) {
  expectSame(legacyOutcome([&] {
               return legacyStrings([&](char **b, size_t n, size_t len) {
                 return legacy->GetPulserReceiverInfo(sim::kModel, sim::kSerial,
                                                      1, b, n, len);
               });
             }),
             cStrings([&](char **b, size_t n, size_t len, size_t *c) {
               return JSR_GetPulserReceiverInfoByModel(
                   cManager, sim::kModel, sim::kSerial, 1, b, n, len, c);
             }));

  JSR_PulserReceiverID cId{};
  ASSERT_EQ(JSR_GetId(cManager, &cId), JSR_OK);
  const PulserReceiverID legacyId = legacy->getId();
  EXPECT_STREQ(cId.InstrumentId.SerialNum, legacyId.InstrumentId.SerialNum);
  EXPECT_STREQ(cId.InstrumentId.ModelName, legacyId.InstrumentId.ModelName);
  EXPECT_STREQ(cId.InstrumentId.Port, legacyId.InstrumentId.Port);
  EXPECT_STREQ(cId.InstrumentId.PluginName, legacyId.InstrumentId.PluginName);
  EXPECT_EQ(cId.PulserReceiverIndex, legacyId.PulserReceiverIndex);

  expectSame(legacyOutcome([&] {
               return legacyStrings([&](char **b, size_t n, size_t len) {
                 return legacy->GetPulserReceiverInfo(legacyId, b, n, len);
               });
             }),
             cStrings([&](char **b, size_t n, size_t len, size_t *c) {
               return JSR_GetPulserReceiverInfoById(cManager, &cId, b, n, len,
                                                    c);
             }));
}

TEST_F(CApiParityTest, PulserReceiversAndInstruments) {
  std::vector<PulserReceiverID> legacyIds(
      legacy->GetPulserReceivers(nullptr, 0));
  legacyIds.resize(
      legacy->GetPulserReceivers(legacyIds.data(), legacyIds.size()));
  size_t count = 0;
  ASSERT_EQ(JSR_GetPulserReceivers(cManager, nullptr, 0, &count), JSR_OK);
  std::vector<JSR_PulserReceiverID> cIds(count);
  ASSERT_EQ(JSR_GetPulserReceivers(cManager, cIds.data(), cIds.size(), &count),
            JSR_OK);
  ASSERT_EQ(cIds.size(), legacyIds.size());
  for (size_t i = 0; i < cIds.size(); ++i) {
    EXPECT_STREQ(cIds[i].InstrumentId.SerialNum,
                 legacyIds[i].InstrumentId.SerialNum);
    EXPECT_EQ(cIds[i].PulserReceiverIndex, legacyIds[i].PulserReceiverIndex);
  }

  std::vector<InstrumentID> legacyInstruments(
      legacy->GetInstruments(sim::kPlugin, nullptr, 0));
  legacyInstruments.resize(legacy->GetInstruments(
      sim::kPlugin, legacyInstruments.data(), legacyInstruments.size()));
  ASSERT_EQ(JSR_GetInstruments(cManager, sim::kPlugin, nullptr, 0, &count),
            JSR_OK);
  std::vector<JSR_InstrumentID> cInstruments(count);
  ASSERT_EQ(JSR_GetInstruments(cManager, sim::kPlugin, cInstruments.data(),
                               cInstruments.size(), &count),
            JSR_OK);
  ASSERT_EQ(cInstruments.size(), legacyInstruments.size());
  for (size_t i = 0; i < cInstruments.size(); ++i) {
    EXPECT_STREQ(cInstruments[i].SerialNum, legacyInstruments[i].SerialNum);
    EXPECT_STREQ(cInstruments[i].ModelName, legacyInstruments[i].ModelName);
  }
}

TEST_F(CApiParityTest, OpenCriteria) {
  expectSame(legacyOutcome([&] {
               return legacyStrings([&](char **b, size_t n, size_t len) {
                 return legacy->GetOpenOptionNames(sim::kPlugin, b, n, len);
               });
             }),
             cStrings([&](char **b, size_t n, size_t len, size_t *c) {
               return JSR_GetOpenOptionNames(cManager, sim::kPlugin, b, n, len,
                                             c);
             }));
  expectSame(legacyOutcome([&] {
               return legacyString([&](char *b, size_t n) {
                 return legacy->GetOpenOption(sim::kPlugin, "CONNECT_ON_OPEN",
                                              b, n);
               });
             }),
             cString([&](char *b, size_t n, size_t *r) {
               return JSR_GetOpenOption(cManager, sim::kPlugin,
                                        "CONNECT_ON_OPEN", b, n, r);
             }));
  for (const char *option : {"NUM_PULSERS_PER_SIM", "NO_SUCH_OPTION"}) {
    SCOPED_TRACE(option);
    int32_t added = 0;
    expectSame(
        legacyOutcome([&] {
          return text(legacy->AddOpenOption(sim::kPlugin, option, "2"));
        }),
        cOutcome(JSR_AddOpenOption(cManager, sim::kPlugin, option, "2", &added),
                 [&] { return text(added, true); }));
    int32_t removed = 0;
    expectSame(
        legacyOutcome([&] {
          return text(legacy->RemoveOpenOption(sim::kPlugin, option));
        }),
        cOutcome(JSR_RemoveOpenOption(cManager, sim::kPlugin, option, &removed),
                 [&] { return text(removed, true); }));
  }
}

//--------------------------------------------------------------------------
//  Generated: every getter without arguments, and every setter with the
//  device's current value
//--------------------------------------------------------------------------
TEST_F(CApiParityTest, GetCustomSettings) {
  expectSame(legacyOutcome([&] {
               return legacyStrings([&](char **b, size_t n, size_t len) {
                 return legacy->GetCustomSettings(b, n, len);
               });
             }),
             cStrings([&](char **b, size_t n, size_t len, size_t *c) {
               return JSR_GetCustomSettings(cManager, b, n, len, c);
             }));
}

TEST_F(CApiParityTest, GetManagedPluginNames) {
  expectSame(legacyOutcome([&] {
               return legacyStrings([&](char **b, size_t n, size_t len) {
                 return legacy->GetManagedPluginNames(b, n, len);
               });
             }),
             cStrings([&](char **b, size_t n, size_t len, size_t *c) {
               return JSR_GetManagedPluginNames(cManager, b, n, len, c);
             }));
}

TEST_F(CApiParityTest, GetPluginNames) {
  expectSame(legacyOutcome([&] {
               return legacyStrings([&](char **b, size_t n, size_t len) {
                 return legacy->GetPluginNames(b, n, len);
               });
             }),
             cStrings([&](char **b, size_t n, size_t len, size_t *c) {
               return JSR_GetPluginNames(cManager, b, n, len, c);
             }));
}

TEST_F(CApiParityTest, GetPluginsLoaded) {
  int32_t value = 0;
  const int32_t rc = JSR_GetPluginsLoaded(cManager, &value);
  expectSame(legacyOutcome([&] { return text(legacy->getPluginsLoaded()); }),
             cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetArePluginsAvailable) {
  int32_t value = 0;
  const int32_t rc = JSR_GetArePluginsAvailable(cManager, &value);
  expectSame(
      legacyOutcome([&] { return text(legacy->getArePluginsAvailable()); }),
      cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetManagerState) {
  int32_t value = 0;
  const int32_t rc = JSR_GetManagerState(cManager, &value);
  expectSame(legacyOutcome([&] { return text(legacy->getManagerState()); }),
             cOutcome(rc, [&] { return text(value, false); }));
}

TEST_F(CApiParityTest, GetPluginPath) {
  expectSame(legacyOutcome([&] {
               return legacyString([&](char *b, size_t n) {
                 return legacy->getPluginPath(b, n);
               });
             }),
             cString([&](char *b, size_t n, size_t *r) {
               return JSR_GetPluginPath(cManager, b, n, r);
             }));
}

TEST_F(CApiParityTest, SetPluginPath_WithCurrentValue) {
  std::string v = std::string();
  try {
    v = legacyString(
        [&](char *b, size_t n) { return legacy->getPluginPath(b, n); });
  } catch (const std::exception &) {
    // Unsupported here: both interfaces must still fail alike.
  }
  expectSame(legacyOutcome([&] {
               legacy->setPluginPath(v.c_str());
               return std::string();
             }),
             cOutcome(JSR_SetPluginPath(cManager, v.c_str()),
                      [] { return std::string(); }));
}

TEST_F(CApiParityTest, GetIsPulserReceiverSelected) {
  int32_t value = 0;
  const int32_t rc = JSR_GetIsPulserReceiverSelected(cManager, &value);
  expectSame(legacyOutcome(
                 [&] { return text(legacy->getIsPulserReceiverSelected()); }),
             cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetLastExceptionContextMessage) {
  expectSame(legacyOutcome([&] {
               return legacyString([&](char *b, size_t n) {
                 return legacy->getLastExceptionContextMessage(b, n);
               });
             }),
             cString([&](char *b, size_t n, size_t *r) {
               return JSR_GetLastExceptionContextMessage(cManager, b, n, r);
             }));
}

TEST_F(CApiParityTest, GetLastExceptionOrNull) {
  expectSame(legacyOutcome([&] {
               return legacyString([&](char *b, size_t n) {
                 return legacy->getLastExceptionOrNull(b, n);
               });
             }),
             cString([&](char *b, size_t n, size_t *r) {
               return JSR_GetLastExceptionOrNull(cManager, b, n, r);
             }));
}

TEST_F(CApiParityTest, GetInfo) {
  expectSame(legacyOutcome([&] {
               return legacyStrings([&](char **b, size_t n, size_t len) {
                 return legacy->getInfo(b, n, len);
               });
             }),
             cStrings([&](char **b, size_t n, size_t len, size_t *c) {
               return JSR_GetInfo(cManager, b, n, len, c);
             }));
}

TEST_F(CApiParityTest, GetPulseRepetitionFrequencyIndexSupported) {
  int32_t value = 0;
  const int32_t rc =
      JSR_GetPulseRepetitionFrequencyIndexSupported(cManager, &value);
  expectSame(legacyOutcome([&] {
               return text(legacy->getPulseRepetitionFrequencyIndexSupported());
             }),
             cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetPulseRepetitionFrequencyNumerator) {
  int32_t value = 0;
  const int32_t rc = JSR_GetPulseRepetitionFrequencyNumerator(cManager, &value);
  expectSame(legacyOutcome([&] {
               return text(legacy->getPulseRepetitionFrequencyNumerator());
             }),
             cOutcome(rc, [&] { return text(value, false); }));
}

TEST_F(CApiParityTest, GetPulseRepetitionFrequencyValues) {
  expectSame(legacyOutcome([&] {
               return legacyDoubles([&](double *b, size_t n) {
                 return legacy->getPulseRepetitionFrequencyValues(b, n);
               });
             }),
             cDoubles([&](double *b, size_t n, size_t *c) {
               return JSR_GetPulseRepetitionFrequencyValues(cManager, b, n, c);
             }));
}

TEST_F(CApiParityTest, GetPulseRepetitionFrequencyIndexMax) {
  int32_t value = 0;
  const int32_t rc = JSR_GetPulseRepetitionFrequencyIndexMax(cManager, &value);
  expectSame(legacyOutcome([&] {
               return text(legacy->getPulseRepetitionFrequencyIndexMax());
             }),
             cOutcome(rc, [&] { return text(value, false); }));
}

TEST_F(CApiParityTest, GetPulseRepetitionFrequencyIndex) {
  int32_t value = 0;
  const int32_t rc = JSR_GetPulseRepetitionFrequencyIndex(cManager, &value);
  expectSame(legacyOutcome([&] {
               return text(legacy->getPulseRepetitionFrequencyIndex());
             }),
             cOutcome(rc, [&] { return text(value, false); }));
}

TEST_F(CApiParityTest, SetPulseRepetitionFrequencyIndex_WithCurrentValue) {
  int v = 0;
  try {
    v = legacy->getPulseRepetitionFrequencyIndex();
  } catch (const std::exception &) {
    // Unsupported here: both interfaces must still fail alike.
  }
  expectSame(legacyOutcome([&] {
               legacy->setPulseRepetitionFrequencyIndex(v);
               return std::string();
             }),
             cOutcome(JSR_SetPulseRepetitionFrequencyIndex(cManager, v),
                      [] { return std::string(); }));
}

TEST_F(CApiParityTest, GetPulseRepetitionFrequencyMin) {
  double value = 0.0;
  const int32_t rc = JSR_GetPulseRepetitionFrequencyMin(cManager, &value);
  expectSame(legacyOutcome([&] {
               return text(legacy->getPulseRepetitionFrequencyMin());
             }),
             cOutcome(rc, [&] { return text(value); }));
}

TEST_F(CApiParityTest, GetPulseRepetitionFrequencyMax) {
  double value = 0.0;
  const int32_t rc = JSR_GetPulseRepetitionFrequencyMax(cManager, &value);
  expectSame(legacyOutcome([&] {
               return text(legacy->getPulseRepetitionFrequencyMax());
             }),
             cOutcome(rc, [&] { return text(value); }));
}

TEST_F(CApiParityTest, GetPulseRepetitionFrequency) {
  double value = 0.0;
  const int32_t rc = JSR_GetPulseRepetitionFrequency(cManager, &value);
  expectSame(legacyOutcome(
                 [&] { return text(legacy->getPulseRepetitionFrequency()); }),
             cOutcome(rc, [&] { return text(value); }));
}

TEST_F(CApiParityTest, SetPulseRepetitionFrequency_WithCurrentValue) {
  double v = 0.0;
  try {
    v = legacy->getPulseRepetitionFrequency();
  } catch (const std::exception &) {
    // Unsupported here: both interfaces must still fail alike.
  }
  expectSame(legacyOutcome([&] {
               legacy->setPulseRepetitionFrequency(v);
               return std::string();
             }),
             cOutcome(JSR_SetPulseRepetitionFrequency(cManager, v),
                      [] { return std::string(); }));
}

TEST_F(CApiParityTest, GetMaxFrequency) {
  double value = 0.0;
  const int32_t rc = JSR_GetMaxFrequency(cManager, &value);
  expectSame(legacyOutcome([&] { return text(legacy->getMaxFrequency()); }),
             cOutcome(rc, [&] { return text(value); }));
}

TEST_F(CApiParityTest, GetPulserMaxPRFsSupported) {
  int32_t value = 0;
  const int32_t rc = JSR_GetPulserMaxPRFsSupported(cManager, &value);
  expectSame(
      legacyOutcome([&] { return text(legacy->getPulserMaxPRFsSupported()); }),
      cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetPulserMaxPRFs) {
  expectSame(legacyOutcome([&] {
               return legacyDoubles([&](double *b, size_t n) {
                 return legacy->getPulserMaxPRFs(b, n);
               });
             }),
             cDoubles([&](double *b, size_t n, size_t *c) {
               return JSR_GetPulserMaxPRFs(cManager, b, n, c);
             }));
}

TEST_F(CApiParityTest, GetHVSupplyMax) {
  double value = 0.0;
  const int32_t rc = JSR_GetHVSupplyMax(cManager, &value);
  expectSame(legacyOutcome([&] { return text(legacy->getHVSupplyMax()); }),
             cOutcome(rc, [&] { return text(value); }));
}

TEST_F(CApiParityTest, GetHVSupplyMin) {
  double value = 0.0;
  const int32_t rc = JSR_GetHVSupplyMin(cManager, &value);
  expectSame(legacyOutcome([&] { return text(legacy->getHVSupplyMin()); }),
             cOutcome(rc, [&] { return text(value); }));
}

TEST_F(CApiParityTest, GetHVSupply) {
  double value = 0.0;
  const int32_t rc = JSR_GetHVSupply(cManager, &value);
  expectSame(legacyOutcome([&] { return text(legacy->getHVSupply()); }),
             cOutcome(rc, [&] { return text(value); }));
}

TEST_F(CApiParityTest, SetHVSupply_WithCurrentValue) {
  double v = 0.0;
  try {
    v = legacy->getHVSupply();
  } catch (const std::exception &) {
    // Unsupported here: both interfaces must still fail alike.
  }
  expectSame(
      legacyOutcome([&] {
        legacy->setHVSupply(v);
        return std::string();
      }),
      cOutcome(JSR_SetHVSupply(cManager, v), [] { return std::string(); }));
}

TEST_F(CApiParityTest, GetHVSupplySupported) {
  int32_t value = 0;
  const int32_t rc = JSR_GetHVSupplySupported(cManager, &value);
  expectSame(
      legacyOutcome([&] { return text(legacy->getHVSupplySupported()); }),
      cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetHVSupplyValues) {
  expectSame(legacyOutcome([&] {
               return legacyDoubles([&](double *b, size_t n) {
                 return legacy->getHVSupplyValues(b, n);
               });
             }),
             cDoubles([&](double *b, size_t n, size_t *c) {
               return JSR_GetHVSupplyValues(cManager, b, n, c);
             }));
}

TEST_F(CApiParityTest, GetHVSupplyIndexSupported) {
  int32_t value = 0;
  const int32_t rc = JSR_GetHVSupplyIndexSupported(cManager, &value);
  expectSame(
      legacyOutcome([&] { return text(legacy->getHVSupplyIndexSupported()); }),
      cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetHVSupplyIndexMax) {
  int32_t value = 0;
  const int32_t rc = JSR_GetHVSupplyIndexMax(cManager, &value);
  expectSame(legacyOutcome([&] { return text(legacy->getHVSupplyIndexMax()); }),
             cOutcome(rc, [&] { return text(value, false); }));
}

TEST_F(CApiParityTest, GetHVSupplyIndex) {
  int32_t value = 0;
  const int32_t rc = JSR_GetHVSupplyIndex(cManager, &value);
  expectSame(legacyOutcome([&] { return text(legacy->getHVSupplyIndex()); }),
             cOutcome(rc, [&] { return text(value, false); }));
}

TEST_F(CApiParityTest, SetHVSupplyIndex_WithCurrentValue) {
  int v = 0;
  try {
    v = legacy->getHVSupplyIndex();
  } catch (const std::exception &) {
    // Unsupported here: both interfaces must still fail alike.
  }
  expectSame(legacyOutcome([&] {
               legacy->setHVSupplyIndex(v);
               return std::string();
             }),
             cOutcome(JSR_SetHVSupplyIndex(cManager, v),
                      [] { return std::string(); }));
}

TEST_F(CApiParityTest, GetHVSupplyEnable) {
  int32_t value = 0;
  const int32_t rc = JSR_GetHVSupplyEnable(cManager, &value);
  expectSame(legacyOutcome([&] { return text(legacy->getHVSupplyEnable()); }),
             cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, SetHVSupplyEnable_WithCurrentValue) {
  bool v = false;
  try {
    v = legacy->getHVSupplyEnable();
  } catch (const std::exception &) {
    // Unsupported here: both interfaces must still fail alike.
  }
  expectSame(legacyOutcome([&] {
               legacy->setHVSupplyEnable(v);
               return std::string();
             }),
             cOutcome(JSR_SetHVSupplyEnable(cManager, v ? 1 : 0),
                      [] { return std::string(); }));
}

TEST_F(CApiParityTest, GetHVSupplyEnableSupported) {
  int32_t value = 0;
  const int32_t rc = JSR_GetHVSupplyEnableSupported(cManager, &value);
  expectSame(
      legacyOutcome([&] { return text(legacy->getHVSupplyEnableSupported()); }),
      cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetHVMeasurement) {
  double value = 0.0;
  const int32_t rc = JSR_GetHVMeasurement(cManager, &value);
  expectSame(legacyOutcome([&] { return text(legacy->getHVMeasurement()); }),
             cOutcome(rc, [&] { return text(value); }));
}

TEST_F(CApiParityTest, GetHVMeasurementSupported) {
  int32_t value = 0;
  const int32_t rc = JSR_GetHVMeasurementSupported(cManager, &value);
  expectSame(
      legacyOutcome([&] { return text(legacy->getHVMeasurementSupported()); }),
      cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetTriggerEdgePolarity) {
  int32_t value = 0;
  const int32_t rc = JSR_GetTriggerEdgePolarity(cManager, &value);
  expectSame(
      legacyOutcome([&] { return text(legacy->getTriggerEdgePolarity()); }),
      cOutcome(rc, [&] { return text(value, false); }));
}

TEST_F(CApiParityTest, SetTriggerEdgePolarity_WithCurrentValue) {
  C_TRIGGER_POLARITY v = C_TRIGGER_POLARITY{};
  try {
    v = legacy->getTriggerEdgePolarity();
  } catch (const std::exception &) {
    // Unsupported here: both interfaces must still fail alike.
  }
  expectSame(
      legacyOutcome([&] {
        legacy->setTriggerEdgePolarity(v);
        return std::string();
      }),
      cOutcome(JSR_SetTriggerEdgePolarity(cManager, static_cast<int32_t>(v)),
               [] { return std::string(); }));
}

TEST_F(CApiParityTest, GetTriggerEdgePolaritySupported) {
  int32_t value = 0;
  const int32_t rc = JSR_GetTriggerEdgePolaritySupported(cManager, &value);
  expectSame(legacyOutcome([&] {
               return text(legacy->getTriggerEdgePolaritySupported());
             }),
             cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetTriggerSource) {
  int32_t value = 0;
  const int32_t rc = JSR_GetTriggerSource(cManager, &value);
  expectSame(legacyOutcome([&] { return text(legacy->getTriggerSource()); }),
             cOutcome(rc, [&] { return text(value, false); }));
}

TEST_F(CApiParityTest, SetTriggerSource_WithCurrentValue) {
  C_TRIGGER_SOURCE v = C_TRIGGER_SOURCE{};
  try {
    v = legacy->getTriggerSource();
  } catch (const std::exception &) {
    // Unsupported here: both interfaces must still fail alike.
  }
  expectSame(legacyOutcome([&] {
               legacy->setTriggerSource(v);
               return std::string();
             }),
             cOutcome(JSR_SetTriggerSource(cManager, static_cast<int32_t>(v)),
                      [] { return std::string(); }));
}

TEST_F(CApiParityTest, GetTriggerSourceInternalSupported) {
  int32_t value = 0;
  const int32_t rc = JSR_GetTriggerSourceInternalSupported(cManager, &value);
  expectSame(legacyOutcome([&] {
               return text(legacy->getTriggerSourceInternalSupported());
             }),
             cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetTriggerSourceExternalSupported) {
  int32_t value = 0;
  const int32_t rc = JSR_GetTriggerSourceExternalSupported(cManager, &value);
  expectSame(legacyOutcome([&] {
               return text(legacy->getTriggerSourceExternalSupported());
             }),
             cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetTriggerSourceSlaveSupported) {
  int32_t value = 0;
  const int32_t rc = JSR_GetTriggerSourceSlaveSupported(cManager, &value);
  expectSame(legacyOutcome([&] {
               return text(legacy->getTriggerSourceSlaveSupported());
             }),
             cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetPulserTriggerSourceValueNames) {
  expectSame(legacyOutcome([&] {
               return legacyStrings([&](char **b, size_t n, size_t len) {
                 return legacy->getPulserTriggerSourceValueNames(b, n, len);
               });
             }),
             cStrings([&](char **b, size_t n, size_t len, size_t *c) {
               return JSR_GetPulserTriggerSourceValueNames(cManager, b, n, len,
                                                           c);
             }));
}

TEST_F(CApiParityTest, GetPulserTriggerSourceIndexMax) {
  int32_t value = 0;
  const int32_t rc = JSR_GetPulserTriggerSourceIndexMax(cManager, &value);
  expectSame(legacyOutcome([&] {
               return text(legacy->getPulserTriggerSourceIndexMax());
             }),
             cOutcome(rc, [&] { return text(value, false); }));
}

TEST_F(CApiParityTest, GetPulserTriggerSourceIndex) {
  int32_t value = 0;
  const int32_t rc = JSR_GetPulserTriggerSourceIndex(cManager, &value);
  expectSame(legacyOutcome(
                 [&] { return text(legacy->getPulserTriggerSourceIndex()); }),
             cOutcome(rc, [&] { return text(value, false); }));
}

TEST_F(CApiParityTest, SetPulserTriggerSourceIndex_WithCurrentValue) {
  int v = 0;
  try {
    v = legacy->getPulserTriggerSourceIndex();
  } catch (const std::exception &) {
    // Unsupported here: both interfaces must still fail alike.
  }
  expectSame(legacyOutcome([&] {
               legacy->setPulserTriggerSourceIndex(v);
               return std::string();
             }),
             cOutcome(JSR_SetPulserTriggerSourceIndex(cManager, v),
                      [] { return std::string(); }));
}

TEST_F(CApiParityTest, GetTriggerEnable) {
  int32_t value = 0;
  const int32_t rc = JSR_GetTriggerEnable(cManager, &value);
  expectSame(legacyOutcome([&] { return text(legacy->getTriggerEnable()); }),
             cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, SetTriggerEnable_WithCurrentValue) {
  bool v = false;
  try {
    v = legacy->getTriggerEnable();
  } catch (const std::exception &) {
    // Unsupported here: both interfaces must still fail alike.
  }
  expectSame(legacyOutcome([&] {
               legacy->setTriggerEnable(v);
               return std::string();
             }),
             cOutcome(JSR_SetTriggerEnable(cManager, v ? 1 : 0),
                      [] { return std::string(); }));
}

TEST_F(CApiParityTest, GetTriggerImpedance) {
  int32_t value = 0;
  const int32_t rc = JSR_GetTriggerImpedance(cManager, &value);
  expectSame(legacyOutcome([&] { return text(legacy->getTriggerImpedance()); }),
             cOutcome(rc, [&] { return text(value, false); }));
}

TEST_F(CApiParityTest, SetTriggerImpedance_WithCurrentValue) {
  C_TRIGGER_IMPEDANCE v = C_TRIGGER_IMPEDANCE{};
  try {
    v = legacy->getTriggerImpedance();
  } catch (const std::exception &) {
    // Unsupported here: both interfaces must still fail alike.
  }
  expectSame(
      legacyOutcome([&] {
        legacy->setTriggerImpedance(v);
        return std::string();
      }),
      cOutcome(JSR_SetTriggerImpedance(cManager, static_cast<int32_t>(v)),
               [] { return std::string(); }));
}

TEST_F(CApiParityTest, GetTriggerImpedanceSupported) {
  int32_t value = 0;
  const int32_t rc = JSR_GetTriggerImpedanceSupported(cManager, &value);
  expectSame(legacyOutcome(
                 [&] { return text(legacy->getTriggerImpedanceSupported()); }),
             cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetPulserTriggerCount) {
  int32_t value = 0;
  const int32_t rc = JSR_GetPulserTriggerCount(cManager, &value);
  expectSame(
      legacyOutcome([&] { return text(legacy->getPulserTriggerCount()); }),
      cOutcome(rc, [&] { return text(value, false); }));
}

TEST_F(CApiParityTest, GetPulserTriggerCountSupported) {
  int32_t value = 0;
  const int32_t rc = JSR_GetPulserTriggerCountSupported(cManager, &value);
  expectSame(legacyOutcome([&] {
               return text(legacy->getPulserTriggerCountSupported());
             }),
             cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetHighPassFilterValues) {
  expectSame(legacyOutcome([&] {
               return legacyDoubles([&](double *b, size_t n) {
                 return legacy->getHighPassFilterValues(b, n);
               });
             }),
             cDoubles([&](double *b, size_t n, size_t *c) {
               return JSR_GetHighPassFilterValues(cManager, b, n, c);
             }));
}

TEST_F(CApiParityTest, GetHighPassFilterIndexMax) {
  int32_t value = 0;
  const int32_t rc = JSR_GetHighPassFilterIndexMax(cManager, &value);
  expectSame(
      legacyOutcome([&] { return text(legacy->getHighPassFilterIndexMax()); }),
      cOutcome(rc, [&] { return text(value, false); }));
}

TEST_F(CApiParityTest, GetHighPassFilterIndex) {
  int32_t value = 0;
  const int32_t rc = JSR_GetHighPassFilterIndex(cManager, &value);
  expectSame(
      legacyOutcome([&] { return text(legacy->getHighPassFilterIndex()); }),
      cOutcome(rc, [&] { return text(value, false); }));
}

TEST_F(CApiParityTest, SetHighPassFilterIndex_WithCurrentValue) {
  int v = 0;
  try {
    v = legacy->getHighPassFilterIndex();
  } catch (const std::exception &) {
    // Unsupported here: both interfaces must still fail alike.
  }
  expectSame(legacyOutcome([&] {
               legacy->setHighPassFilterIndex(v);
               return std::string();
             }),
             cOutcome(JSR_SetHighPassFilterIndex(cManager, v),
                      [] { return std::string(); }));
}

TEST_F(CApiParityTest, GetLowPassFilterValues) {
  expectSame(legacyOutcome([&] {
               return legacyDoubles([&](double *b, size_t n) {
                 return legacy->getLowPassFilterValues(b, n);
               });
             }),
             cDoubles([&](double *b, size_t n, size_t *c) {
               return JSR_GetLowPassFilterValues(cManager, b, n, c);
             }));
}

TEST_F(CApiParityTest, GetLowPassFilterIndexMax) {
  int32_t value = 0;
  const int32_t rc = JSR_GetLowPassFilterIndexMax(cManager, &value);
  expectSame(
      legacyOutcome([&] { return text(legacy->getLowPassFilterIndexMax()); }),
      cOutcome(rc, [&] { return text(value, false); }));
}

TEST_F(CApiParityTest, GetLowPassFilterIndex) {
  int32_t value = 0;
  const int32_t rc = JSR_GetLowPassFilterIndex(cManager, &value);
  expectSame(
      legacyOutcome([&] { return text(legacy->getLowPassFilterIndex()); }),
      cOutcome(rc, [&] { return text(value, false); }));
}

TEST_F(CApiParityTest, SetLowPassFilterIndex_WithCurrentValue) {
  int v = 0;
  try {
    v = legacy->getLowPassFilterIndex();
  } catch (const std::exception &) {
    // Unsupported here: both interfaces must still fail alike.
  }
  expectSame(legacyOutcome([&] {
               legacy->setLowPassFilterIndex(v);
               return std::string();
             }),
             cOutcome(JSR_SetLowPassFilterIndex(cManager, v),
                      [] { return std::string(); }));
}

TEST_F(CApiParityTest, GetReceiverMode) {
  int32_t value = 0;
  const int32_t rc = JSR_GetReceiverMode(cManager, &value);
  expectSame(legacyOutcome([&] { return text(legacy->getReceiverMode()); }),
             cOutcome(rc, [&] { return text(value, false); }));
}

TEST_F(CApiParityTest, SetReceiverMode_WithCurrentValue) {
  C_RECEIVER_MODE v = C_RECEIVER_MODE{};
  try {
    v = legacy->getReceiverMode();
  } catch (const std::exception &) {
    // Unsupported here: both interfaces must still fail alike.
  }
  expectSame(legacyOutcome([&] {
               legacy->setReceiverMode(v);
               return std::string();
             }),
             cOutcome(JSR_SetReceiverMode(cManager, static_cast<int32_t>(v)),
                      [] { return std::string(); }));
}

TEST_F(CApiParityTest, GetReceiverModeBothSupported) {
  int32_t value = 0;
  const int32_t rc = JSR_GetReceiverModeBothSupported(cManager, &value);
  expectSame(legacyOutcome(
                 [&] { return text(legacy->getReceiverModeBothSupported()); }),
             cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetReceiverModeThruSupported) {
  int32_t value = 0;
  const int32_t rc = JSR_GetReceiverModeThruSupported(cManager, &value);
  expectSame(legacyOutcome(
                 [&] { return text(legacy->getReceiverModeThruSupported()); }),
             cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetReceiverModeEchoSupported) {
  int32_t value = 0;
  const int32_t rc = JSR_GetReceiverModeEchoSupported(cManager, &value);
  expectSame(legacyOutcome(
                 [&] { return text(legacy->getReceiverModeEchoSupported()); }),
             cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetReceiverSupplyVoltages) {
  expectSame(legacyOutcome([&] {
               return legacyStrings([&](char **b, size_t n, size_t len) {
                 return legacy->getReceiverSupplyVoltages(b, n, len);
               });
             }),
             cStrings([&](char **b, size_t n, size_t len, size_t *c) {
               return JSR_GetReceiverSupplyVoltages(cManager, b, n, len, c);
             }));
}

TEST_F(CApiParityTest, GetReceiverSupplyVoltagesSupported) {
  int32_t value = 0;
  const int32_t rc = JSR_GetReceiverSupplyVoltagesSupported(cManager, &value);
  expectSame(legacyOutcome([&] {
               return text(legacy->getReceiverSupplyVoltagesSupported());
             }),
             cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetGainValues) {
  expectSame(legacyOutcome([&] {
               return legacyDoubles([&](double *b, size_t n) {
                 return legacy->getGainValues(b, n);
               });
             }),
             cDoubles([&](double *b, size_t n, size_t *c) {
               return JSR_GetGainValues(cManager, b, n, c);
             }));
}

TEST_F(CApiParityTest, GetGainIndexMax) {
  int32_t value = 0;
  const int32_t rc = JSR_GetGainIndexMax(cManager, &value);
  expectSame(legacyOutcome([&] { return text(legacy->getGainIndexMax()); }),
             cOutcome(rc, [&] { return text(value, false); }));
}

TEST_F(CApiParityTest, GetGainIndex) {
  int32_t value = 0;
  const int32_t rc = JSR_GetGainIndex(cManager, &value);
  expectSame(legacyOutcome([&] { return text(legacy->getGainIndex()); }),
             cOutcome(rc, [&] { return text(value, false); }));
}

TEST_F(CApiParityTest, SetGainIndex_WithCurrentValue) {
  int v = 0;
  try {
    v = legacy->getGainIndex();
  } catch (const std::exception &) {
    // Unsupported here: both interfaces must still fail alike.
  }
  expectSame(
      legacyOutcome([&] {
        legacy->setGainIndex(v);
        return std::string();
      }),
      cOutcome(JSR_SetGainIndex(cManager, v), [] { return std::string(); }));
}

TEST_F(CApiParityTest, GetGainIndexSupported) {
  int32_t value = 0;
  const int32_t rc = JSR_GetGainIndexSupported(cManager, &value);
  expectSame(
      legacyOutcome([&] { return text(legacy->getGainIndexSupported()); }),
      cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetGainStepSizeSupported) {
  int32_t value = 0;
  const int32_t rc = JSR_GetGainStepSizeSupported(cManager, &value);
  expectSame(
      legacyOutcome([&] { return text(legacy->getGainStepSizeSupported()); }),
      cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetGainStepSize) {
  double value = 0.0;
  const int32_t rc = JSR_GetGainStepSize(cManager, &value);
  expectSame(legacyOutcome([&] { return text(legacy->getGainStepSize()); }),
             cOutcome(rc, [&] { return text(value); }));
}

TEST_F(CApiParityTest, GetGainMin) {
  double value = 0.0;
  const int32_t rc = JSR_GetGainMin(cManager, &value);
  expectSame(legacyOutcome([&] { return text(legacy->getGainMin()); }),
             cOutcome(rc, [&] { return text(value); }));
}

TEST_F(CApiParityTest, GetGainMax) {
  double value = 0.0;
  const int32_t rc = JSR_GetGainMax(cManager, &value);
  expectSame(legacyOutcome([&] { return text(legacy->getGainMax()); }),
             cOutcome(rc, [&] { return text(value); }));
}

TEST_F(CApiParityTest, GetGain) {
  double value = 0.0;
  const int32_t rc = JSR_GetGain(cManager, &value);
  expectSame(legacyOutcome([&] { return text(legacy->getGain()); }),
             cOutcome(rc, [&] { return text(value); }));
}

TEST_F(CApiParityTest, SetGain_WithCurrentValue) {
  double v = 0.0;
  try {
    v = legacy->getGain();
  } catch (const std::exception &) {
    // Unsupported here: both interfaces must still fail alike.
  }
  expectSame(legacyOutcome([&] {
               legacy->setGain(v);
               return std::string();
             }),
             cOutcome(JSR_SetGain(cManager, v), [] { return std::string(); }));
}

TEST_F(CApiParityTest, GetPulseEnergyValueNames) {
  expectSame(legacyOutcome([&] {
               return legacyStrings([&](char **b, size_t n, size_t len) {
                 return legacy->getPulseEnergyValueNames(b, n, len);
               });
             }),
             cStrings([&](char **b, size_t n, size_t len, size_t *c) {
               return JSR_GetPulseEnergyValueNames(cManager, b, n, len, c);
             }));
}

TEST_F(CApiParityTest, GetPulseEnergyIndexMax) {
  int32_t value = 0;
  const int32_t rc = JSR_GetPulseEnergyIndexMax(cManager, &value);
  expectSame(
      legacyOutcome([&] { return text(legacy->getPulseEnergyIndexMax()); }),
      cOutcome(rc, [&] { return text(value, false); }));
}

TEST_F(CApiParityTest, GetPulseEnergyIndex) {
  int32_t value = 0;
  const int32_t rc = JSR_GetPulseEnergyIndex(cManager, &value);
  expectSame(legacyOutcome([&] { return text(legacy->getPulseEnergyIndex()); }),
             cOutcome(rc, [&] { return text(value, false); }));
}

TEST_F(CApiParityTest, SetPulseEnergyIndex_WithCurrentValue) {
  int v = 0;
  try {
    v = legacy->getPulseEnergyIndex();
  } catch (const std::exception &) {
    // Unsupported here: both interfaces must still fail alike.
  }
  expectSame(legacyOutcome([&] {
               legacy->setPulseEnergyIndex(v);
               return std::string();
             }),
             cOutcome(JSR_SetPulseEnergyIndex(cManager, v),
                      [] { return std::string(); }));
}

TEST_F(CApiParityTest, GetPulseEnergyIndexSupported) {
  int32_t value = 0;
  const int32_t rc = JSR_GetPulseEnergyIndexSupported(cManager, &value);
  expectSame(legacyOutcome(
                 [&] { return text(legacy->getPulseEnergyIndexSupported()); }),
             cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetEnergyPerPulse) {
  double value = 0.0;
  const int32_t rc = JSR_GetEnergyPerPulse(cManager, &value);
  expectSame(legacyOutcome([&] { return text(legacy->getEnergyPerPulse()); }),
             cOutcome(rc, [&] { return text(value); }));
}

TEST_F(CApiParityTest, GetPulserEnergyCapacitorValuesSupported) {
  int32_t value = 0;
  const int32_t rc =
      JSR_GetPulserEnergyCapacitorValuesSupported(cManager, &value);
  expectSame(legacyOutcome([&] {
               return text(legacy->getPulserEnergyCapacitorValuesSupported());
             }),
             cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetPulserEnergyCapacitorValues) {
  expectSame(legacyOutcome([&] {
               return legacyDoubles([&](double *b, size_t n) {
                 return legacy->getPulserEnergyCapacitorValues(b, n);
               });
             }),
             cDoubles([&](double *b, size_t n, size_t *c) {
               return JSR_GetPulserEnergyCapacitorValues(cManager, b, n, c);
             }));
}

TEST_F(CApiParityTest, GetDampingValues) {
  expectSame(legacyOutcome([&] {
               return legacyDoubles([&](double *b, size_t n) {
                 return legacy->getDampingValues(b, n);
               });
             }),
             cDoubles([&](double *b, size_t n, size_t *c) {
               return JSR_GetDampingValues(cManager, b, n, c);
             }));
}

TEST_F(CApiParityTest, GetDampingIndexMax) {
  int32_t value = 0;
  const int32_t rc = JSR_GetDampingIndexMax(cManager, &value);
  expectSame(legacyOutcome([&] { return text(legacy->getDampingIndexMax()); }),
             cOutcome(rc, [&] { return text(value, false); }));
}

TEST_F(CApiParityTest, GetDampingIndex) {
  int32_t value = 0;
  const int32_t rc = JSR_GetDampingIndex(cManager, &value);
  expectSame(legacyOutcome([&] { return text(legacy->getDampingIndex()); }),
             cOutcome(rc, [&] { return text(value, false); }));
}

TEST_F(CApiParityTest, SetDampingIndex_WithCurrentValue) {
  int v = 0;
  try {
    v = legacy->getDampingIndex();
  } catch (const std::exception &) {
    // Unsupported here: both interfaces must still fail alike.
  }
  expectSame(
      legacyOutcome([&] {
        legacy->setDampingIndex(v);
        return std::string();
      }),
      cOutcome(JSR_SetDampingIndex(cManager, v), [] { return std::string(); }));
}

TEST_F(CApiParityTest, GetDampingIndexSupported) {
  int32_t value = 0;
  const int32_t rc = JSR_GetDampingIndexSupported(cManager, &value);
  expectSame(
      legacyOutcome([&] { return text(legacy->getDampingIndexSupported()); }),
      cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetPulserImpedance) {
  int32_t value = 0;
  const int32_t rc = JSR_GetPulserImpedance(cManager, &value);
  expectSame(legacyOutcome([&] { return text(legacy->getPulserImpedance()); }),
             cOutcome(rc, [&] { return text(value, false); }));
}

TEST_F(CApiParityTest, SetPulserImpedance_WithCurrentValue) {
  C_PULSER_IMPEDANCE v = C_PULSER_IMPEDANCE{};
  try {
    v = legacy->getPulserImpedance();
  } catch (const std::exception &) {
    // Unsupported here: both interfaces must still fail alike.
  }
  expectSame(legacyOutcome([&] {
               legacy->setPulserImpedance(v);
               return std::string();
             }),
             cOutcome(JSR_SetPulserImpedance(cManager, static_cast<int32_t>(v)),
                      [] { return std::string(); }));
}

TEST_F(CApiParityTest, GetPulserImpedanceSupported) {
  int32_t value = 0;
  const int32_t rc = JSR_GetPulserImpedanceSupported(cManager, &value);
  expectSame(legacyOutcome(
                 [&] { return text(legacy->getPulserImpedanceSupported()); }),
             cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetPulserIsPulsing) {
  int32_t value = 0;
  const int32_t rc = JSR_GetPulserIsPulsing(cManager, &value);
  expectSame(legacyOutcome([&] { return text(legacy->getPulserIsPulsing()); }),
             cOutcome(rc, [&] { return text(value, false); }));
}

TEST_F(CApiParityTest, GetPulserPowerLimitStatus) {
  int32_t value = 0;
  const int32_t rc = JSR_GetPulserPowerLimitStatus(cManager, &value);
  expectSame(
      legacyOutcome([&] { return text(legacy->getPulserPowerLimitStatus()); }),
      cOutcome(rc, [&] { return text(value, false); }));
}

TEST_F(CApiParityTest, GetIsPulserPresent) {
  int32_t value = 0;
  const int32_t rc = JSR_GetIsPulserPresent(cManager, &value);
  expectSame(legacyOutcome([&] { return text(legacy->getIsPulserPresent()); }),
             cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetIsPulserPresentSupported) {
  int32_t value = 0;
  const int32_t rc = JSR_GetIsPulserPresentSupported(cManager, &value);
  expectSame(legacyOutcome(
                 [&] { return text(legacy->getIsPulserPresentSupported()); }),
             cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetHasManualControls) {
  int32_t value = 0;
  const int32_t rc = JSR_GetHasManualControls(cManager, &value);
  expectSame(
      legacyOutcome([&] { return text(legacy->getHasManualControls()); }),
      cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetLEDBlinkModeValues) {
  expectSame(legacyOutcome([&] {
               return legacyStrings([&](char **b, size_t n, size_t len) {
                 return legacy->getLEDBlinkModeValues(b, n, len);
               });
             }),
             cStrings([&](char **b, size_t n, size_t len, size_t *c) {
               return JSR_GetLEDBlinkModeValues(cManager, b, n, len, c);
             }));
}

TEST_F(CApiParityTest, GetLEDBlinkModeIndexMax) {
  int32_t value = 0;
  const int32_t rc = JSR_GetLEDBlinkModeIndexMax(cManager, &value);
  expectSame(
      legacyOutcome([&] { return text(legacy->getLEDBlinkModeIndexMax()); }),
      cOutcome(rc, [&] { return text(value, false); }));
}

TEST_F(CApiParityTest, GetLEDBlinkModeIndex) {
  int32_t value = 0;
  const int32_t rc = JSR_GetLEDBlinkModeIndex(cManager, &value);
  expectSame(
      legacyOutcome([&] { return text(legacy->getLEDBlinkModeIndex()); }),
      cOutcome(rc, [&] { return text(value, false); }));
}

TEST_F(CApiParityTest, SetLEDBlinkModeIndex_WithCurrentValue) {
  int v = 0;
  try {
    v = legacy->getLEDBlinkModeIndex();
  } catch (const std::exception &) {
    // Unsupported here: both interfaces must still fail alike.
  }
  expectSame(legacyOutcome([&] {
               legacy->setLEDBlinkModeIndex(v);
               return std::string();
             }),
             cOutcome(JSR_SetLEDBlinkModeIndex(cManager, v),
                      [] { return std::string(); }));
}

TEST_F(CApiParityTest, GetLEDBlinkModeIndexSupported) {
  int32_t value = 0;
  const int32_t rc = JSR_GetLEDBlinkModeIndexSupported(cManager, &value);
  expectSame(legacyOutcome(
                 [&] { return text(legacy->getLEDBlinkModeIndexSupported()); }),
             cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetUnitModelName) {
  expectSame(legacyOutcome([&] {
               return legacyString([&](char *b, size_t n) {
                 return legacy->getUnitModelName(b, n);
               });
             }),
             cString([&](char *b, size_t n, size_t *r) {
               return JSR_GetUnitModelName(cManager, b, n, r);
             }));
}

TEST_F(CApiParityTest, SetUnitModelName_WithCurrentValue) {
  std::string v = std::string();
  try {
    v = legacyString(
        [&](char *b, size_t n) { return legacy->getUnitModelName(b, n); });
  } catch (const std::exception &) {
    // Unsupported here: both interfaces must still fail alike.
  }
  expectSame(legacyOutcome([&] {
               legacy->setUnitModelName(v.c_str());
               return std::string();
             }),
             cOutcome(JSR_SetUnitModelName(cManager, v.c_str()),
                      [] { return std::string(); }));
}

TEST_F(CApiParityTest, GetUnitModelNameSupported) {
  int32_t value = 0;
  const int32_t rc = JSR_GetUnitModelNameSupported(cManager, &value);
  expectSame(
      legacyOutcome([&] { return text(legacy->getUnitModelNameSupported()); }),
      cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetUnitSerialNum) {
  expectSame(legacyOutcome([&] {
               return legacyString([&](char *b, size_t n) {
                 return legacy->getUnitSerialNum(b, n);
               });
             }),
             cString([&](char *b, size_t n, size_t *r) {
               return JSR_GetUnitSerialNum(cManager, b, n, r);
             }));
}

TEST_F(CApiParityTest, SetUnitSerialNum_WithCurrentValue) {
  std::string v = std::string();
  try {
    v = legacyString(
        [&](char *b, size_t n) { return legacy->getUnitSerialNum(b, n); });
  } catch (const std::exception &) {
    // Unsupported here: both interfaces must still fail alike.
  }
  expectSame(legacyOutcome([&] {
               legacy->setUnitSerialNum(v.c_str());
               return std::string();
             }),
             cOutcome(JSR_SetUnitSerialNum(cManager, v.c_str()),
                      [] { return std::string(); }));
}

TEST_F(CApiParityTest, GetUnitSerialNumSupported) {
  int32_t value = 0;
  const int32_t rc = JSR_GetUnitSerialNumSupported(cManager, &value);
  expectSame(
      legacyOutcome([&] { return text(legacy->getUnitSerialNumSupported()); }),
      cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetPulserModelName) {
  expectSame(legacyOutcome([&] {
               return legacyString([&](char *b, size_t n) {
                 return legacy->getPulserModelName(b, n);
               });
             }),
             cString([&](char *b, size_t n, size_t *r) {
               return JSR_GetPulserModelName(cManager, b, n, r);
             }));
}

TEST_F(CApiParityTest, SetPulserModelName_WithCurrentValue) {
  std::string v = std::string();
  try {
    v = legacyString(
        [&](char *b, size_t n) { return legacy->getPulserModelName(b, n); });
  } catch (const std::exception &) {
    // Unsupported here: both interfaces must still fail alike.
  }
  expectSame(legacyOutcome([&] {
               legacy->setPulserModelName(v.c_str());
               return std::string();
             }),
             cOutcome(JSR_SetPulserModelName(cManager, v.c_str()),
                      [] { return std::string(); }));
}

TEST_F(CApiParityTest, GetPulserModelNameSupported) {
  int32_t value = 0;
  const int32_t rc = JSR_GetPulserModelNameSupported(cManager, &value);
  expectSame(legacyOutcome(
                 [&] { return text(legacy->getPulserModelNameSupported()); }),
             cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetPulserSerialNum) {
  expectSame(legacyOutcome([&] {
               return legacyString([&](char *b, size_t n) {
                 return legacy->getPulserSerialNum(b, n);
               });
             }),
             cString([&](char *b, size_t n, size_t *r) {
               return JSR_GetPulserSerialNum(cManager, b, n, r);
             }));
}

TEST_F(CApiParityTest, SetPulserSerialNum_WithCurrentValue) {
  std::string v = std::string();
  try {
    v = legacyString(
        [&](char *b, size_t n) { return legacy->getPulserSerialNum(b, n); });
  } catch (const std::exception &) {
    // Unsupported here: both interfaces must still fail alike.
  }
  expectSame(legacyOutcome([&] {
               legacy->setPulserSerialNum(v.c_str());
               return std::string();
             }),
             cOutcome(JSR_SetPulserSerialNum(cManager, v.c_str()),
                      [] { return std::string(); }));
}

TEST_F(CApiParityTest, GetPulserSerialNumSupported) {
  int32_t value = 0;
  const int32_t rc = JSR_GetPulserSerialNumSupported(cManager, &value);
  expectSame(legacyOutcome(
                 [&] { return text(legacy->getPulserSerialNumSupported()); }),
             cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetPulserHWRev) {
  expectSame(legacyOutcome([&] {
               return legacyString([&](char *b, size_t n) {
                 return legacy->getPulserHWRev(b, n);
               });
             }),
             cString([&](char *b, size_t n, size_t *r) {
               return JSR_GetPulserHWRev(cManager, b, n, r);
             }));
}

TEST_F(CApiParityTest, SetPulserHWRev_WithCurrentValue) {
  std::string v = std::string();
  try {
    v = legacyString(
        [&](char *b, size_t n) { return legacy->getPulserHWRev(b, n); });
  } catch (const std::exception &) {
    // Unsupported here: both interfaces must still fail alike.
  }
  expectSame(legacyOutcome([&] {
               legacy->setPulserHWRev(v.c_str());
               return std::string();
             }),
             cOutcome(JSR_SetPulserHWRev(cManager, v.c_str()),
                      [] { return std::string(); }));
}

TEST_F(CApiParityTest, GetPulserHWRevSupported) {
  int32_t value = 0;
  const int32_t rc = JSR_GetPulserHWRevSupported(cManager, &value);
  expectSame(
      legacyOutcome([&] { return text(legacy->getPulserHWRevSupported()); }),
      cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetPulserFirmwareVer) {
  expectSame(legacyOutcome([&] {
               return legacyString([&](char *b, size_t n) {
                 return legacy->getPulserFirmwareVer(b, n);
               });
             }),
             cString([&](char *b, size_t n, size_t *r) {
               return JSR_GetPulserFirmwareVer(cManager, b, n, r);
             }));
}

TEST_F(CApiParityTest, GetPulserFirmwareVerSupported) {
  int32_t value = 0;
  const int32_t rc = JSR_GetPulserFirmwareVerSupported(cManager, &value);
  expectSame(legacyOutcome(
                 [&] { return text(legacy->getPulserFirmwareVerSupported()); }),
             cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetPulserOEMDataSupported) {
  int32_t value = 0;
  const int32_t rc = JSR_GetPulserOEMDataSupported(cManager, &value);
  expectSame(
      legacyOutcome([&] { return text(legacy->getPulserOEMDataSupported()); }),
      cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetReceiverModelName) {
  expectSame(legacyOutcome([&] {
               return legacyString([&](char *b, size_t n) {
                 return legacy->getReceiverModelName(b, n);
               });
             }),
             cString([&](char *b, size_t n, size_t *r) {
               return JSR_GetReceiverModelName(cManager, b, n, r);
             }));
}

TEST_F(CApiParityTest, SetReceiverModelName_WithCurrentValue) {
  std::string v = std::string();
  try {
    v = legacyString(
        [&](char *b, size_t n) { return legacy->getReceiverModelName(b, n); });
  } catch (const std::exception &) {
    // Unsupported here: both interfaces must still fail alike.
  }
  expectSame(legacyOutcome([&] {
               legacy->setReceiverModelName(v.c_str());
               return std::string();
             }),
             cOutcome(JSR_SetReceiverModelName(cManager, v.c_str()),
                      [] { return std::string(); }));
}

TEST_F(CApiParityTest, GetReceiverModelNameSupported) {
  int32_t value = 0;
  const int32_t rc = JSR_GetReceiverModelNameSupported(cManager, &value);
  expectSame(legacyOutcome(
                 [&] { return text(legacy->getReceiverModelNameSupported()); }),
             cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetReceiverSerialNum) {
  expectSame(legacyOutcome([&] {
               return legacyString([&](char *b, size_t n) {
                 return legacy->getReceiverSerialNum(b, n);
               });
             }),
             cString([&](char *b, size_t n, size_t *r) {
               return JSR_GetReceiverSerialNum(cManager, b, n, r);
             }));
}

TEST_F(CApiParityTest, SetReceiverSerialNum_WithCurrentValue) {
  std::string v = std::string();
  try {
    v = legacyString(
        [&](char *b, size_t n) { return legacy->getReceiverSerialNum(b, n); });
  } catch (const std::exception &) {
    // Unsupported here: both interfaces must still fail alike.
  }
  expectSame(legacyOutcome([&] {
               legacy->setReceiverSerialNum(v.c_str());
               return std::string();
             }),
             cOutcome(JSR_SetReceiverSerialNum(cManager, v.c_str()),
                      [] { return std::string(); }));
}

TEST_F(CApiParityTest, GetReceiverSerialNumSupported) {
  int32_t value = 0;
  const int32_t rc = JSR_GetReceiverSerialNumSupported(cManager, &value);
  expectSame(legacyOutcome(
                 [&] { return text(legacy->getReceiverSerialNumSupported()); }),
             cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetReceiverHWRev) {
  expectSame(legacyOutcome([&] {
               return legacyString([&](char *b, size_t n) {
                 return legacy->getReceiverHWRev(b, n);
               });
             }),
             cString([&](char *b, size_t n, size_t *r) {
               return JSR_GetReceiverHWRev(cManager, b, n, r);
             }));
}

TEST_F(CApiParityTest, SetReceiverHWRev_WithCurrentValue) {
  std::string v = std::string();
  try {
    v = legacyString(
        [&](char *b, size_t n) { return legacy->getReceiverHWRev(b, n); });
  } catch (const std::exception &) {
    // Unsupported here: both interfaces must still fail alike.
  }
  expectSame(legacyOutcome([&] {
               legacy->setReceiverHWRev(v.c_str());
               return std::string();
             }),
             cOutcome(JSR_SetReceiverHWRev(cManager, v.c_str()),
                      [] { return std::string(); }));
}

TEST_F(CApiParityTest, GetReceiverHWRevSupported) {
  int32_t value = 0;
  const int32_t rc = JSR_GetReceiverHWRevSupported(cManager, &value);
  expectSame(
      legacyOutcome([&] { return text(legacy->getReceiverHWRevSupported()); }),
      cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetReceiverFirmwareVer) {
  expectSame(legacyOutcome([&] {
               return legacyString([&](char *b, size_t n) {
                 return legacy->getReceiverFirmwareVer(b, n);
               });
             }),
             cString([&](char *b, size_t n, size_t *r) {
               return JSR_GetReceiverFirmwareVer(cManager, b, n, r);
             }));
}

TEST_F(CApiParityTest, GetReceiverFirmwareVerSupported) {
  int32_t value = 0;
  const int32_t rc = JSR_GetReceiverFirmwareVerSupported(cManager, &value);
  expectSame(legacyOutcome([&] {
               return text(legacy->getReceiverFirmwareVerSupported());
             }),
             cOutcome(rc, [&] { return text(value, true); }));
}

TEST_F(CApiParityTest, GetReceiverOEMDataSupported) {
  int32_t value = 0;
  const int32_t rc = JSR_GetReceiverOEMDataSupported(cManager, &value);
  expectSame(legacyOutcome(
                 [&] { return text(legacy->getReceiverOEMDataSupported()); }),
             cOutcome(rc, [&] { return text(value, true); }));
}
