#include "JSR-SDK/InstrumentID.h"
#include "JSR-SDK/JSRLibMetadata.h"
#include "JSR-SDK/PulserReceiverID.h"
#include "JSR-SDK/enums/ConnectionType.h"
#include "JSR-SDK/enums/IsPulsing.h"
#include "JSR-SDK/enums/ManagerState.h"
#include "JSR-SDK/enums/PowerLimit.h"
#include "JSR-SDK/enums/PropertyUnits.h"
#include "JSR-SDK/enums/PulserImpedance.h"
#include "JSR-SDK/enums/PulserPropertyRoles.h"
#include "JSR-SDK/enums/ReceiverMode.h"
#include "JSR-SDK/enums/TriggerImpedance.h"
#include "JSR-SDK/enums/TriggerPolarity.h"
#include "JSR-SDK/enums/TriggerSource.h"
#include <msclr/marshal_cppstd.h>

using namespace JSRDotNETSDK;
using namespace System::Collections::Generic;
using namespace msclr::interop;

/**
 * @file MarshalTypes.cpp
 * @brief This file contains the conversion functions between JSR-SDK types and
 * JSRDotNETSDK types.
 * These can't be exposed through the headers as it contains references to
 * managed objects.
 *
 **/

/**
 * @brief Converts a managed List (C#) to a std::vector (C++).
 **/
template <typename managedType, typename unmanagedType>
static std::vector<unmanagedType> listToVector(IEnumerable<managedType> ^
                                               list) {
  std::vector<unmanagedType> vec;
  if (list == nullptr)
    return vec;

  // vec.reserve(list->Count);
  for each (managedType s in list)
    vec.push_back(marshal_as<unmanagedType>(s));

  return vec;
}

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

