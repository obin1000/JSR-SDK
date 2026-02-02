/**
 * @file StatusChangedEvent.h
 * @brief Status change event structure
 * 
 * C-compatible event structure for status change notifications.
 * Can be used from both C and C++ code safely across DLL boundaries.
 */

#pragma once

#include "JSR-SDK/enums/C_PULSER_PROPERTY_DATA_TYPE.h"
#include "JSR-SDK/enums/C_PULSER_RECEIVER_STATE.h"
#include "JSR-SDK/enums/C_STATUS_CHANGE.h"
#include "JSR-SDK/enums/C_ERROR_CODE.h"
#include "JSR-SDK/structs/ExceptionJSRSDK.h"
#include "JSR-SDK/structs/PulserReceiverID.h"
#include "JSR-SDK/JSRString.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Status changed event structure
 * 
 * Contains all information about a status change event.
 * Safe to use across DLL boundaries.
 */
typedef struct StatusChangedEvent {
    /** @brief Name of the pulser property that changed */
    JSRString PulserProperty;
    
    /** @brief Current state of the pulser/receiver */
    C_PULSER_RECEIVER_STATE PulserState;
    
    /** @brief New value of the changed property (as string) */
    JSRString NewValue;
    
    /** @brief Data type of the property */
    C_PULSER_PROPERTY_DATA_TYPE DataType;
    
    /** @brief Type of status change */
    C_STATUS_CHANGE ChangeType;
    
    /** @brief ID of the pulser/receiver that changed */
    PulserReceiverID PulserReceiverId;
    
    /** @brief Error message if applicable */
    JSRString ErrorMessage;
    
    /** @brief Error code if applicable */
    C_ERROR_CODE ErrorCode;
    
    /** @brief Exception details if one was thrown */
    ExceptionJSRSDK ThrownException;
} StatusChangedEvent;

/**
 * @brief Initialize a StatusChangedEvent to default values
 * @param evt Pointer to event structure
 */
static inline void StatusChangedEvent_Init(StatusChangedEvent* evt) {
    if (!evt) return;
    JSRString_Init(&evt->PulserProperty);
    evt->PulserState = C_PULSER_RECEIVER_STATE::DETECTED;
    JSRString_Init(&evt->NewValue);
    evt->DataType = C_PULSER_PROPERTY_DATA_TYPE::STRING;
    evt->ChangeType = C_STATUS_CHANGE::STATE_CHANGE;
    PulserReceiverID_Init(&evt->PulserReceiverId);
    JSRString_Init(&evt->ErrorMessage);
    evt->ErrorCode = C_ERROR_CODE::SUCCESS;
    ExceptionJSRSDK_Init(&evt->ThrownException);
}

#ifdef __cplusplus
}

// C++ callback type
#include <functional>
using StatusChangeCallback = std::function<void(const StatusChangedEvent&)>;

#endif // __cplusplus
