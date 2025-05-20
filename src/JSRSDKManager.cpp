#include "JSR-SDK/JSRSDKManager.h"
#include "MarshalTypes.cpp"
#include <msclr/marshal_cppstd.h>
#include <vcclr.h>

using namespace System;
using namespace JSRDotNETSDK;
using namespace msclr::interop;
using namespace System::Reflection;
using namespace System::IO;

class JSRSDKManagerWrapper : public JSRSDKManager {
public:
  JSRSDKManagerWrapper() {
    try {
      m_manager = gcnew JSRDotNETManager("");
      String ^ dllPath = Assembly::GetExecutingAssembly()->Location;
      String ^ dllFolder = Path::GetDirectoryName(dllPath);

      Console::WriteLine("Reading plugins from: " + dllFolder);
      m_manager->LoadPlugins(dllFolder);
    } catch (System::Exception ^ exception) {
      std::string msg =
          msclr::interop::marshal_as<std::string>(exception->Message);
      throw msg;
    }
  }

  ~JSRSDKManagerWrapper() override {
    if (static_cast<JSRDotNETManager ^>(m_manager) != nullptr) {
      m_manager->Shutdown();
      m_manager = nullptr;
    }
  }

  int GetNumberOfPlugins() override {
    return m_manager->GetPluginNames()->Length;
  }

  std::vector<std::string> GetPluginNames() override {
    std::vector<std::string> result;
    auto names = m_manager->GetPluginNames();
    for each (String ^ name in names) {
      result.push_back(marshal_as<std::string>(name));
    }
    return result;
  }

  void AddManagedPlugin(std::string pluginName) {
    m_manager->AddManagedPlugin(marshal_as<String ^>(pluginName));
  }

  void SetDiscoveryEnable(bool bEnable) {
    m_manager->SetDiscoveryEnable(bEnable);
  }

  std::vector<InstrumentID> GetInstruments(std::string pluginName) {
    std::vector<InstrumentID> result;
    auto instruments =
        m_manager->GetInstruments(marshal_as<String ^>(pluginName));
    for each (IInstrumentIdentity ^ instrument in instruments) {
      result.push_back(instrumentFromManaged(instrument));
    }
    return result;
  }
  std::vector<PulserReceiverID> GetPulserReceivers() {
    std::vector<PulserReceiverID> result;
    auto pulsereceivers = m_manager->GetPulserReceivers(nullptr);

    for each (IPulserReceiverIdentity ^ pulsereceiver in pulsereceivers) {
      result.push_back(pulsereceiverFromManaged(pulsereceiver));
    }
    return result;
  }

  void SetCurrentPulserReceiver(PulserReceiverID prID) {
    SetCurrentPulserReceiver(prID.InstrumentId.ModelName,
                             prID.InstrumentId.SerialNum,
                             prID.PulserReceiverIndex);
  }
  void SetCurrentPulserReceiver(std::string model, std::string serialNum,
                                int idxPR) {
    m_manager->SetCurrentPulserReceiver(marshal_as<String ^>(model),
                                        marshal_as<String ^>(serialNum), idxPR);
  }

  bool getTriggerEnable() { return m_manager->TriggerEnable; }
  void setTriggerEnable(bool enable) { m_manager->TriggerEnable = enable; }

  // Variables
  bool getPulseRepetitionFrequencyIndexSupported() {
    return m_manager->PulseRepetitionFrequencyIndexSupported;
  }
  double getHVSupplyMax() { return m_manager->HVSupplyMax; }
  double getHVSupply() { return m_manager->HVSupply; }
  void setHVSupply(double hvSupply) { m_manager->HVSupply = hvSupply; }
  bool getHVSupplySupported() { return m_manager->HVSupplySupported; }
  int getPulseRepetitionFrequencyNumerator() {
    return m_manager->PulseRepetitionFrequencyNumerator;
  }
  std::vector<double> getPulseRepetitionFrequencyValues() {
    std::vector<double> result;
    auto values = m_manager->PulseRepetitionFrequencyValues;
    for each (double value in values) {
      result.push_back(value);
    }
    return result;
  }
  int getPulseRepetitionFrequencyIndexMax() {
    return m_manager->PulseRepetitionFrequencyIndexMax;
  }
  int getPulseRepetitionFrequencyIndex() {
    return m_manager->PulseRepetitionFrequencyIndex;
  }
  void setPulseRepetitionFrequencyIndex(int index) {
    m_manager->PulseRepetitionFrequencyIndex = index;
  }

  double getPulseRepetitionFrequencyMin() {
    return m_manager->PulseRepetitionFrequencyMin;
  }
  double getPulseRepetitionFrequencyMax() {
    return m_manager->PulseRepetitionFrequencyMax;
  }

  double getPulseRepetitionFrequency() {
    return m_manager->PulseRepetitionFrequency;
  }
  void setPulseRepetitionFrequency(double frequency) {
    m_manager->PulseRepetitionFrequency = frequency;
  }

  bool getTriggerEdgePolaritySupported() {
    return m_manager->TriggerEdgePolaritySupported;
  }

  std::vector<double> getHighPassFilterValues() {
    std::vector<double> result;
    auto values = m_manager->HighPassFilterValues;
    for each (double value in values) {
      result.push_back(value);
    }
    return result;
  }

  int getHighPassFilterIndexMax() { return m_manager->HighPassFilterIndexMax; }

  int getHighPassFilterIndex() { return m_manager->HighPassFilterIndex; }
  void setHighPassFilterIndex(int index) {
    m_manager->HighPassFilterIndex = index;
  }

  int getLowPassFilterIndexMax() { return m_manager->LowPassFilterIndexMax; }

private:
  gcroot<JSRDotNETManager ^> m_manager;
};

extern "C" {

__declspec(dllexport) JSRSDKManager *CreateJSRSDKManager() {
  try {
    return new JSRSDKManagerWrapper();
  } catch (System::Exception ^ exception) {
    std::string msg =
        msclr::interop::marshal_as<std::string>(exception->Message);
    throw "Failed creating JSR Bridge object: " + msg;
  }
}

__declspec(dllexport) void DestroyJSRSDKManager(JSRSDKManager *bridge) {
  delete bridge;
}
}
