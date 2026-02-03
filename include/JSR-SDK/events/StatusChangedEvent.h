#pragma once

#include "JSR-SDK/enums/C_PULSER_PROPERTY_DATA_TYPE.h"
#include "JSR-SDK/enums/C_PULSER_RECEIVER_STATE.h"
#include "JSR-SDK/enums/C_STATUS_CHANGE.h"
#include "JSR-SDK/enums/C_ERROR_CODE.h"

#include "JSR-SDK/structs/ExceptionJSRSDK.h"
#include "JSR-SDK/structs/PulserReceiverID.h"

#include <functional>
#include <string>
#include <cstring>

/**
 * @struct StatusChangedEvent
 * @brief Event structure containing status change data from the JSR SDK Manager.
 * 
 * This structure is used in status change callbacks to inform about changes to
 * pulser/receiver properties, state transitions, and errors that occur during
 * device operations.
 * 
 * All string fields are fixed-size character arrays to ensure ABI stability
 * and avoid dynamic memory allocation.
 * 
 * @note This is a POD (Plain Old Data) structure suitable for C interop.
 */
struct StatusChangedEvent {
  /// Maximum length for string fields (including null terminator)
  static constexpr size_t MAX_STRING_LENGTH = 256;

  // === Property Change Information ===
  
  /// Name of the pulser property that changed (e.g., "Gain", "PRF", "HVSupply")
  char pulserProperty[MAX_STRING_LENGTH];
  
  /// New value of the property as a string representation
  char newValue[MAX_STRING_LENGTH];
  
  /// Data type of the property that changed
  C_PULSER_PROPERTY_DATA_TYPE dataType;

  // === State Information ===
  
  /// Current state of the pulser/receiver after the change
  C_PULSER_RECEIVER_STATE pulserState;
  
  /// Type of status change that occurred
  C_STATUS_CHANGE changeType;
  
  /// Identifier of the pulser/receiver that changed
  PulserReceiverID pulserReceiverId;

  // === Error Information ===
  
  /// Error message if the status change was due to an error
  char errorMessage[MAX_STRING_LENGTH];
  
  /// Error code indicating the type of error (if any)
  C_ERROR_CODE errorCode;
  
  /// Detailed exception information if an exception was thrown
  ExceptionJSRSDK thrownException;

  /**
   * @brief Default constructor - initializes all fields to safe defaults.
   * 
   * All string fields are initialized to empty strings.
   * Enums are value-initialized to their default values.
   * The exception object is default-constructed.
   */
  StatusChangedEvent()
      : pulserState{}, dataType{}, changeType{}, errorCode{} {
    pulserProperty[0] = '\0';
    newValue[0] = '\0';
    errorMessage[0] = '\0';
  }
};

/**
 * @typedef StatusChangeCallback
 * @brief Callback function type for status change events.
 * 
 * Use this to register a callback that will be invoked when status changes occur.
 * The callback receives a const reference to a StatusChangedEvent structure
 * containing all relevant event information.
 * 
 * @param event The status change event data
 * 
 * @example
 * @code
 * manager->replaceStatusChangeEventHandler([](const StatusChangedEvent& event) {
 *     std::cout << "Property: " << event.pulserProperty 
 *               << " = " << event.newValue << std::endl;
 *     
 *     if (event.errorCode != C_ERROR_CODE::NO_ERROR) {
 *         std::cerr << "Error: " << event.errorMessage << std::endl;
 *     }
 * });
 * @endcode
 */
using StatusChangeCallback = std::function<void(const StatusChangedEvent &)>;
