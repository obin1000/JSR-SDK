#pragma once

#include "JSR-SDK/enums/C_CONNECTION_TYPE.h"
#include <cstring>

/**
 * @struct JSRLibMetadata
 * @brief Contains metadata information about a JSR plugin library.
 *
 * This structure provides comprehensive information about a plugin library,
 * including identification, capabilities, supported devices, and version information.
 * This data is typically used for plugin discovery, validation, and compatibility checking.
 * 
 * Mirrors JSRDotNETSDK::IJSRDotNETLibMetadata interface.
 *
 * All strings are fixed-size buffers to ensure ABI stability. Arrays have a maximum
 * size with explicit count fields to track actual usage.
 * 
 * @note This is a POD (Plain Old Data) structure suitable for C interop.
 */
struct JSRLibMetadata {
  /// Maximum length for string fields (including null terminator)
  static constexpr size_t MAX_STRING_LENGTH = 256;
  
  /// Maximum number of elements in array fields
  static constexpr size_t MAX_ARRAY_SIZE = 64;

  // === Library Identification ===
  
  /// Internal name of the library (e.g., "JSR-DPR")
  char Name[MAX_STRING_LENGTH];
  
  /// Globally Unique Identifier for the library
  char GUID[MAX_STRING_LENGTH];
  
  /// User-friendly display name (e.g., "JSR Digital Pulser/Receiver Plugin")
  char FriendlyName[MAX_STRING_LENGTH];
  
  /// Library version string (e.g., "2.1.0", "1.0.0-beta")
  char Version[MAX_STRING_LENGTH];
  
  /// Interface version number for compatibility checking
  int InterfaceVersion;

  // === Supported Connection Types ===
  
  /// Array of connection types this plugin supports (USB, ETHERNET, etc.)
  C_CONNECTION_TYPE ConnectionType[MAX_ARRAY_SIZE];
  
  /// Number of valid entries in ConnectionType array
  size_t ConnectionTypeCount;

  // === Supported Device Models ===
  
  /// Array of device model names this plugin can control
  /// (e.g., "JSR-DPR300", "HB3-1", "HB4")
  char SupportedModels[MAX_ARRAY_SIZE][MAX_STRING_LENGTH];
  
  /// Number of valid entries in SupportedModels array
  size_t SupportedModelsCount;

  // === Configuration Options ===
  
  /// Array of available open/configuration option names
  /// These are plugin-specific settings that can be configured
  char OpenOptions[MAX_ARRAY_SIZE][MAX_STRING_LENGTH];
  
  /// Number of valid entries in OpenOptions array
  size_t OpenOptionsCount;

  /**
   * @brief Default constructor - initializes all fields to safe defaults.
   * 
   * All string fields are initialized to empty strings.
   * All array counts are initialized to zero.
   * InterfaceVersion is initialized to 0.
   */
  JSRLibMetadata() : InterfaceVersion(0), ConnectionTypeCount(0), SupportedModelsCount(0), OpenOptionsCount(0) {
    Name[0] = '\0';
    GUID[0] = '\0';
    FriendlyName[0] = '\0';
    Version[0] = '\0';
  }
};
