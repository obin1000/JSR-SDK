// ManagedErrorsTest.cpp
// Tests for turning .NET exceptions into SdkError (src/ManagedErrors.hpp).
#include <gtest/gtest.h>

#include "ManagedErrors.hpp"

#include <stdexcept>
#include <string>

using namespace System;
using namespace JSRDotNETSDK;
using jsr::detail::errorCodeForManaged;
using jsr::detail::rethrowManaged;
using jsr::detail::SdkError;
using jsr::detail::sdkErrorFromManaged;

namespace {

void expectSdkCode(ERROR_CODE managed, C_ERROR_CODE native) {
  EXPECT_EQ(errorCodeForManaged(gcnew ExceptionJSRDotNET(managed, "msg")),
            native)
      << "managed code " << static_cast<int>(managed);
}

} // namespace

//--------------------------------------------------------------------------
//  Error codes
//--------------------------------------------------------------------------
TEST(ManagedErrors, SdkException_KeepsItsCode) {
  expectSdkCode(ERROR_CODE::DEVICE_NOT_FOUND, C_ERROR_CODE::DEVICE_NOT_FOUND);
  expectSdkCode(ERROR_CODE::NOT_FOUND, C_ERROR_CODE::NOT_FOUND);
  expectSdkCode(ERROR_CODE::TIMEOUT, C_ERROR_CODE::TIMEOUT);
  expectSdkCode(ERROR_CODE::VALUE_OUT_RANGE, C_ERROR_CODE::VALUE_OUT_RANGE);
  expectSdkCode(ERROR_CODE::HARDWARE_FAILURE, C_ERROR_CODE::HARDWARE_FAILURE);
  expectSdkCode(ERROR_CODE::NO_PULSER_RCVR_SELECTED,
                C_ERROR_CODE::NO_PULSER_RCVR_SELECTED);
  expectSdkCode(ERROR_CODE::UNKNOWN, C_ERROR_CODE::UNKNOWN);
}

TEST(ManagedErrors, SdkExceptionWithSuccessCode_BecomesUnknown) {
  // An exception always means failure, whatever code it carries.
  expectSdkCode(ERROR_CODE::SUCCESS, C_ERROR_CODE::UNKNOWN);
}

TEST(ManagedErrors, ArgumentExceptions_AreInvalidArgument) {
  EXPECT_EQ(errorCodeForManaged(gcnew ArgumentException("x")),
            C_ERROR_CODE::INVALID_ARGUMENT);
  EXPECT_EQ(errorCodeForManaged(gcnew ArgumentNullException("x")),
            C_ERROR_CODE::INVALID_ARGUMENT);
  EXPECT_EQ(errorCodeForManaged(gcnew ArgumentOutOfRangeException("x")),
            C_ERROR_CODE::INVALID_ARGUMENT);
}

TEST(ManagedErrors, NullReference_IsNullPointer) {
  EXPECT_EQ(errorCodeForManaged(gcnew NullReferenceException()),
            C_ERROR_CODE::NULL_POINTER);
}

TEST(ManagedErrors, NotSupportedAndNotImplemented_AreOperationNotSupported) {
  EXPECT_EQ(errorCodeForManaged(gcnew NotSupportedException()),
            C_ERROR_CODE::OPERATION_NOT_SUPPORTED);
  EXPECT_EQ(errorCodeForManaged(gcnew NotImplementedException()),
            C_ERROR_CODE::OPERATION_NOT_SUPPORTED);
}

TEST(ManagedErrors, Timeout_IsTimeout) {
  EXPECT_EQ(errorCodeForManaged(gcnew TimeoutException()),
            C_ERROR_CODE::TIMEOUT);
}

TEST(ManagedErrors, OtherExceptions_AreUnknown) {
  EXPECT_EQ(errorCodeForManaged(gcnew Exception("x")), C_ERROR_CODE::UNKNOWN);
  EXPECT_EQ(errorCodeForManaged(gcnew InvalidOperationException("x")),
            C_ERROR_CODE::UNKNOWN);
  EXPECT_EQ(errorCodeForManaged(gcnew IO::IOException("x")),
            C_ERROR_CODE::UNKNOWN);
}

//--------------------------------------------------------------------------
//  Messages and exception types
//--------------------------------------------------------------------------
TEST(ManagedErrors, SdkError_HasContextAndManagedMessage) {
  SdkError error = sdkErrorFromManaged(
      gcnew ExceptionJSRDotNET(ERROR_CODE::TIMEOUT, "device timed out"),
      "Reading gain");
  EXPECT_STREQ(error.what(), "Reading gain: device timed out");
  EXPECT_EQ(error.code(), C_ERROR_CODE::TIMEOUT);
}

TEST(ManagedErrors, RethrowManaged_ThrowsSdkErrorAsRuntimeError) {
  // Existing callers catch std::runtime_error; that must keep working.
  EXPECT_THROW(rethrowManaged(gcnew InvalidOperationException("boom"), "Ctx"),
               std::runtime_error);

  try {
    rethrowManaged(gcnew ArgumentException("bad value"), "Ctx");
    FAIL() << "expected SdkError";
  } catch (const SdkError &e) {
    EXPECT_EQ(e.code(), C_ERROR_CODE::INVALID_ARGUMENT);
    EXPECT_STREQ(e.what(), "Ctx: bad value");
  }
}
