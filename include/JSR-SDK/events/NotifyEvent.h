#pragma once

#include "JSR-SDK/enums/C_DISCOVERY_STATE_FLAGS.h"
#include "JSR-SDK/enums/C_NOTIFY_TYPE.h"
#include "JSR-SDK/enums/C_PULSER_PROPERTY_DATA_TYPE.h"
#include "JSR-SDK/enums/C_PULSER_RECEIVER_STATE.h"
#include "JSR-SDK/enums/C_STATUS_CHANGE.h"
#include "JSR-SDK/structs/PulserReceiverID.h"
#include "JSR-SDK/structs/InstrumentID.h"

#include <functional>
#include <cstring>

/**
 * @struct NotifyEvent
 * @brief Event structure containing notification data from the JSR SDK Manager.
 *
 * This structure is used in notification callbacks to inform about various events
 * such as device discovery, property changes, errors, and state changes.
 * All string fields are fixed-size character arrays to ensure ABI stability.
 *
 * @note This is a POD (Plain Old Data) structure suitable for C interop.
 */
struct NotifyEvent {
  /// Maximum length for string fields (including null terminator)
  static constexpr size_t MAX_STRING_LENGTH = 256;

  /// Maximum number of info strings that can be stored
  static constexpr size_t MAX_INFO_ARRAY = 32;

  // === Device Identification ===

  /// Model name of the device (e.g., "JSR-DPR300")
  char model[MAX_STRING_LENGTH];

  /// Serial number of the device
  char serial[MAX_STRING_LENGTH];

  /// Plugin name responsible for this device
  char pluginName[MAX_STRING_LENGTH];

  /// Pulser/receiver index (0-based)
  int prIndex;

  /// Complete pulser/receiver identifier
  PulserReceiverID pulserReceiverId;

  /// Instrument identification information
  InstrumentID instrumentId;

  // === State and Event Type ===

  /// Current state of the pulser/receiver
  C_PULSER_RECEIVER_STATE state;

  /// Type of notification event (e.g., DEVICE_DISCOVERED, PROPERTY_CHANGE)
  C_NOTIFY_TYPE notifyType;

  /// Discovery state flags (for discovery events)
  C_DISCOVERY_STATE_FLAGS discoverState;

  /// Indicates if this device was selected by the user
  bool wasSelected;

  // === Property Change Information ===

  /// Name of the property that changed (for PROPERTY_CHANGE events)
  char propertyName[MAX_STRING_LENGTH];

  /// New value of the property as a string
  char newValue[MAX_STRING_LENGTH];

  /// Data type of the property that changed
  C_PULSER_PROPERTY_DATA_TYPE dataType;

  // === Device Capabilities ===

  /// Maximum operating frequency of the device (in Hz)
  double maxFrequency;

  /// Array of additional device information strings
  char info[MAX_INFO_ARRAY][MAX_STRING_LENGTH];

  /// Number of valid entries in the info array
  size_t infoCount;

  // === Error Information ===

  /// Type information about any exception that occurred
  char exceptionTypeInfo[MAX_STRING_LENGTH];

  /// Error message (if applicable)
  char errorMsg[MAX_STRING_LENGTH];

  /// Detailed error text (if applicable)
  char errorText[MAX_STRING_LENGTH];

  /**
   * @brief Default constructor - initializes all fields to safe defaults.
   *
   * All string fields are initialized to empty strings.
   * Numeric fields are initialized to zero.
   * Boolean fields are initialized to false.
   */
  NotifyEvent()
      : state{}, wasSelected(false), maxFrequency(0.0), infoCount(0),
        prIndex(0), discoverState{}, notifyType{}, dataType{} {
    model[0] = '\0';
    newValue[0] = '\0';
    propertyName[0] = '\0';
    exceptionTypeInfo[0] = '\0';
    errorMsg[0] = '\0';
    errorText[0] = '\0';
    serial[0] = '\0';
    pluginName[0] = '\0';
    info[0][0] = '\0';
  }
};

/**
 * @typedef NotifyCallback
 * @brief Callback function type for notification events.
 *
 * Use this to register a callback that will be invoked when notification events occur.
 * The callback receives a const reference to a NotifyEvent structure containing
 * all relevant event information.
 *
 * @param event The notification event data
 *
 * @example
 * @code
 * manager->replaceNotifyEventHandler([](const NotifyEvent& event) {
 *     std::cout << "Device: " << event.model << " - " << event.serial << std::endl;
 *     if (event.notifyType == C_NOTIFY_TYPE::PULSER_RCVR_DISCOVERED) {
 *         std::cout << "New device discovered!" << std::endl;
 *     }
 * });
 * @endcode
 */
using NotifyCallback = std::function<void(const NotifyEvent &)>;
