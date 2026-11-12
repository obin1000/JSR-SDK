// CApiTest.cpp
// Portable: every compiler in CI builds and runs it (see test/capi).
// Tests for the C API in jsr_sdk.h and the C++ wrapper in jsr_sdk.hpp.
// They need no hardware: only the plugins next to the test program.
#include <gtest/gtest.h>

#include "CrashSupport.h"
#include "JSR-SDK/jsr_sdk.h"
#include "JSR-SDK/jsr_sdk.hpp"

#include <cstring>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace {

std::string lastError() {
  std::vector<char> buffer(JSR_GetLastErrorMessage(nullptr, 0));
  JSR_GetLastErrorMessage(buffer.data(), buffer.size());
  return buffer.data();
}

std::vector<std::string> pluginNames(JSR_Manager *manager) {
  size_t count = 0;
  EXPECT_EQ(JSR_GetPluginNames(manager, nullptr, 0, 0, &count), JSR_OK);
  std::vector<std::vector<char>> storage(count,
                                         std::vector<char>(JSR_STRING_LENGTH));
  std::vector<char *> buffers;
  for (auto &s : storage) {
    buffers.push_back(s.data());
  }
  EXPECT_EQ(JSR_GetPluginNames(manager, buffers.data(), buffers.size(),
                               JSR_STRING_LENGTH, &count),
            JSR_OK);
  return std::vector<std::string>(buffers.begin(), buffers.end());
}

class CApiTest : public ::testing::Test {
protected:
  JSR_Manager *manager = nullptr;

  void SetUp() override {
    ASSERT_EQ(JSR_Create(&manager), JSR_OK) << lastError();
    ASSERT_NE(manager, nullptr);
    ASSERT_EQ(JSR_LoadPluginsFromBinaryDir(manager), JSR_OK) << lastError();
  }

  void TearDown() override { EXPECT_EQ(JSR_Destroy(manager), JSR_OK); }
};

} // namespace

//--------------------------------------------------------------------------
//  Errors without a manager
//--------------------------------------------------------------------------
TEST(CApiErrors, NullManager_ReturnsNullPointer) {
  int32_t value = 0;
  EXPECT_EQ(JSR_GetGainIndex(nullptr, &value), JSR_ERROR_NULL_POINTER);
  EXPECT_NE(lastError().find("manager"), std::string::npos);
}

TEST(CApiErrors, CreateWithoutOutPointer_ReturnsNullPointer) {
  EXPECT_EQ(JSR_Create(nullptr), JSR_ERROR_NULL_POINTER);
}

TEST(CApiErrors, DestroyNull_IsOk) { EXPECT_EQ(JSR_Destroy(nullptr), JSR_OK); }

TEST(CApiErrors, LastErrorMessage_TruncatesAndReportsFullSize) {
  JSR_GetGainIndex(nullptr, nullptr);
  const size_t full = JSR_GetLastErrorMessage(nullptr, 0);
  char tiny[5] = "xxxx";
  EXPECT_EQ(JSR_GetLastErrorMessage(tiny, sizeof tiny), full);
  EXPECT_GT(full, sizeof tiny);
  EXPECT_EQ(std::strlen(tiny), sizeof tiny - 1);
}

TEST(CApiErrors, LastErrorMessage_IsPerThread) {
  JSR_GetGainIndex(nullptr, nullptr);
  ASSERT_FALSE(lastError().empty());

  std::string otherThread = "not run";
  std::thread([&] { otherThread = lastError(); }).join();
  EXPECT_EQ(otherThread, "");
}

//--------------------------------------------------------------------------
//  Crashes inside a call (#36)
//--------------------------------------------------------------------------
#if GTEST_HAS_DEATH_TEST
TEST(CApiDeathTest, CrashInsideACall_EndsTheProcess) {
  // A crash inside the library, here from a bad buffer the caller passed,
  // must end the process. It used to be turned into JSR_ERROR_UNKNOWN, and
  // the program carried on in an unknown state. The call runs on its own
  // thread because googletest catches crashes on the test thread, which
  // would hide the exit code.
  const int accessViolation = static_cast<int>(0xC0000005);
  EXPECT_EXIT(
      {
        crash::quietly();
        std::thread([] {
          char *const bad = reinterpret_cast<char *>(crash::badAddress);
          JSR_GetLibraryVersion(bad, 64, nullptr);
        }).join();
      },
      ::testing::ExitedWithCode(accessViolation), "");
}
#endif

