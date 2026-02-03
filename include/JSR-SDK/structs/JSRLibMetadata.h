#pragma once

#include "JSR-SDK/enums/C_CONNECTION_TYPE.h"
#include "JSR-SDK/boundary/CString.h"
#include "JSR-SDK/boundary/CVector.h"

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
  CString Name;

  /**
   * @brief The globally unique identifier (GUID) of the library.
   */
  CString GUID;

  /**
   * @brief A user-friendly name for the library.
   */
  CString FriendlyName;

  /**
   * @brief A list of supported connection types for the library.
   */
  CVector<C_CONNECTION_TYPE> ConnectionType;

  /**
   * @brief A list of models supported by the library.
   */
  CVector<CString> SupportedModels;

  /**
   * @brief A list of options available when opening the library.
   */
  CVector<CString> OpenOptions;

  /**
   * @brief The interface version of the library.
   */
  int InterfaceVersion;

  /**
   * @brief The version of the library.
   */
  CString Version;

  JSRLibMetadata() : InterfaceVersion(0) {}

  /**
   * @brief Destructor to free allocated memory.
   */
  ~JSRLibMetadata() {
    Name.free_cstring();
    GUID.free_cstring();
    FriendlyName.free_cstring();
    Version.free_cstring();
    ConnectionType.free_cvector();
    
    // Free each string in the vectors
    for (size_t i = 0; i < SupportedModels.size; ++i) {
      SupportedModels.data[i].free_cstring();
    }
    SupportedModels.free_cvector();
    
    for (size_t i = 0; i < OpenOptions.size; ++i) {
      OpenOptions.data[i].free_cstring();
    }
    OpenOptions.free_cvector();
  }

  JSRLibMetadata(const JSRLibMetadata &other) = delete;
  JSRLibMetadata &operator=(const JSRLibMetadata &other) = delete;

  JSRLibMetadata(JSRLibMetadata &&other) noexcept
      : Name(other.Name), GUID(other.GUID), FriendlyName(other.FriendlyName),
        ConnectionType(other.ConnectionType),
        SupportedModels(other.SupportedModels), OpenOptions(other.OpenOptions),
        InterfaceVersion(other.InterfaceVersion), Version(other.Version) {
    other.Name = CString();
    other.GUID = CString();
    other.FriendlyName = CString();
    other.ConnectionType = CVector<C_CONNECTION_TYPE>();
    other.SupportedModels = CVector<CString>();
    other.OpenOptions = CVector<CString>();
    other.Version = CString();
  }

  JSRLibMetadata &operator=(JSRLibMetadata &&other) noexcept {
    if (this != &other) {
      Name.free_cstring();
      GUID.free_cstring();
      FriendlyName.free_cstring();
      Version.free_cstring();
      ConnectionType.free_cvector();
      
      for (size_t i = 0; i < SupportedModels.size; ++i) {
        SupportedModels.data[i].free_cstring();
      }
      SupportedModels.free_cvector();
      
      for (size_t i = 0; i < OpenOptions.size; ++i) {
        OpenOptions.data[i].free_cstring();
      }
      OpenOptions.free_cvector();

      Name = other.Name;
      GUID = other.GUID;
      FriendlyName = other.FriendlyName;
      ConnectionType = other.ConnectionType;
      SupportedModels = other.SupportedModels;
      OpenOptions = other.OpenOptions;
      InterfaceVersion = other.InterfaceVersion;
      Version = other.Version;

      other.Name = CString();
      other.GUID = CString();
      other.FriendlyName = CString();
      other.ConnectionType = CVector<C_CONNECTION_TYPE>();
      other.SupportedModels = CVector<CString>();
      other.OpenOptions = CVector<CString>();
      other.Version = CString();
    }
    return *this;
  }
};
