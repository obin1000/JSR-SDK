#pragma once

#include "JSR-SDK/enums/C_CONNECTION_TYPE.h"
#include <cstring>

/**
 * @class JSRLibMetadata
 * @brief Represents metadata information for the JSR library.
 *
 * This class contains various properties that describe the library's metadata,
 * including its name, GUID, supported connection types, models, and versioning details.
 * Mirrors JSRDotNETSDK::IJSRdotNETLibMetadata
 *
 * All strings are fixed-size buffers. Arrays use dynamic allocation but the caller
 * must manage the lifecycle.
 */
struct JSRLibMetadata {
  static constexpr size_t MAX_STRING_LENGTH = 256;
  static constexpr size_t MAX_ARRAY_SIZE = 64;

  char Name[MAX_STRING_LENGTH];
  char GUID[MAX_STRING_LENGTH];
  char FriendlyName[MAX_STRING_LENGTH];
  char Version[MAX_STRING_LENGTH];

  C_CONNECTION_TYPE ConnectionType[MAX_ARRAY_SIZE];
  size_t ConnectionTypeCount;

  char SupportedModels[MAX_ARRAY_SIZE][MAX_STRING_LENGTH];
  size_t SupportedModelsCount;

  char OpenOptions[MAX_ARRAY_SIZE][MAX_STRING_LENGTH];
  size_t OpenOptionsCount;

  int InterfaceVersion;

  JSRLibMetadata() : InterfaceVersion(0), ConnectionTypeCount(0), SupportedModelsCount(0), OpenOptionsCount(0) {
    Name[0] = '\0';
    GUID[0] = '\0';
    FriendlyName[0] = '\0';
    Version[0] = '\0';
  }
};
