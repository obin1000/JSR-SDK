#pragma once

#include "JSR-SDK/enums/ConnectionType.h"

#include <string>
#include <vector>

/**
 * @class JSRLibMetadata
 * @brief Represents metadata information for the JSR library.
 *
 * This class contains various properties that describe the library's metadata,
 * including its name, GUID, supported connection types, models, and versioning details.
 * Mirrors JSRDotNETSDK::IJSRdotNETLibMetadata
 */
class JSRLibMetadata {
public:
  /**
   * @brief The name of the library.
   */
  std::string Name;

  /**
   * @brief The globally unique identifier (GUID) of the library.
   */
  std::string GUID;

  /**
   * @brief A user-friendly name for the library.
   */
  std::string FriendlyName;

  /**
   * @brief A list of supported connection types for the library.
   */
  std::vector<ConnectionType> ConnectionType;

  /**
   * @brief A list of models supported by the library.
   */
  std::vector<std::string> SupportedModels;

  /**
   * @brief A list of options available when opening the library.
   */
  std::vector<std::string> OpenOptions;

  /**
   * @brief The interface version of the library.
   */
  int InterfaceVersion;

  /**
   * @brief The version of the library.
   */
  std::string Version;
};
