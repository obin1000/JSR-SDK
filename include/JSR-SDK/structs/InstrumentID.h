#pragma once
#include "JSR-SDK/boundary/CString.h"
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
  CString ModelName;

  /**
   * @brief The serial number of the instrument.
   */
  CString SerialNum;

  /**
   * @brief The port associated with the instrument.
   */
  CString Port;

  /**
   * @brief The name of the plugin associated with the instrument.
   */
  CString PluginName;

  InstrumentID() = default;

  /**
   * @brief Constructs a detailed description of the instrument.
   * @return A string containing the model name, serial number, port, and plugin
   * name.
   */
  std::string GetDescription() const {
    std::string description = "Model: " + ModelName.to_std_string() + "\n" +
                              "Serial Number: " + SerialNum.to_std_string() +
                              "\n" + "Port: " + Port.to_std_string() + "\n" +
                              "Plugin Name: " + PluginName.to_std_string() +
                              "\n";
    return description;
  }

  /**
   * @brief Converts the instrument details to a string representation.
   * @return A string containing the instrument's description.
   */
  std::string ToString() { return GetDescription(); }

  /**
   * @brief Destructor to free allocated CString memory.
   */
  ~InstrumentID() {
    ModelName.free_cstring();
    SerialNum.free_cstring();
    Port.free_cstring();
    PluginName.free_cstring();
  }

  InstrumentID(const InstrumentID &other) {
    ModelName = CString::from_std_string(other.ModelName.to_std_string());
    SerialNum = CString::from_std_string(other.SerialNum.to_std_string());
    Port = CString::from_std_string(other.Port.to_std_string());
    PluginName = CString::from_std_string(other.PluginName.to_std_string());
  }

  InstrumentID &operator=(const InstrumentID &other) {
    if (this != &other) {
      ModelName.free_cstring();
      SerialNum.free_cstring();
      Port.free_cstring();
      PluginName.free_cstring();

      ModelName = CString::from_std_string(other.ModelName.to_std_string());
      SerialNum = CString::from_std_string(other.SerialNum.to_std_string());
      Port = CString::from_std_string(other.Port.to_std_string());
      PluginName = CString::from_std_string(other.PluginName.to_std_string());
    }
    return *this;
  }

  InstrumentID(InstrumentID &&other) noexcept
      : ModelName(other.ModelName), SerialNum(other.SerialNum),
        Port(other.Port), PluginName(other.PluginName) {
    other.ModelName = CString();
    other.SerialNum = CString();
    other.Port = CString();
    other.PluginName = CString();
  }

  InstrumentID &operator=(InstrumentID &&other) noexcept {
    if (this != &other) {
      ModelName.free_cstring();
      SerialNum.free_cstring();
      Port.free_cstring();
      PluginName.free_cstring();

      ModelName = other.ModelName;
      SerialNum = other.SerialNum;
      Port = other.Port;
      PluginName = other.PluginName;

      other.ModelName = CString();
      other.SerialNum = CString();
      other.Port = CString();
      other.PluginName = CString();
    }
    return *this;
  }
};