// Converting JSR-SDK::JSRLibMetadata <-> JSRDotNETSDK::IJSRDotNETLibMetadata^
static JSRLibMetadata libMetadataFromManaged(IJSRDotNETLibMetadata ^ metadata) {
  JSRLibMetadata unmanaged;
  unmanaged.Name = marshal_as<std::string>(metadata->Name);
  unmanaged.GUID = marshal_as<std::string>(metadata->GUID);
  unmanaged.FriendlyName = marshal_as<std::string>(metadata->FriendlyName);
  unmanaged.Version = marshal_as<std::string>(metadata->Version);
  unmanaged.InterfaceVersion = metadata->InterfaceVersion;
  unmanaged.SupportedModels = listToVector < System::String ^,
  std::string > (metadata->SupportedModels);
  unmanaged.OpenOptions = listToVector < System::String ^,
  std::string > (metadata->OpenOptions);
  unmanaged.ConnectionType =
      listToVector<CONNECTION_TYPE, ConnectionType>(metadata->ConnectionType);

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

// converting JSR-SDK::ConnectionType <-> JSRDotNETSDK::CONNECTION_TYPE
static ConnectionType connectionTypeFromManaged(CONNECTION_TYPE connection) {
  switch (connection) {
  case CONNECTION_TYPE::SOFTWARE:
    return ConnectionType::SOFTWARE;
  case CONNECTION_TYPE::SERIALPORT:
    return ConnectionType::SERIALPORT;
  case CONNECTION_TYPE::PCI:
    return ConnectionType::PCI;
  case CONNECTION_TYPE::USB:
    return ConnectionType::USB;
  case CONNECTION_TYPE::ETHERNET:
    return ConnectionType::ETHERNET;
  case CONNECTION_TYPE::FIREWIRE:
    return ConnectionType::FIREWIRE;
  case CONNECTION_TYPE::BLUETOOTH:
    return ConnectionType::BLUETOOTH;
  default:
    return ConnectionType::UNKNOWN;
  }
}

inline static CONNECTION_TYPE
connectionTypeToManaged(const ConnectionType &connection) {
  switch (connection) {
  case ConnectionType::SOFTWARE:
    return CONNECTION_TYPE::SOFTWARE;
  case ConnectionType::SERIALPORT:
    return CONNECTION_TYPE::SERIALPORT;
  case ConnectionType::PCI:
    return CONNECTION_TYPE::PCI;
  case ConnectionType::USB:
    return CONNECTION_TYPE::USB;
  case ConnectionType::ETHERNET:
    return CONNECTION_TYPE::ETHERNET;
  case ConnectionType::FIREWIRE:
    return CONNECTION_TYPE::FIREWIRE;
  case ConnectionType::BLUETOOTH:
    return CONNECTION_TYPE::BLUETOOTH;
  default:
    // TODO: throw exception for unknown enum value?
    return CONNECTION_TYPE::SOFTWARE;
  }
}

namespace msclr::interop {
template <>
inline ConnectionType
marshal_as<ConnectionType, CONNECTION_TYPE>(const CONNECTION_TYPE &from) {
  return connectionTypeFromManaged(from);
}
template <>
inline CONNECTION_TYPE
marshal_as<CONNECTION_TYPE, ConnectionType>(const ConnectionType &from) {
  return connectionTypeToManaged(from);
}
} // namespace msclr::interop

// converting JSR-SDK::PropertyUnits <-> JSRDotNETSDK::PROPERTY_UNITS
static PropertyUnits propertyUnitsFromManaged(PROPERTY_UNITS units) {
  switch (units) {
  case PROPERTY_UNITS::UNIT_NONE:
    return PropertyUnits::UNIT_NONE;
  case PROPERTY_UNITS::UNIT_HERTZ:
    return PropertyUnits::UNIT_HERTZ;
  case PROPERTY_UNITS::UNIT_KILOHERTZ:
    return PropertyUnits::UNIT_KILOHERTZ;
  case PROPERTY_UNITS::UNIT_MEGAHERTZ:
    return PropertyUnits::UNIT_MEGAHERTZ;
  case PROPERTY_UNITS::UNIT_MICROJOULES:
    return PropertyUnits::UNIT_MICROJOULES;
  case PROPERTY_UNITS::UNIT_JOULES:
    return PropertyUnits::UNIT_JOULES;
  case PROPERTY_UNITS::UNIT_WATTS:
    return PropertyUnits::UNIT_WATTS;
  case PROPERTY_UNITS::UNIT_DB:
    return PropertyUnits::UNIT_DB;
  case PROPERTY_UNITS::UNIT_VOLTS:
    return PropertyUnits::UNIT_VOLTS;
  case PROPERTY_UNITS::UNIT_OHMS:
    return PropertyUnits::UNIT_OHMS;
  case PROPERTY_UNITS::UNIT_PICOFARADS:
    return PropertyUnits::UNIT_PICOFARADS;
  default:
    return PropertyUnits::UNKNOWN;
  }
}
static PROPERTY_UNITS propertyUnitsToManaged(PropertyUnits units) {
  switch (units) {
  case PropertyUnits::UNIT_NONE:
    return PROPERTY_UNITS::UNIT_NONE;
  case PropertyUnits::UNIT_HERTZ:
    return PROPERTY_UNITS::UNIT_HERTZ;
  case PropertyUnits::UNIT_KILOHERTZ:
    return PROPERTY_UNITS::UNIT_KILOHERTZ;
  case PropertyUnits::UNIT_MEGAHERTZ:
    return PROPERTY_UNITS::UNIT_MEGAHERTZ;
  case PropertyUnits::UNIT_MICROJOULES:
    return PROPERTY_UNITS::UNIT_MICROJOULES;
  case PropertyUnits::UNIT_JOULES:
    return PROPERTY_UNITS::UNIT_JOULES;
  case PropertyUnits::UNIT_WATTS:
    return PROPERTY_UNITS::UNIT_WATTS;
  case PropertyUnits::UNIT_DB:
    return PROPERTY_UNITS::UNIT_DB;
  case PropertyUnits::UNIT_VOLTS:
    return PROPERTY_UNITS::UNIT_VOLTS;
  case PropertyUnits::UNIT_OHMS:
    return PROPERTY_UNITS::UNIT_OHMS;
  case PropertyUnits::UNIT_PICOFARADS:
    return PROPERTY_UNITS::UNIT_PICOFARADS;
  default:
    // TODO: throw exception for unknown enum value?
    return PROPERTY_UNITS::UNIT_NONE;
  }
}

// converting JSR-SDK::PulserPropertyRoles <-> JSRDotNETSDK::PulserPropertyRole
static PulserPropertyRoles
pulserPropertyRoleFromManaged(PulserPropertyRole role) {
  switch (role) {
  case PulserPropertyRole::DIRECT:
    return PulserPropertyRoles::DIRECT;
  case PulserPropertyRole::DIRECTSUPPORTED:
    return PulserPropertyRoles::DIRECTSUPPORTED;
  case PulserPropertyRole::SPECIFICVALUESUPPORTED:
    return PulserPropertyRoles::SPECIFICVALUESUPPORTED;
  case PulserPropertyRole::DIRECTVALUES:
    return PulserPropertyRoles::DIRECTVALUES;
  case PulserPropertyRole::STEPSIZE:
    return PulserPropertyRoles::STEPSIZE;
  case PulserPropertyRole::STEPSIZESUPPORTED:
    return PulserPropertyRoles::STEPSIZESUPPORTED;
  case PulserPropertyRole::INDEX:
    return PulserPropertyRoles::INDEX;
  case PulserPropertyRole::INDEXMAX:
    return PulserPropertyRoles::INDEXMAX;
  case PulserPropertyRole::INDEXSUPPORTED:
    return PulserPropertyRoles::INDEXSUPPORTED;
  case PulserPropertyRole::MAX:
    return PulserPropertyRoles::MAX;
  case PulserPropertyRole::MIN:
    return PulserPropertyRoles::MIN;
  case PulserPropertyRole::INDEXVALUES:
    return PulserPropertyRoles::INDEXVALUES;
  case PulserPropertyRole::NUMERATOR:
    return PulserPropertyRoles::NUMERATOR;
  case PulserPropertyRole::OTHER:
    return PulserPropertyRoles::OTHER;
  default:
    return PulserPropertyRoles::UKNOWN;
  }
}
static PulserPropertyRole
pulserPropertyRoleToManaged(PulserPropertyRoles role) {
  switch (role) {
  case PulserPropertyRoles::DIRECT:
    return PulserPropertyRole::DIRECT;
  case PulserPropertyRoles::DIRECTSUPPORTED:
    return PulserPropertyRole::DIRECTSUPPORTED;
  case PulserPropertyRoles::SPECIFICVALUESUPPORTED:
    return PulserPropertyRole::SPECIFICVALUESUPPORTED;
  case PulserPropertyRoles::DIRECTVALUES:
    return PulserPropertyRole::DIRECTVALUES;
  case PulserPropertyRoles::STEPSIZE:
    return PulserPropertyRole::STEPSIZE;
  case PulserPropertyRoles::STEPSIZESUPPORTED:
    return PulserPropertyRole::STEPSIZESUPPORTED;
  case PulserPropertyRoles::INDEX:
    return PulserPropertyRole::INDEX;
  case PulserPropertyRoles::INDEXMAX:
    return PulserPropertyRole::INDEXMAX;
  case PulserPropertyRoles::INDEXSUPPORTED:
    return PulserPropertyRole::INDEXSUPPORTED;
  case PulserPropertyRoles::MAX:
    return PulserPropertyRole::MAX;
  case PulserPropertyRoles::MIN:
    return PulserPropertyRole::MIN;
  case PulserPropertyRoles::INDEXVALUES:
    return PulserPropertyRole::INDEXVALUES;
  case PulserPropertyRoles::NUMERATOR:
    return PulserPropertyRole::NUMERATOR;
  case PulserPropertyRoles::OTHER:
    return PulserPropertyRole::OTHER;
  default:
    // TODO: throw exception for unknown enum value?
    return PulserPropertyRole::OTHER;
  }
}
