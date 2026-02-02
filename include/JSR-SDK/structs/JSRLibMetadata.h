/**
 * @file JSRLibMetadata.h
 * @brief C-compatible library metadata structure
 * 
 * Contains metadata information for the JSR library including name, version,
 * supported models, and connection types.
 * Mirrors JSRDotNETSDK::IJSRdotNETLibMetadata
 */

#pragma once

#include "JSR-SDK/enums/C_CONNECTION_TYPE.h"
#include "JSR-SDK/JSRString.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Maximum number of supported models in metadata
 */
#define JSR_METADATA_MAX_MODELS 32

/**
 * @brief Maximum number of open options in metadata
 */
#define JSR_METADATA_MAX_OPTIONS 32

/**
 * @brief Maximum number of connection types in metadata
 */
#define JSR_METADATA_MAX_CONNECTIONS 8

/**
 * @brief Library metadata structure
 * 
 * Contains comprehensive information about a JSR library plugin.
 * Safe to use across DLL boundaries.
 */
typedef struct JSRLibMetadata {
    /** @brief The name of the library */
    JSRString Name;
    
    /** @brief The globally unique identifier (GUID) of the library */
    JSRString GUID;
    
    /** @brief A user-friendly name for the library */
    JSRString FriendlyName;
    
    /** @brief The version of the library */
    JSRString Version;
    
    /** @brief Array of supported connection types */
    C_CONNECTION_TYPE ConnectionType[JSR_METADATA_MAX_CONNECTIONS];
    
    /** @brief Number of valid connection types in the array */
    int ConnectionTypeCount;
    
    /** @brief Array of supported model names */
    JSRString SupportedModels[JSR_METADATA_MAX_MODELS];
    
    /** @brief Number of valid models in the array */
    int SupportedModelsCount;
    
    /** @brief Array of open option names */
    JSRString OpenOptions[JSR_METADATA_MAX_OPTIONS];
    
    /** @brief Number of valid open options in the array */
    int OpenOptionsCount;
    
    /** @brief The interface version of the library */
    int InterfaceVersion;
} JSRLibMetadata;

/**
 * @brief Initialize library metadata to default values
 * @param metadata Pointer to metadata structure
 */
static inline void JSRLibMetadata_Init(JSRLibMetadata* metadata) {
    if (!metadata) return;
    JSRString_Init(&metadata->Name);
    JSRString_Init(&metadata->GUID);
    JSRString_Init(&metadata->FriendlyName);
    JSRString_Init(&metadata->Version);
    metadata->ConnectionTypeCount = 0;
    metadata->SupportedModelsCount = 0;
    metadata->OpenOptionsCount = 0;
    metadata->InterfaceVersion = 0;
}

#ifdef __cplusplus
}

// C++ convenience functions
#include <string>
#include <vector>

namespace JSR {

/**
 * @brief Get connection types as vector (C++ only)
 */
inline std::vector<C_CONNECTION_TYPE> GetConnectionTypes(const JSRLibMetadata& metadata) {
    return std::vector<C_CONNECTION_TYPE>(
        metadata.ConnectionType,
        metadata.ConnectionType + metadata.ConnectionTypeCount
    );
}

/**
 * @brief Get supported models as vector (C++ only)
 */
inline std::vector<std::string> GetSupportedModels(const JSRLibMetadata& metadata) {
    std::vector<std::string> models;
    models.reserve(metadata.SupportedModelsCount);
    for (int i = 0; i < metadata.SupportedModelsCount; ++i) {
        models.push_back(metadata.SupportedModels[i].data);
    }
    return models;
}

/**
 * @brief Get open options as vector (C++ only)
 */
inline std::vector<std::string> GetOpenOptions(const JSRLibMetadata& metadata) {
    std::vector<std::string> options;
    options.reserve(metadata.OpenOptionsCount);
    for (int i = 0; i < metadata.OpenOptionsCount; ++i) {
        options.push_back(metadata.OpenOptions[i].data);
    }
    return options;
}

} // namespace JSR

#endif // __cplusplus
