#include "JSR-SDK/JSRSDKManager.h"
#include "JSR-SDK/JSRSDKManager.hpp"

#include <cstring>
#include <algorithm>
#include <vector>

// Helper to copy std::string into caller buffer. Returns required buffer size (including null)
static int copyStringToBuffer(const std::string &s, char *buf, int bufSize) {
  int required = static_cast<int>(s.size()) + 1; // include null
  if (buf == nullptr || bufSize <= 0)
    return required;
  int toCopy = std::min(bufSize - 1, static_cast<int>(s.size()));
  memcpy(buf, s.c_str(), toCopy);
  buf[toCopy] = '\0';
  return required;
}

// Create / destroy
JSRSDKManagerHandle JSR_CreateManager() {
  JSRSDKManager *m = CreateJSRSDKManager();
  return reinterpret_cast<JSRSDKManagerHandle>(m);
}

void JSR_DestroyManager(JSRSDKManagerHandle mgr) {
  JSRSDKManager *m = reinterpret_cast<JSRSDKManager *>(mgr);
  DestroyJSRSDKManager(m);
}

// Plugin names
int JSR_GetPluginNamesCount(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    auto v = m->GetPluginNames();
    return static_cast<int>(v.size());
  } catch(...) { return -1; }
}

int JSR_GetPluginName(JSRSDKManagerHandle mgr, int index, char *outBuf, int bufSize) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    auto v = m->GetPluginNames();
    if (index < 0 || index >= static_cast<int>(v.size())) return -2;
    return copyStringToBuffer(v[index], outBuf, bufSize);
  } catch(...) { return -1; }
}

// Managed plugin names
int JSR_GetManagedPluginNamesCount(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1;
  try { auto m = reinterpret_cast<JSRSDKManager *>(mgr); auto v = m->GetManagedPluginNames(); return static_cast<int>(v.size()); } catch(...) { return -1; }
}
int JSR_GetManagedPluginName(JSRSDKManagerHandle mgr, int index, char *outBuf, int bufSize) {
  if (!mgr) return -1;
  try { auto m = reinterpret_cast<JSRSDKManager *>(mgr); auto v = m->GetManagedPluginNames(); if (index<0||index>=static_cast<int>(v.size())) return -2; return copyStringToBuffer(v[index], outBuf, bufSize); } catch(...) { return -1; }
}

// Custom settings
int JSR_GetCustomSettingsCount(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1;
  try { auto m = reinterpret_cast<JSRSDKManager *>(mgr); auto v = m->GetCustomSettings(); return static_cast<int>(v.size()); } catch(...) { return -1; }
}
int JSR_GetCustomSetting(JSRSDKManagerHandle mgr, int index, char *outBuf, int bufSize) {
  if (!mgr) return -1;
  try { auto m = reinterpret_cast<JSRSDKManager *>(mgr); auto v = m->GetCustomSettings(); if (index<0||index>=static_cast<int>(v.size())) return -2; return copyStringToBuffer(v[index], outBuf, bufSize); } catch(...) { return -1; }
}

// Open option names
int JSR_GetOpenOptionNamesCount(JSRSDKManagerHandle mgr, const char *plugin) {
  if (!mgr || !plugin) return -1;
  try { auto m = reinterpret_cast<JSRSDKManager *>(mgr); auto v = m->GetOpenOptionNames(std::string(plugin)); return static_cast<int>(v.size()); } catch(...) { return -1; }
}
int JSR_GetOpenOptionName(JSRSDKManagerHandle mgr, const char *plugin, int index, char *outBuf, int bufSize) {
  if (!mgr || !plugin) return -1;
  try { auto m = reinterpret_cast<JSRSDKManager *>(mgr); auto v = m->GetOpenOptionNames(std::string(plugin)); if (index<0||index>=static_cast<int>(v.size())) return -2; return copyStringToBuffer(v[index], outBuf, bufSize); } catch(...) { return -1; }
}

// Ports include/exclude
int JSR_GetPortsToExcludeCount(JSRSDKManagerHandle mgr, const char *plugin) {
  if (!mgr || !plugin) return -1;
  try { auto m = reinterpret_cast<JSRSDKManager *>(mgr); auto v = m->GetPortsToExclude(std::string(plugin)); return static_cast<int>(v.size()); } catch(...) { return -1; }
}
int JSR_GetPortToExclude(JSRSDKManagerHandle mgr, const char *plugin, int index, char *outBuf, int bufSize) {
  if (!mgr || !plugin) return -1;
  try { auto m = reinterpret_cast<JSRSDKManager *>(mgr); auto v = m->GetPortsToExclude(std::string(plugin)); if (index<0||index>=static_cast<int>(v.size())) return -2; return copyStringToBuffer(v[index], outBuf, bufSize); } catch(...) { return -1; }
}

