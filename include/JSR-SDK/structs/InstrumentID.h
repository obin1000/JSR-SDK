#pragma once
#include <cstring>
#include <string>

/**
 * @struct InstrumentID
 * @brief Identifies a physical instrument/device in the JSR SDK system.
 * 
 * This structure contains all the information needed to uniquely identify
 * and locate a specific ultrasonic testing instrument. It mirrors the
 * JSRDotNETSDK::IInstrumentIdentity interface.
 * 
 * All string fields are fixed-size character arrays to ensure ABI stability
 * and avoid dynamic memory allocation. This makes the structure safe to pass
 * across DLL boundaries and suitable for C interop.
 * 
 * @note In C this is a plain aggregate struct; in C++ it is a
 *       standard-layout, ABI-stable type with optional helper functions.
 */
struct InstrumentID {
  /// Maximum length for string fields (including null terminator)
  static constexpr size_t MAX_STRING_LENGTH = 256;

  /// Device model name (e.g., "JSR-DPR300", "HB3-1")
  char ModelName[MAX_STRING_LENGTH];
  
  /// Device serial number for unique identification (e.g., "JB0104", "SN12345")
  char SerialNum[MAX_STRING_LENGTH];
  
  /// Communication port where the device is connected (e.g., "COM3", "USB", "/dev/ttyUSB0")
  char Port[MAX_STRING_LENGTH];
  
  /// Name of the plugin that manages this device (e.g., "JSR-DPR", "Simulator")
  char PluginName[MAX_STRING_LENGTH];

  /**
   * @brief Default constructor - initializes all fields to empty strings.
   * 
   * This ensures the structure is in a safe state with all strings
   * properly null-terminated.
   */
  InstrumentID() {
    ModelName[0] = '\0';
    SerialNum[0] = '\0';
    Port[0] = '\0';
    PluginName[0] = '\0';
  }

#ifdef __cplusplus
  /**
   * @brief Constructs a detailed description of the instrument.
   * 
   * Creates a multi-line string containing all identification information
   * in a human-readable format.
   * 
   * @return A formatted string with model, serial number, port, and plugin name.
   * 
   * @example
   * @code
   * InstrumentID id;
   * std::string desc = id.GetDescription();
   * // Output:
   * // Model: JSR-DPR300
   * // Serial Number: JB0104
   * // Port: COM3
   * // Plugin Name: JSR-DPR
   * @endcode
   */
  std::string GetDescription() const {
    std::string description = std::string("Model: ") + ModelName + "\n" +
                              "Serial Number: " + SerialNum + "\n" +
                              "Port: " + Port + "\n" +
                              "Plugin Name: " + PluginName + "\n";
    return description;
  }

  /**
   * @brief Converts the instrument details to a string representation.
   * 
   * Convenience method that calls GetDescription().
   * 
   * @return A formatted string containing the instrument's description.
   */
  std::string ToString() const { return GetDescription(); }
#endif
};