//--------------------------------------------------------------------------
//  Versions (#20)
//--------------------------------------------------------------------------
TEST(CApiVersion, ApiVersion_MatchesHeader) {
  int32_t version = -1;
  ASSERT_EQ(JSR_GetApiVersion(&version), JSR_OK) << lastError();
  EXPECT_EQ(version, JSR_API_VERSION);
  EXPECT_EQ(JSR_GetApiVersion(nullptr), JSR_ERROR_NULL_POINTER);
}

TEST(CApiVersion, LibraryVersion_QueryThenReadAndTruncate) {
  size_t required = 0;
  ASSERT_EQ(JSR_GetLibraryVersion(nullptr, 0, &required), JSR_OK);
  ASSERT_GT(required, 1u);

  std::vector<char> buffer(required, 'x');
  ASSERT_EQ(JSR_GetLibraryVersion(buffer.data(), buffer.size(), nullptr),
            JSR_OK);
  const std::string version = buffer.data();
  EXPECT_EQ(version.size(), required - 1);
#ifdef JSR_SDK_EXPECTED_VERSION
  EXPECT_EQ(version, JSR_SDK_EXPECTED_VERSION);
#endif

  char tiny[3] = "xx";
  size_t again = 0;
  ASSERT_EQ(JSR_GetLibraryVersion(tiny, sizeof tiny, &again), JSR_OK);
  EXPECT_EQ(again, required);
  EXPECT_EQ(std::string(tiny), version.substr(0, 2));
}

TEST(CppWrapper, Versions_MatchTheCApi) {
  EXPECT_EQ(jsr::apiVersion(), JSR_API_VERSION);
  std::vector<char> buffer(64);
  ASSERT_EQ(JSR_GetLibraryVersion(buffer.data(), buffer.size(), nullptr),
            JSR_OK);
  EXPECT_EQ(jsr::libraryVersion(), buffer.data());
}

TEST(CppWrapper, OtherApiVersion_ThrowsVersionMismatch) {
  EXPECT_NO_THROW(jsr::detail::checkApiVersion(JSR_API_VERSION, "1.2.3"));
  try {
    jsr::detail::checkApiVersion(JSR_API_VERSION + 1, "1.2.3");
    FAIL() << "no exception";
  } catch (const jsr::Error &e) {
    EXPECT_EQ(e.code(), JSR_ERROR_VERSION_MISMATCH);
    const std::string message = e.what();
    EXPECT_NE(message.find("1.2.3"), std::string::npos) << message;
    EXPECT_NE(message.find(std::to_string(JSR_API_VERSION + 1)),
              std::string::npos)
        << message;
  }
}

//--------------------------------------------------------------------------
//  Calls on a manager with the plugins loaded
//--------------------------------------------------------------------------
TEST_F(CApiTest, PluginsLoaded_IsTrue) {
  int32_t loaded = 0;
  ASSERT_EQ(JSR_GetPluginsLoaded(manager, &loaded), JSR_OK) << lastError();
  EXPECT_EQ(loaded, 1);
}

TEST_F(CApiTest, NullOutPointer_ReturnsNullPointer) {
  EXPECT_EQ(JSR_GetPluginsLoaded(manager, nullptr), JSR_ERROR_NULL_POINTER);
  EXPECT_NE(lastError().find("value"), std::string::npos);
}

TEST_F(CApiTest, NullStringArgument_ReturnsNullPointer) {
  EXPECT_EQ(JSR_LoadPlugins(manager, nullptr), JSR_ERROR_NULL_POINTER);
  EXPECT_NE(lastError().find("pluginPath"), std::string::npos);
}

TEST_F(CApiTest, PluginNames_QueryThenRead) {
  const auto names = pluginNames(manager);
  ASSERT_FALSE(names.empty());
  for (const auto &name : names) {
    EXPECT_FALSE(name.empty());
  }
}

