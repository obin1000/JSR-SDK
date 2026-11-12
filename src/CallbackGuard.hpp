#pragma once

#include "types/events/NotifyEvent.h"
#include "types/events/StatusChangedEvent.h"

namespace jsr::detail {

/**
 * @brief Calls a user callback and drops any C++ exception it throws.
 *
 * The SDK raises events inside its own calls, so an exception from a
 * callback would make that call fail; it is dropped instead.
 *
 * A crash in the callback (a structured exception, such as an access
 * violation) is not caught: it ends the process where it happened (#36).
 * That is why this is a separate native file compiled with /EHsc. In /clr
 * code, which always uses /EHa, catch (...) would catch the crash too.
 */
void invokeCallback(const StatusChangeCallback &callback,
                    const StatusChangedEvent &event);

/// The same, for notify events.
void invokeCallback(const NotifyCallback &callback, const NotifyEvent &event);

} // namespace jsr::detail
