#include "JSR-SDK/InstrumentID.h"
#include "JSR-SDK/PulserReceiverID.h"
#include "JSR-SDK/enums/IsPulsing.h"
#include "JSR-SDK/enums/ManagerState.h"
#include "JSR-SDK/enums/PowerLimit.h"
#include "JSR-SDK/enums/PulserImpedance.h"
#include "JSR-SDK/enums/ReceiverMode.h"
#include "JSR-SDK/enums/TriggerImpedance.h"
#include "JSR-SDK/enums/TriggerPolarity.h"
#include "JSR-SDK/enums/TriggerSource.h"
#include <msclr/marshal_cppstd.h>

using namespace JSRDotNETSDK;
/**
 * @file MarshalTypes.cpp
 * @brief This file contains the conversion functions between JSR-SDK types and
 * JSRDotNETSDK types.
 * These can't be exposed through the headers as it contains references to
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

// converting JSR-SDK::TriggerImpedance <-> JSRDotNETSDK::TRIGGER_IMPEDANCE
static TriggerImpedance triggerImpedanceFromManaged(TRIGGER_IMPEDANCE source) {
  switch (source) {
  case TRIGGER_IMPEDANCE::HIGH_Z:
    return TriggerImpedance::HIGH_Z;
  case TRIGGER_IMPEDANCE::LOW_Z:
    return TriggerImpedance::LOW_Z;
  case TRIGGER_IMPEDANCE::LAST_VAL:
    return TriggerImpedance::LAST_VAL;
  default:
    return TriggerImpedance::UNKNOWN;
  }
}

static TRIGGER_IMPEDANCE triggerImpedanceToManaged(TriggerImpedance source) {
  switch (source) {
  case TriggerImpedance::HIGH_Z:
    return TRIGGER_IMPEDANCE::HIGH_Z;
  case TriggerImpedance::LOW_Z:
    return TRIGGER_IMPEDANCE::LOW_Z;
  case TriggerImpedance::LAST_VAL:
    return TRIGGER_IMPEDANCE::LAST_VAL;
  default:
    // TODO: throw exception for unknown enum value?
    return TRIGGER_IMPEDANCE::LAST_VAL;
  }
}

// converting JSR-SDK::PulserImpedance <-> JSRDotNETSDK::PULSER_IMPEDANCE
static PulserImpedance pulserImpedanceFromManaged(PULSER_IMPEDANCE source) {
  switch (source) {
  case PULSER_IMPEDANCE::HIGH_Z:
    return PulserImpedance::HIGH_Z;
  case PULSER_IMPEDANCE::LOW_Z:
    return PulserImpedance::LOW_Z;
  case PULSER_IMPEDANCE::LAST_VAL:
    return PulserImpedance::LAST_VAL;
  default:
    return PulserImpedance::UNKNOWN;
  }
}

static PULSER_IMPEDANCE pulserImpedanceToManaged(PulserImpedance source) {
  switch (source) {
  case PulserImpedance::HIGH_Z:
    return PULSER_IMPEDANCE::HIGH_Z;
  case PulserImpedance::LOW_Z:
    return PULSER_IMPEDANCE::LOW_Z;
  case PulserImpedance::LAST_VAL:
    return PULSER_IMPEDANCE::LAST_VAL;
  default:
    // TODO: throw exception for unknown enum value?
    return PULSER_IMPEDANCE::LAST_VAL;
  }
}

// converting JSR-SDK::IsPulsing <-> JSRDotNETSDK::IS_PULSING
static IsPulsing isPulsingFromManaged(IS_PULSING source) {
  switch (source) {
  case IS_PULSING::ACTIVE:
    return IsPulsing::ACTIVE;
  case IS_PULSING::INACTIVE:
    return IsPulsing::INACTIVE;
  default:
    return IsPulsing::UNKNOWN;
  }
}

static IS_PULSING isPulsingToManaged(IsPulsing source) {
  switch (source) {
  case IsPulsing::ACTIVE:
    return IS_PULSING::ACTIVE;
  case IsPulsing::INACTIVE:
    return IS_PULSING::INACTIVE;
  default:
    return IS_PULSING::UNKNOWN;
  }
}

// converting JSR-SDK::PowerLimit <-> JSRDotNETSDK::POWER_LIMIT
static PowerLimit powerLimitFromManaged(POWER_LIMIT source) {
  switch (source) {
  case POWER_LIMIT::OVER_LIMIT:
    return PowerLimit::OVER_LIMIT;
  case POWER_LIMIT::WITHIN_LIMIT:
    return PowerLimit::WITHIN_LIMIT;
  default:
    return PowerLimit::UNKNOWN;
  }
}

static POWER_LIMIT powerLimitToManaged(PowerLimit source) {
  switch (source) {
  case PowerLimit::OVER_LIMIT:
    return POWER_LIMIT::OVER_LIMIT;
  case PowerLimit::WITHIN_LIMIT:
    return POWER_LIMIT::WITHIN_LIMIT;
  default:
    return POWER_LIMIT::UNKNOWN;
  }
}

// converting JSR-SDK::ManagerState <->
// JSRDotNETSDK::JSRDotNETManager::MANAGER_STATE
static ManagerState
managerStateFromManaged(JSRDotNETManager::MANAGER_STATE state) {
  switch (state) {
  case JSRDotNETManager::MANAGER_STATE::NOT_STARTED:
    return ManagerState::NOT_STARTED;
  case JSRDotNETManager::MANAGER_STATE::PLUGINS_LOADED:
    return ManagerState::PLUGINS_LOADED;
  case JSRDotNETManager::MANAGER_STATE::RUNNING:
    return ManagerState::RUNNING;
  case JSRDotNETManager::MANAGER_STATE::SHUTTING_DOWN:
    return ManagerState::SHUTTING_DOWN;
  default:
    return ManagerState::UNKNOWN;
  }
}

static JSRDotNETManager::MANAGER_STATE
managerStateToManaged(ManagerState state) {
  switch (state) {
  case ManagerState::NOT_STARTED:
    return JSRDotNETManager::MANAGER_STATE::NOT_STARTED;
  case ManagerState::PLUGINS_LOADED:
    return JSRDotNETManager::MANAGER_STATE::PLUGINS_LOADED;
  case ManagerState::RUNNING:
    return JSRDotNETManager::MANAGER_STATE::RUNNING;
  case ManagerState::SHUTTING_DOWN:
    return JSRDotNETManager::MANAGER_STATE::SHUTTING_DOWN;
  default:
    // TODO: throw exception for unknown enum value?
    return JSRDotNETManager::MANAGER_STATE::SHUTTING_DOWN;
  }
}