int JSR_GetPortsToIncludeCount(JSRSDKManagerHandle mgr, const char *plugin) {
  if (!mgr || !plugin) return -1;
  try { auto m = reinterpret_cast<JSRSDKManager *>(mgr); auto v = m->GetPortsToInclude(std::string(plugin)); return static_cast<int>(v.size()); } catch(...) { return -1; }
}
int JSR_GetPortToInclude(JSRSDKManagerHandle mgr, const char *plugin, int index, char *outBuf, int bufSize) {
  if (!mgr || !plugin) return -1;
  try { auto m = reinterpret_cast<JSRSDKManager *>(mgr); auto v = m->GetPortsToInclude(std::string(plugin)); if (index<0||index>=static_cast<int>(v.size())) return -2; return copyStringToBuffer(v[index], outBuf, bufSize); } catch(...) { return -1; }
}

// Instruments
int JSR_GetInstrumentsCount(JSRSDKManagerHandle mgr, const char *plugin) {
  if (!mgr || !plugin) return -1;
  try { auto m = reinterpret_cast<JSRSDKManager *>(mgr); auto v = m->GetInstruments(std::string(plugin)); return static_cast<int>(v.size()); } catch(...) { return -1; }
}
int JSR_GetInstrumentModelName(JSRSDKManagerHandle mgr, const char *plugin, int index, char *outBuf, int bufSize) {
  if (!mgr || !plugin) return -1;
  try { auto m = reinterpret_cast<JSRSDKManager *>(mgr); auto v = m->GetInstruments(std::string(plugin)); if (index<0||index>=static_cast<int>(v.size())) return -2; return copyStringToBuffer(v[index].ModelName, outBuf, bufSize); } catch(...) { return -1; }
}
int JSR_GetInstrumentSerialNum(JSRSDKManagerHandle mgr, const char *plugin, int index, char *outBuf, int bufSize) {
  if (!mgr || !plugin) return -1;
  try { auto m = reinterpret_cast<JSRSDKManager *>(mgr); auto v = m->GetInstruments(std::string(plugin)); if (index<0||index>=static_cast<int>(v.size())) return -2; return copyStringToBuffer(v[index].SerialNum, outBuf, bufSize); } catch(...) { return -1; }
}
int JSR_GetInstrumentPort(JSRSDKManagerHandle mgr, const char *plugin, int index, char *outBuf, int bufSize) {
  if (!mgr || !plugin) return -1;
  try { auto m = reinterpret_cast<JSRSDKManager *>(mgr); auto v = m->GetInstruments(std::string(plugin)); if (index<0||index>=static_cast<int>(v.size())) return -2; return copyStringToBuffer(v[index].Port, outBuf, bufSize); } catch(...) { return -1; }
}
int JSR_GetInstrumentPluginName(JSRSDKManagerHandle mgr, const char *plugin, int index, char *outBuf, int bufSize) {
  if (!mgr || !plugin) return -1;
  try { auto m = reinterpret_cast<JSRSDKManager *>(mgr); auto v = m->GetInstruments(std::string(plugin)); if (index<0||index>=static_cast<int>(v.size())) return -2; return copyStringToBuffer(v[index].PluginName, outBuf, bufSize); } catch(...) { return -1; }
}

// PulserReceiver info
int JSR_GetPulserReceiverInfoCount(JSRSDKManagerHandle mgr, const char *model, const char *serial, int idxPR) {
  if (!mgr || !model || !serial) return -1;
  try { auto m = reinterpret_cast<JSRSDKManager *>(mgr); auto v = m->GetPulserReceiverInfo(std::string(model), std::string(serial), idxPR); return static_cast<int>(v.size()); } catch(...) { return -1; }
}
int JSR_GetPulserReceiverInfoItem(JSRSDKManagerHandle mgr, const char *model, const char *serial, int idxPR, int index, char *outBuf, int bufSize) {
  if (!mgr || !model || !serial) return -1;
  try { auto m = reinterpret_cast<JSRSDKManager *>(mgr); auto v = m->GetPulserReceiverInfo(std::string(model), std::string(serial), idxPR); if (index<0||index>=static_cast<int>(v.size())) return -2; return copyStringToBuffer(v[index], outBuf, bufSize); } catch(...) { return -1; }
}

