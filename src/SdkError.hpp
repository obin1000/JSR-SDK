#pragma once

#include "JSR-SDK/enums/C_ERROR_CODE.h"

#include <stdexcept>
#include <string>

namespace jsr::detail {

/**
 * @brief Native exception thrown by the manager when the .NET SDK fails.
 *
 * It derives from std::runtime_error, so existing callers that catch
 * std::runtime_error or std::exception keep working. It also carries the
 * SDK's error code, which a plain std::runtime_error would lose.
 *
 * This type is internal to the DLL: it is not part of the public headers.
 */
class SdkError : public std::runtime_error {
public:
  SdkError(C_ERROR_CODE code, const std::string &message)
      : std::runtime_error(message), m_code(code) {}

  C_ERROR_CODE code() const noexcept { return m_code; }

private:
  C_ERROR_CODE m_code;
};

} // namespace jsr::detail
