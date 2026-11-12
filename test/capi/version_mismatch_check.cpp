// version_mismatch_check.cpp
// #20: a program built with the headers of another API version than the
// loaded JSR-SDK.dll implements must get a clear error, not undefined
// behaviour. This program pretends its headers are API version 999;
// jsr::Manager must refuse with JSR_ERROR_VERSION_MISMATCH.
#include "JSR-SDK/jsr_sdk.h"

#undef JSR_API_VERSION
#define JSR_API_VERSION 999

#include "JSR-SDK/jsr_sdk.hpp"

#include <cstdio>
#include <string>

int main() {
  try {
    jsr::Manager manager;
    std::fprintf(stderr,
                 "FAILED: jsr::Manager accepted a DLL of API version %d with "
                 "headers of version %d\n",
                 static_cast<int>(jsr::apiVersion()), JSR_API_VERSION);
    return 1;
  } catch (const jsr::Error &e) {
    std::printf("jsr::Manager refused: %s\n", e.what());
    const std::string message = e.what();
    if (e.code() != JSR_ERROR_VERSION_MISMATCH ||
        message.find("999") == std::string::npos) {
      std::fprintf(stderr,
                   "FAILED: expected JSR_ERROR_VERSION_MISMATCH (%d) "
                   "naming version 999, got %d\n",
                   JSR_ERROR_VERSION_MISMATCH, static_cast<int>(e.code()));
      return 1;
    }
    return 0;
  }
}
