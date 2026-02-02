/**
 * @file NotifyEvent.h
 * @brief Notify event structure
 * 
 * C-compatible event structure for notifications.
 * Can be used from both C and C++ code safely across DLL boundaries.
 */

#pragma once

#include "JSR-SDK/enums/C_DISCOVERY_STATE_FLAGS.h"
#include "JSR-SDK/enums/C_NOTIFY_TYPE.h"
#include "JSR-SDK/enums/C_PULSER_PROPERTY_DATA_TYPE.h"
#include "JSR-SDK/enums/C_PULSER_RECEIVER_STATE.h"
#include "JSR-SDK/structs/PulserReceiverID.h"
#include "JSR-SDK/structs/InstrumentID.h"
#include "JSR-SDK/JSRString.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Maximum number of info strings in a notify event
 */
#define JSR_NOTIFY_INFO_MAX_COUNT 32

/**
 * @brief Notify event structure
 * 
 * Contains all information about a notify event.
 * Safe to use across DLL boundaries.
 */
typedef struct {
    /** @brief Model name of the device */
    JSRString Model;
    
    /** @brief Current state of the pulser/receiver */
    C_PULSER_RECEIVER_STATE State;
    
    /** @brief New value (as string) */
    JSRString NewValue;
    
    /** @brief Name of the property that changed */
    JSRString PropertyName;
    
    /** @brief Whether the device was previously selected */
    int WasSelected;  // bool as int for C compatibility
    
    /** @brief Type information about any exception */
    JSRString ExceptionTypeInfo;
    
    /** @brief Error message */
    JSRString ErrorMsg;
    
    /** @brief Error text description */
    JSRString ErrorText;
    
    /** @brief Maximum frequency of the device */
    double MaxFrequency;
    
    /** @brief Array of informational strings */
    JSRString Info[JSR_NOTIFY_INFO_MAX_COUNT];
    
    /** @brief Number of valid entries in the info array */
    int InfoCount;
    
    /** @brief Pulser/receiver index */
    int PrIndex;
    
    /** @brief Pulser/receiver ID */
    PulserReceiverID PulserReceiverId;
    
    /** @brief Serial number */
    JSRString Serial;
    
    /** @brief Discovery state flags */
    C_DISCOVERY_STATE_FLAGS DiscoverState;
    
    /** @brief Instrument ID */
    InstrumentID InstrumentId;
    
    /** @brief Plugin name */
    JSRString PluginName;
    
    /** @brief Type of notification */
    C_NOTIFY_TYPE NotifyType;
    
    /** @brief Data type of the property */
    C_PULSER_PROPERTY_DATA_TYPE DataType;
} NotifyEvent;

/**
 * @brief Initialize a NotifyEvent to default values
 * @param evt Pointer to event structure
 */
static inline void NotifyEvent_Init(NotifyEvent* evt) {
    if (!evt) return;
    JSRString_Init(&evt->Model);
    evt->State = C_PULSER_RECEIVER_STATE::DETECTED;
    JSRString_Init(&evt->NewValue);
    JSRString_Init(&evt->PropertyName);
    evt->WasSelected = 0;
    JSRString_Init(&evt->ExceptionTypeInfo);
    JSRString_Init(&evt->ErrorMsg);
    JSRString_Init(&evt->ErrorText);
    evt->MaxFrequency = 0.0;
    evt->InfoCount = 0;
    evt->PrIndex = 0;
    PulserReceiverID_Init(&evt->PulserReceiverId);
    JSRString_Init(&evt->Serial);
    evt->DiscoverState = C_DISCOVERY_STATE_FLAGS::NONE;
    InstrumentID_Init(&evt->InstrumentId);
    JSRString_Init(&evt->PluginName);
    evt->NotifyType = C_NOTIFY_TYPE::ERROR;
    evt->DataType = C_PULSER_PROPERTY_DATA_TYPE::STRING;
}

#ifdef __cplusplus
}

// C++ callback type
#include <functional>
using NotifyCallback = std::function<void(const NotifyEvent&)>;

#endif // __cplusplus