TEST_F(CApiTest, PluginNames_ShortListReportsFullCount) {
  const auto names = pluginNames(manager);
  ASSERT_GE(names.size(), 2u);

  char first[JSR_STRING_LENGTH] = "";
  char *buffers[] = {first};
  size_t count = 0;
  ASSERT_EQ(JSR_GetPluginNames(manager, buffers, 1, JSR_STRING_LENGTH, &count),
            JSR_OK);
  EXPECT_EQ(count, names.size());
  EXPECT_EQ(std::string(first), names[0]);
}

TEST_F(CApiTest, StringResult_QueryThenReadAndTruncate) {
  size_t required = 0;
  ASSERT_EQ(JSR_GetPluginPath(manager, nullptr, 0, &required), JSR_OK);
  ASSERT_GT(required, 4u);

  std::vector<char> path(required);
  size_t again = 0;
  ASSERT_EQ(JSR_GetPluginPath(manager, path.data(), path.size(), &again),
            JSR_OK);
  EXPECT_EQ(again, required);
  EXPECT_EQ(std::strlen(path.data()) + 1, required);

  char tiny[4];
  ASSERT_EQ(JSR_GetPluginPath(manager, tiny, sizeof tiny, &again), JSR_OK);
  EXPECT_EQ(again, required);
  EXPECT_EQ(std::strncmp(tiny, path.data(), 3), 0);
  EXPECT_EQ(tiny[3], '\0');
}

TEST_F(CApiTest, SdkException_BecomesCodeAndMessage) {
  const int32_t rc = JSR_SetCurrentPulserReceiverByModel(
      manager, "NO-SUCH-MODEL", "NO-SUCH-SERIAL", 0);
  EXPECT_EQ(rc, JSR_ERROR_NOT_FOUND);
  EXPECT_NE(lastError().find("NO-SUCH-MODEL"), std::string::npos);
}

TEST_F(CApiTest, GetIdWithoutSelection_ReportsNoPulserSelected) {
  // It used to fail with "Object reference not set to an instance of an
  // object" (#17).
  JSR_PulserReceiverID id{};
  EXPECT_EQ(JSR_GetId(manager, &id), JSR_ERROR_NO_PULSER_RCVR_SELECTED);
  EXPECT_NE(lastError().find("No pulser-receiver is selected"),
            std::string::npos);
}

TEST_F(CApiTest, Callbacks_CanBeSetAndRemoved) {
  auto onStatus = [](const JSR_StatusChangedEvent *, void *) {};
  auto onNotify = [](const JSR_NotifyEvent *, void *) {};
  EXPECT_EQ(JSR_SetStatusChangeCallback(manager, onStatus, nullptr), JSR_OK);
  EXPECT_EQ(JSR_SetNotifyCallback(manager, onNotify, nullptr), JSR_OK);
  EXPECT_EQ(JSR_SetStatusChangeCallback(manager, nullptr, nullptr), JSR_OK);
  EXPECT_EQ(JSR_SetNotifyCallback(manager, nullptr, nullptr), JSR_OK);
}

//--------------------------------------------------------------------------
//  C++ wrapper
//--------------------------------------------------------------------------
TEST(CppWrapper, ReturnsStringsAndVectors) {
  jsr::Manager manager;
  manager.loadPluginsFromBinaryDir();
  const auto names = manager.GetPluginNames();
  ASSERT_FALSE(names.empty());
  EXPECT_FALSE(manager.getPluginPath().empty());
  EXPECT_STREQ(manager.GetPluginLibraryMetadata(names[0])->Name,
               names[0].c_str());
}

TEST(CppWrapper, Failure_ThrowsErrorWithCode) {
  jsr::Manager manager;
  manager.loadPluginsFromBinaryDir();
  try {
    manager.SetCurrentPulserReceiver("NO-SUCH-MODEL", "NO-SUCH-SERIAL", 0);
    FAIL() << "expected jsr::Error";
  } catch (const jsr::Error &e) {
    EXPECT_EQ(e.code(), JSR_ERROR_NOT_FOUND);
    EXPECT_NE(std::string(e.what()).find("NO-SUCH-MODEL"), std::string::npos);
  }
}

TEST(CppWrapper, Move_TransfersOwnership) {
  jsr::Manager first;
  JSR_Manager *handle = first.handle();
  jsr::Manager second = std::move(first);
  EXPECT_EQ(second.handle(), handle);
  EXPECT_EQ(first.handle(), nullptr); // NOLINT(bugprone-use-after-move)
}
