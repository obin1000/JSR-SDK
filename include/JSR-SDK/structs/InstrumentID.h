#pragma once
#include <string>

/**
 * @class InstrumentID
 * @brief Represents an instrument with identifying details such as model name,
 * serial number, port, and plugin name.
 * Mirrors JSRDotNETSDK::IInstrumentIdentity
 */
class InstrumentID {
public:
  /**
   * @brief The model name of the instrument.
   */
  std::string ModelName;

  /**
   * @brief The serial number of the instrument.
   */
  std::string SerialNum;

  /**
   * @brief The port associated with the instrument.
   */
  std::string Port;

  /**
   * @brief The name of the plugin associated with the instrument.
   */
  std::string PluginName;

  /**
   * @brief Constructs a detailed description of the instrument.
   * @return A string containing the model name, serial number, port, and plugin
   * name.
   */
  std::string GetDescription() const {
    std::string description =
        "Model: " + ModelName + "\n" + "Serial Number: " + SerialNum + "\n" +
        "Port: " + Port + "\n" + "Plugin Name: " + PluginName + "\n";
    return description;
  }

  /**
   * @brief Converts the instrument details to a string representation.
   * @return A string containing the instrument's description.
   */
  std::string ToString() { return GetDescription(); }
};
