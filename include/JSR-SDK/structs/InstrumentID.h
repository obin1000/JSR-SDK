#pragma once
#include <cstring>
#include <string>

/**
 * @class InstrumentID
 * @brief Represents an instrument with identifying details such as model name,
 * serial number, port, and plugin name.
 * Mirrors JSRDotNETSDK::IInstrumentIdentity
 *
 * All string fields are fixed-size buffers. The caller is responsible for
 * ensuring data fits within these buffers.
 */
struct InstrumentID {
  static constexpr size_t MAX_STRING_LENGTH = 256;

  char ModelName[MAX_STRING_LENGTH];
  char SerialNum[MAX_STRING_LENGTH];
  char Port[MAX_STRING_LENGTH];
  char PluginName[MAX_STRING_LENGTH];

  InstrumentID() {
    ModelName[0] = '\0';
    SerialNum[0] = '\0';
    Port[0] = '\0';
    PluginName[0] = '\0';
  }

#ifdef __cplusplus
  /**
   * @brief Constructs a detailed description of the instrument.
   * @return A string containing the model name, serial number, port, and plugin name.
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
   * @return A string containing the instrument's description.
   */
  std::string ToString() const { return GetDescription(); }
#endif
};
