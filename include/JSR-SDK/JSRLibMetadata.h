#pragma once

#include "JSR-SDK/enums/ConnectionType.h"

#include <string>
#include <vector>

class JSRLibMetadata {
public:
  std::string Name;
  std::string GUID;
  std::string FriendlyName;
  std::vector<ConnectionType> ConnectionType;
  std::vector<std::string> SupportedModels;
  std::vector<std::string> OpenOptions;
  int InterfaceVersion;
  std::string Version;
};
