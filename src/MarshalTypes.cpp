#include "JSR-SDK/InstrumentID.h"
#include "JSR-SDK/PulserReceiverID.h"
#include "JSR-SDK/enums/ReceiverMode.h"
#include "JSR-SDK/enums/TriggerPolarity.h"
#include "JSR-SDK/enums/TriggerSource.h"
#include <msclr/marshal_cppstd.h>

using namespace JSRDotNETSDK;
/**
 * @file MarshalTypes.cpp
 * @brief This file contains the conversion functions between JSR-SDK types and
 * JSRDotNETSDK types.
 * These can't be exposed through the headers as it contains refernces to
 * managed objects.
 *
 **/

// Converting JSR-SDK::InstrumentID <-> JSRDotNETSDK::IInstrumentIdentity^
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

// Converting JSR-SDK::PulserReceiverID <->
// JSRDotNETSDK::IPulserReceiverIdentity^
static PulserReceiverID pulsereceiverFromManaged(IPulserReceiverIdentity ^
                                                 instrumentIdentity) {
  PulserReceiverID unmanaged;
  unmanaged.InstrumentId =
      instrumentFromManaged(instrumentIdentity->InstrumentId);
  unmanaged.PulserReceiverIndex = instrumentIdentity->PulserReceiverIndex;
  return unmanaged;
}

// converting JSR-SDK::RecieverMode <-> JSRDotNETSDK::RECEIVER_MODE
static RecieverMode receiverModeFromManaged(RECEIVER_MODE mode) {
  switch (mode) {
  case RECEIVER_MODE::BOTH:
    return RecieverMode::BOTH;
  case RECEIVER_MODE::ECHO:
    return RecieverMode::ECHO;
  case RECEIVER_MODE::LAST_VAL:
    return RecieverMode::LAST_VAL;
  case RECEIVER_MODE::THRU:
    return RecieverMode::THRU;
  default:
    return RecieverMode::UNKNOWN;
  }
}

static RECEIVER_MODE receiverModeToManaged(RecieverMode mode) {
  switch (mode) {
  case RecieverMode::BOTH:
    return RECEIVER_MODE::BOTH;
  case RecieverMode::ECHO:
    return RECEIVER_MODE::ECHO;
  case RecieverMode::LAST_VAL:
    return RECEIVER_MODE::LAST_VAL;
  case RecieverMode::THRU:
    return RECEIVER_MODE::THRU;
  default:
    // TODO: throw exception for unknown enum value?
    return RECEIVER_MODE::LAST_VAL;
  }
}

// converting JSR-SDK::TriggerSource <-> JSRDotNETSDK::TRIGGER_SOURCE
static TriggerSource triggerSourceFromManaged(TRIGGER_SOURCE source) {
  switch (source) {
  case TRIGGER_SOURCE::INTERNAL:
    return TriggerSource::INTERNAL;
  case TRIGGER_SOURCE::EXTERNAL:
    return TriggerSource::EXTERNAL;
  case TRIGGER_SOURCE::LAST_VAL:
    return TriggerSource::LAST_VAL;
  case TRIGGER_SOURCE::SLAVE:
    return TriggerSource::SLAVE;
  default:
    return TriggerSource::UNKNOWN;
  }
}

static TRIGGER_SOURCE triggerSourceToManaged(TriggerSource source) {
  switch (source) {
  case TriggerSource::INTERNAL:
    return TRIGGER_SOURCE::INTERNAL;
  case TriggerSource::EXTERNAL:
    return TRIGGER_SOURCE::EXTERNAL;
  case TriggerSource::LAST_VAL:
    return TRIGGER_SOURCE::LAST_VAL;
  case TriggerSource::SLAVE:
    return TRIGGER_SOURCE::SLAVE;
  default:
    // TODO: throw exception for unknown enum value?
    return TRIGGER_SOURCE::LAST_VAL;
  }
}

// converting JSR-SDK::TriggerPolarity <-> JSRDotNETSDK::TRIGGER_POLARITY
static TriggerPolarity triggerPolarityFromManaged(TRIGGER_POLARITY source) {
  switch (source) {
  case TRIGGER_POLARITY::FALLING:
    return TriggerPolarity::FALLING;
  case TRIGGER_POLARITY::RISING:
    return TriggerPolarity::RISING;
  case TRIGGER_POLARITY::LAST_VAL:
    return TriggerPolarity::LAST_VAL;
  default:
    return TriggerPolarity::UNKNOWN;
  }
}

static TRIGGER_POLARITY triggerPolarityToManaged(TriggerPolarity source) {
  switch (source) {
  case TriggerPolarity::FALLING:
    return TRIGGER_POLARITY::FALLING;
  case TriggerPolarity::RISING:
    return TRIGGER_POLARITY::RISING;
  case TriggerPolarity::LAST_VAL:
    return TRIGGER_POLARITY::LAST_VAL;
  default:
    // TODO: throw exception for unknown enum value?
    return TRIGGER_POLARITY::LAST_VAL;
  }
}