// PulserReceivers list
int JSR_GetPulserReceiversCount(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1;
  try { auto m = reinterpret_cast<JSRSDKManager *>(mgr); auto v = m->GetPulserReceivers(); return static_cast<int>(v.size()); } catch(...) { return -1; }
}
int JSR_GetPulserReceiverInstrumentModelName(JSRSDKManagerHandle mgr, int index, char *outBuf, int bufSize) {
  if (!mgr) return -1;
  try { auto m = reinterpret_cast<JSRSDKManager *>(mgr); auto v = m->GetPulserReceivers(); if (index<0||index>=static_cast<int>(v.size())) return -2; return copyStringToBuffer(v[index].InstrumentId.ModelName, outBuf, bufSize); } catch(...) { return -1; }
}
int JSR_GetPulserReceiverInstrumentSerialNum(JSRSDKManagerHandle mgr, int index, char *outBuf, int bufSize) {
  if (!mgr) return -1;
  try { auto m = reinterpret_cast<JSRSDKManager *>(mgr); auto v = m->GetPulserReceivers(); if (index<0||index>=static_cast<int>(v.size())) return -2; return copyStringToBuffer(v[index].InstrumentId.SerialNum, outBuf, bufSize); } catch(...) { return -1; }
}
int JSR_GetPulserReceiverIndex(JSRSDKManagerHandle mgr, int index) {
  if (!mgr) return -1;
  try { auto m = reinterpret_cast<JSRSDKManager *>(mgr); auto v = m->GetPulserReceivers(); if (index<0||index>=static_cast<int>(v.size())) return -2; return v[index].PulserReceiverIndex; } catch(...) { return -1; }
}

// PulserMaxPRFs
int JSR_GetPulserMaxPRFsCount(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1;
  try { auto m = reinterpret_cast<JSRSDKManager *>(mgr); auto v = m->getPulserMaxPRFs(); return static_cast<int>(v.size()); } catch(...) { return -1; }
}
double JSR_GetPulserMaxPRF(JSRSDKManagerHandle mgr, int index) {
  if (!mgr) return -1.0;
  try { auto m = reinterpret_cast<JSRSDKManager *>(mgr); auto v = m->getPulserMaxPRFs(); if (index<0||index>=static_cast<int>(v.size())) return -1.0; return v[index]; } catch(...) { return -1.0; }
}

// Pulse repetition frequencies
int JSR_GetPulseRepetitionFrequencyValuesCount(JSRSDKManagerHandle mgr) { if(!mgr) return -1; try { auto m = reinterpret_cast<JSRSDKManager *>(mgr); auto v = m->getPulseRepetitionFrequencyValues(); return static_cast<int>(v.size()); } catch(...) { return -1; } }
double JSR_GetPulseRepetitionFrequencyValue(JSRSDKManagerHandle mgr, int index) { if(!mgr) return -1.0; try { auto m = reinterpret_cast<JSRSDKManager *>(mgr); auto v = m->getPulseRepetitionFrequencyValues(); if(index<0||index>=static_cast<int>(v.size())) return -1.0; return v[index]; } catch(...) { return -1.0; } }

// OEM data
int JSR_GetReceiverOEMDataSize(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1;
  try { auto m = reinterpret_cast<JSRSDKManager *>(mgr); auto v = m->getReceiverOEMData(); return static_cast<int>(v.size()); } catch(...) { return -1; }
}
int JSR_CopyReceiverOEMData(JSRSDKManagerHandle mgr, unsigned char *dest, int destSize) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    auto v = m->getReceiverOEMData();
    int required = static_cast<int>(v.size());
    if (dest == nullptr || destSize <= 0) return required;
    int toCopy = std::min(destSize, required);
    memcpy(dest, v.data(), toCopy);
    return toCopy;
  } catch(...) { return -1; }
}

int JSR_GetPulserOEMDataSize(JSRSDKManagerHandle mgr) {
  if (!mgr) return -1;
  try { auto m = reinterpret_cast<JSRSDKManager *>(mgr); auto v = m->getPulserOEMData(); return static_cast<int>(v.size()); } catch(...) { return -1; }
}
int JSR_CopyPulserOEMData(JSRSDKManagerHandle mgr, unsigned char *dest, int destSize) {
  if (!mgr) return -1;
  try {
    auto m = reinterpret_cast<JSRSDKManager *>(mgr);
    auto v = m->getPulserOEMData();
    int required = static_cast<int>(v.size());
    if (dest == nullptr || destSize <= 0) return required;
    int toCopy = std::min(destSize, required);
    memcpy(dest, v.data(), toCopy);
    return toCopy;
  } catch(...) { return -1; }
}

