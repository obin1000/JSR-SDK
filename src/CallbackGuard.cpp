// Native code with /EHsc (see JSR-SDK.vcxproj): catch (...) here stops C++
// exceptions only, and a crash goes on to end the process. See
// CallbackGuard.hpp.
#include "CallbackGuard.hpp"

namespace jsr::detail {

void invokeCallback(const StatusChangeCallback &callback,
                    const StatusChangedEvent &event) {
  try {
    callback(event);
  } catch (...) {
  }
}

void invokeCallback(const NotifyCallback &callback, const NotifyEvent &event) {
  try {
    callback(event);
  } catch (...) {
  }
}

} // namespace jsr::detail
