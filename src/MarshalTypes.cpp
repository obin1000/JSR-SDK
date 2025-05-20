#include "JSR-SDK/InstrumentID.h"
#include "JSR-SDK/PulserReceiverID.h"
#include <msclr/marshal_cppstd.h>

using namespace JSRDotNETSDK;


// Converting InstrumentID <-> IInstrumentIdentity^
static InstrumentID instrumentFromManaged(IInstrumentIdentity ^
                                          instrumentIdentity) {
  InstrumentID unmanaged;
  unmanaged.ModelName =
      msclr::interop::marshal_as<std::string>(instrumentIdentity->ModelName);
  unmanaged.PluginName =
      msclr::interop::marshal_as<std::string>(instrumentIdentity->PluginName);
  unmanaged.Port =
      msclr::interop::marshal_as<std::string>(instrumentIdentity->Port);
  unmanaged.SerialNum =
      msclr::interop::marshal_as<std::string>(instrumentIdentity->SerialNum);
  return unmanaged;
}


// Converting InstrumentID <-> IInstrumentIdentity^
static PulserReceiverID pulsereceiverFromManaged(IPulserReceiverIdentity ^
                                    instrumentIdentity) {
  PulserReceiverID unmanaged;
  unmanaged.InstrumentId =
      instrumentFromManaged(instrumentIdentity->InstrumentId);
  unmanaged.PulserReceiverIndex = instrumentIdentity->PulserReceiverIndex;
  return unmanaged;
}