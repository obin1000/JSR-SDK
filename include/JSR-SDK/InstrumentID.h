#pragma once
#include<string>


class InstrumentID {
    public:
      std::string ModelName;
      std::string SerialNum;
      std::string Port;
      std::string PluginName;

      std::string GetDescription() const {
          std::string description = "Model: " + ModelName + "\n" +
              "Serial Number: " + SerialNum + "\n" +
              "Port: " + Port + "\n" +
              "Plugin Name: " + PluginName + "\n";
            return description;
      }
      std::string ToString() { 
          return GetDescription();
      }
};

