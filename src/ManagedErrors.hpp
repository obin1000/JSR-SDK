#pragma once

// Turns managed (.NET) exceptions into native SdkError exceptions.
// C++/CLI only (/clr). Internal to the DLL; test/ManagedErrorsTest.cpp tests
// it.

#include "marshals/MarshalEnums.h"
#include "SdkError.hpp"

#include <msclr/marshal_cppstd.h>
#include <string>

namespace jsr::detail {

/**
 * @brief Picks the error code that best describes a managed exception.
 *
 * Exceptions from the JSR SDK carry their own code. For standard .NET
 * exceptions the closest code is used, and UNKNOWN for everything else.
 * The result is never SUCCESS: an exception always means a failure.
 */
inline C_ERROR_CODE errorCodeForManaged(System::Exception ^ exception) {
  if (auto sdkException =
          dynamic_cast<JSRDotNETSDK::ExceptionJSRDotNET ^>(exception)) {
    C_ERROR_CODE code = errorCodeFromManaged(sdkException->ErrorCode);
    return code == C_ERROR_CODE::SUCCESS ? C_ERROR_CODE::UNKNOWN : code;
  }
  if (dynamic_cast<System::ArgumentException ^>(exception)) {
    return C_ERROR_CODE::INVALID_ARGUMENT;
  }
  if (dynamic_cast<System::NullReferenceException ^>(exception)) {
    return C_ERROR_CODE::NULL_POINTER;
  }
  if (dynamic_cast<System::NotSupportedException ^>(exception) ||
      dynamic_cast<System::NotImplementedException ^>(exception)) {
    return C_ERROR_CODE::OPERATION_NOT_SUPPORTED;
  }
  if (dynamic_cast<System::TimeoutException ^>(exception)) {
    return C_ERROR_CODE::TIMEOUT;
  }
  return C_ERROR_CODE::UNKNOWN;
}

/**
 * @brief Builds the SdkError for a managed exception.
 *
 * The message keeps the "<context>: <managed message>" form that callers
 * already see, and the error code is preserved for the C API.
 */
inline SdkError sdkErrorFromManaged(System::Exception ^ exception,
                                    const char *context) {
  std::string message =
      msclr::interop::marshal_as<std::string>(exception->Message);
  return SdkError(errorCodeForManaged(exception),
                  std::string(context) + ": " + message);
}

/// Throws the SdkError for a managed exception.
[[noreturn]] inline void rethrowManaged(System::Exception ^ exception,
                                        const char *context) {
  throw sdkErrorFromManaged(exception, context);
}

} // namespace jsr::detail
