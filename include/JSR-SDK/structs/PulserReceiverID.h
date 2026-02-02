/**
 * @file PulserReceiverID.h
 * @brief C-compatible Pulser/Receiver identification structure
 * 
 * Represents a Pulser/Receiver with an associated instrument and index.
 * Mirrors JSRDotNETSDK::PulserReceiverIdentity
 */

#pragma once

#include "InstrumentID.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Pulser/Receiver identification structure
 * 
 * Combines an instrument ID with a pulser/receiver index to uniquely
 * identify a specific pulser/receiver instance.
 * Safe to use across DLL boundaries.
 */
typedef struct PulserReceiverID {
    /** @brief The instrument associated with this Pulser/Receiver */
    InstrumentID InstrumentId;
    
    /** @brief The index of the Pulser/Receiver */
    int PulserReceiverIndex;
} PulserReceiverID;

/**
 * @brief Initialize a PulserReceiverID to default values
 * @param id Pointer to PulserReceiverID structure
 */
static inline void PulserReceiverID_Init(PulserReceiverID* id) {
    if (!id) return;
    InstrumentID_Init(&id->InstrumentId);
    id->PulserReceiverIndex = 0;
}

#ifdef __cplusplus
}

// C++ convenience functions
#include <string>
#include <sstream>

namespace JSR {

/**
 * @brief Get description string from PulserReceiverID (C++ only)
 * @param id The pulser/receiver ID
 * @return Formatted description string
 */
inline std::string GetDescription(const PulserReceiverID& id) {
    std::ostringstream oss;
    oss << "Pulser/Receiver: " << id.PulserReceiverIndex << "\n"
        << GetDescription(id.InstrumentId);
    return oss.str();
}

/**
 * @brief Convert PulserReceiverID to string (C++ only)
 * @param id The pulser/receiver ID
 * @return String representation
 */
inline std::string ToString(const PulserReceiverID& id) {
    return GetDescription(id);
}

} // namespace JSR

#endif // __cplusplus
