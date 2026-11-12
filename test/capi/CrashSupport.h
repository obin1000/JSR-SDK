// CrashSupport.h
// Helpers for the death tests of #36: a crash (a structured exception such
// as an access violation) inside a callback or a call must end the process
// where it happened, rather than be caught like an exception.
#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <cstdint>

namespace crash {

/// An address no program can write to. It is volatile, so the compiler
/// can't see the access is invalid and turn it into something else.
inline volatile std::uintptr_t badAddress = 8;

/// Writes to badAddress: an access violation.
inline void accessViolation() {
  *reinterpret_cast<volatile int *>(badAddress) = 1;
}

/// Turns off the Windows error dialog and report for the crash a death test
/// expects, so the test doesn't wait for someone to close it.
inline void quietly() {
  SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
}

} // namespace crash
