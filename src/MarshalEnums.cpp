#pragma once

#include "JSR-SDK/enums/ConnectionType.h"
#include "JSR-SDK/enums/DiscoveryStateFlagsC.h"
#include "JSR-SDK/enums/ErrorCode.h"
#include "JSR-SDK/enums/IsPulsing.h"
#include "JSR-SDK/enums/ManagerState.h"
#include "JSR-SDK/enums/NotifyType.h"
#include "JSR-SDK/enums/PowerLimit.h"
#include "JSR-SDK/enums/PropertyUnits.h"
#include "JSR-SDK/enums/PulserImpedance.h"
#include "JSR-SDK/enums/PulserPropertyDataTypeC.h"
#include "JSR-SDK/enums/PulserPropertyRoles.h"
#include "JSR-SDK/enums/PulserReceiverStateC.h"
#include "JSR-SDK/enums/ReceiverMode.h"
#include "JSR-SDK/enums/StatusChange.h"
#include "JSR-SDK/enums/TriggerImpedance.h"
#include "JSR-SDK/enums/TriggerPolarity.h"
#include "JSR-SDK/enums/TriggerSource.h"

#include <msclr/marshal_cppstd.h>

using namespace JSRDotNETSDK;

// JSRDotNETSDK::PulserReceiverState uses ERROR as an enum value, which is a
// macro in windows.h
// So incredibly stupid
#undef ERROR

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

// converting JSR-SDK::StatusChange <-> JSRDotNETSDK::STATUS_CHANGE
static StatusChange statusChangeFromManaged(STATUS_CHANGE change) {
  switch (change) {
  case STATUS_CHANGE::INSTRUMENT_DISCONNECT:
    return StatusChange::INSTRUMENT_DISCONNECT;
  case STATUS_CHANGE::PROPERTY_CHANGE:
    return StatusChange::PROPERTY_CHANGE;
  case STATUS_CHANGE::OPERATIONAL_FAULT:
    return StatusChange::OPERATIONAL_FAULT;
  case STATUS_CHANGE::STATE_CHANGE:
    return StatusChange::STATE_CHANGE;
  default:
    return StatusChange::UNKNOWN;
  }
}

static STATUS_CHANGE statusChangeToManaged(StatusChange change) {
  switch (change) {
  case StatusChange::INSTRUMENT_DISCONNECT:
    return STATUS_CHANGE::INSTRUMENT_DISCONNECT;
  case StatusChange::PROPERTY_CHANGE:
    return STATUS_CHANGE::PROPERTY_CHANGE;
  case StatusChange::OPERATIONAL_FAULT:
    return STATUS_CHANGE::OPERATIONAL_FAULT;
  case StatusChange::STATE_CHANGE:
    return STATUS_CHANGE::STATE_CHANGE;
  default:
    // TODO: throw exception for unknown enum value?
    return STATUS_CHANGE::INSTRUMENT_DISCONNECT;
  }
}

// converting JSR-SDK::PulserReceiverStateC <->
// JSRDotNETSDK::PulserReceiverState
static PulserReceiverStateC
pulserReceiverStateFromManaged(PulserReceiverState state) {
  switch (state) {
  case PulserReceiverState::DETECTED:
    return PulserReceiverStateC::DETECTED;
  case PulserReceiverState::INITIALIZING:
    return PulserReceiverStateC::INITIALIZING;
  case PulserReceiverState::READY:
    return PulserReceiverStateC::READY;
  case PulserReceiverState::ERROR:
    return PulserReceiverStateC::ERRORC;
  case PulserReceiverState::NO_PULSER:
    return PulserReceiverStateC::NO_PULSER;
  case PulserReceiverState::DETACHED:
    return PulserReceiverStateC::DETACHED;
  default:
    return PulserReceiverStateC::UNKNOWN;
  }
}

static PulserReceiverState
pulserReceiverStateToManaged(PulserReceiverStateC state) {
  switch (state) {
  case PulserReceiverStateC::DETECTED:
    return PulserReceiverState::DETECTED;
  case PulserReceiverStateC::INITIALIZING:
    return PulserReceiverState::INITIALIZING;
  case PulserReceiverStateC::READY:
    return PulserReceiverState::READY;
  case PulserReceiverStateC::ERRORC:
    return PulserReceiverState::ERROR;
  case PulserReceiverStateC::NO_PULSER:
    return PulserReceiverState::NO_PULSER;
  case PulserReceiverStateC::DETACHED:
    return PulserReceiverState::DETACHED;
  default:
    // TODO: throw exception for unknown enum value?
    return PulserReceiverState::ERROR;
  }
}

// converting JSR-SDK::PulserPropertyDataTypeC <->
// JSRDotNETSDK::PulserPropertyDataType

static PulserPropertyDataTypeC
pulserPropertyDataTypeFromManaged(PulserPropertyDataType type) {
  switch (type) {
  case PulserPropertyDataType::INTEGER:
    return PulserPropertyDataTypeC::INTEGER;
  case PulserPropertyDataType::BYTE:
    return PulserPropertyDataTypeC::BYTE;
  case PulserPropertyDataType::DOUBLE:
    return PulserPropertyDataTypeC::DOUBLE;
  case PulserPropertyDataType::BOOLEAN:
    return PulserPropertyDataTypeC::BOOLEAN;
  case PulserPropertyDataType::STRING:
    return PulserPropertyDataTypeC::STRING;
  case PulserPropertyDataType::BYTE_ARRAY:
    return PulserPropertyDataTypeC::BYTE_ARRAY;
  case PulserPropertyDataType::INTEGER_ARRAY:
    return PulserPropertyDataTypeC::INTEGER_ARRAY;
  case PulserPropertyDataType::DOUBLE_ARRAY:
    return PulserPropertyDataTypeC::DOUBLE_ARRAY;
  case PulserPropertyDataType::BOOLEAN_ARRAY:
    return PulserPropertyDataTypeC::BOOLEAN_ARRAY;
  case PulserPropertyDataType::STRING_ARRAY:
    return PulserPropertyDataTypeC::STRING_ARRAY;
  case PulserPropertyDataType::ENUM:
    return PulserPropertyDataTypeC::ENUM;
  case PulserPropertyDataType::ID:
    return PulserPropertyDataTypeC::ID;
  case PulserPropertyDataType::OTHER:
    return PulserPropertyDataTypeC::OTHER;
  default:
    return PulserPropertyDataTypeC::UNKNOWN;
  }
}

static PulserPropertyDataType
pulserPropertyDataTypeToManaged(PulserPropertyDataTypeC type) {
  switch (type) {
  case PulserPropertyDataTypeC::INTEGER:
    return PulserPropertyDataType::INTEGER;
  case PulserPropertyDataTypeC::BYTE:
    return PulserPropertyDataType::BYTE;
  case PulserPropertyDataTypeC::DOUBLE:
    return PulserPropertyDataType::DOUBLE;
  case PulserPropertyDataTypeC::BOOLEAN:
    return PulserPropertyDataType::BOOLEAN;
  case PulserPropertyDataTypeC::STRING:
    return PulserPropertyDataType::STRING;
  case PulserPropertyDataTypeC::BYTE_ARRAY:
    return PulserPropertyDataType::BYTE_ARRAY;
  case PulserPropertyDataTypeC::INTEGER_ARRAY:
    return PulserPropertyDataType::INTEGER_ARRAY;
  case PulserPropertyDataTypeC::DOUBLE_ARRAY:
    return PulserPropertyDataType::DOUBLE_ARRAY;
  case PulserPropertyDataTypeC::BOOLEAN_ARRAY:
    return PulserPropertyDataType::BOOLEAN_ARRAY;
  case PulserPropertyDataTypeC::STRING_ARRAY:
    return PulserPropertyDataType::STRING_ARRAY;
  case PulserPropertyDataTypeC::ENUM:
    return PulserPropertyDataType::ENUM;
  case PulserPropertyDataTypeC::ID:
    return PulserPropertyDataType::ID;
  case PulserPropertyDataTypeC::OTHER:
    return PulserPropertyDataType::OTHER;
  default:
    // TODO: throw exception for unknown enum value?
    return PulserPropertyDataType::OTHER;
  }
}

// converting JSR-SDK::ErrorCode <-> JSRDotNETSDK::ERROR_CODE
static ErrorCode errorCodeFromManaged(ERROR_CODE code) {
  switch (code) {
  case ERROR_CODE::UNKNOWN:
    return ErrorCode::UNKNOWN;
  case ERROR_CODE::DEVICE_NOT_FOUND:
    return ErrorCode::DEVICE_NOT_FOUND;
  case ERROR_CODE::PLUGINS_NOT_LOADED:
    return ErrorCode::PLUGINS_NOT_LOADED;
  case ERROR_CODE::NOT_FOUND:
    return ErrorCode::NOT_FOUND;
  case ERROR_CODE::COMMUNICATION_FAILURE:
    return ErrorCode::COMMUNICATION_FAILURE;
  case ERROR_CODE::RESP_ADDRESS_MISMATCH:
    return ErrorCode::RESP_ADDRESS_MISMATCH;
  case ERROR_CODE::RESP_INVALID_LENGTH:
    return ErrorCode::RESP_INVALID_LENGTH;
  case ERROR_CODE::PORT_OPEN_FAILED:
    return ErrorCode::PORT_OPEN_FAILED;
  case ERROR_CODE::TIMEOUT:
    return ErrorCode::TIMEOUT;
  case ERROR_CODE::VALUE_OUT_RANGE:
    return ErrorCode::VALUE_OUT_RANGE;
  case ERROR_CODE::OPERATION_NOT_SUPPORTED:
    return ErrorCode::OPERATION_NOT_SUPPORTED;
  case ERROR_CODE::HARDWARE_FAILURE:
    return ErrorCode::HARDWARE_FAILURE;
  case ERROR_CODE::INTERNAL_SOFTWARE_FAULT:
    return ErrorCode::INTERNAL_SOFTWARE_FAULT;
  case ERROR_CODE::PROPERTY_WRITE_NOT_FINISHED_YET:
    return ErrorCode::PROPERTY_WRITE_NOT_FINISHED_YET;
  case ERROR_CODE::NO_PULSER_RCVR_SELECTED:
    return ErrorCode::NO_PULSER_RCVR_SELECTED;
  case ERROR_CODE::INVALID_ARGUMENT:
    return ErrorCode::INVALID_ARGUMENT;
  case ERROR_CODE::NULL_POINTER:
    return ErrorCode::NULL_POINTER;
  case ERROR_CODE::OPERATION_FAILED:
    return ErrorCode::OPERATION_FAILED;
  case ERROR_CODE::VERSION_MISMATCH:
    return ErrorCode::VERSION_MISMATCH;
  case ERROR_CODE::PLUGIN_LOAD_ERROR:
    return ErrorCode::PLUGIN_LOAD_ERROR;
  case ERROR_CODE::DEVICE_DISCOVERY_ERROR:
    return ErrorCode::DEVICE_DISCOVERY_ERROR;
  case ERROR_CODE::DEVICE_NOT_READY:
    return ErrorCode::DEVICE_NOT_READY;
  case ERROR_CODE::SUCCESS:
    return ErrorCode::SUCCESS;
  default:
    return ErrorCode::UNKNOWN;
  }
}

static ERROR_CODE errorCodeToManaged(ErrorCode code) {
  switch (code) {
  case ErrorCode::UNKNOWN:
    return ERROR_CODE::UNKNOWN;
  case ErrorCode::DEVICE_NOT_FOUND:
    return ERROR_CODE::DEVICE_NOT_FOUND;
  case ErrorCode::PLUGINS_NOT_LOADED:
    return ERROR_CODE::PLUGINS_NOT_LOADED;
  case ErrorCode::NOT_FOUND:
    return ERROR_CODE::NOT_FOUND;
  case ErrorCode::COMMUNICATION_FAILURE:
    return ERROR_CODE::COMMUNICATION_FAILURE;
  case ErrorCode::RESP_ADDRESS_MISMATCH:
    return ERROR_CODE::RESP_ADDRESS_MISMATCH;
  case ErrorCode::RESP_INVALID_LENGTH:
    return ERROR_CODE::RESP_INVALID_LENGTH;
  case ErrorCode::PORT_OPEN_FAILED:
    return ERROR_CODE::PORT_OPEN_FAILED;
  case ErrorCode::TIMEOUT:
    return ERROR_CODE::TIMEOUT;
  case ErrorCode::VALUE_OUT_RANGE:
    return ERROR_CODE::VALUE_OUT_RANGE;
  case ErrorCode::OPERATION_NOT_SUPPORTED:
    return ERROR_CODE::OPERATION_NOT_SUPPORTED;
  case ErrorCode::HARDWARE_FAILURE:
    return ERROR_CODE::HARDWARE_FAILURE;
  case ErrorCode::INTERNAL_SOFTWARE_FAULT:
    return ERROR_CODE::INTERNAL_SOFTWARE_FAULT;
  case ErrorCode::PROPERTY_WRITE_NOT_FINISHED_YET:
    return ERROR_CODE::PROPERTY_WRITE_NOT_FINISHED_YET;
  case ErrorCode::NO_PULSER_RCVR_SELECTED:
    return ERROR_CODE::NO_PULSER_RCVR_SELECTED;
  case ErrorCode::INVALID_ARGUMENT:
    return ERROR_CODE::INVALID_ARGUMENT;
  case ErrorCode::NULL_POINTER:
    return ERROR_CODE::NULL_POINTER;
  case ErrorCode::OPERATION_FAILED:
    return ERROR_CODE::OPERATION_FAILED;
  case ErrorCode::VERSION_MISMATCH:
    return ERROR_CODE::VERSION_MISMATCH;
  case ErrorCode::PLUGIN_LOAD_ERROR:
    return ERROR_CODE::PLUGIN_LOAD_ERROR;
  case ErrorCode::DEVICE_DISCOVERY_ERROR:
    return ERROR_CODE::DEVICE_DISCOVERY_ERROR;
  case ErrorCode::DEVICE_NOT_READY:
    return ERROR_CODE::DEVICE_NOT_READY;
  case ErrorCode::SUCCESS:
    return ERROR_CODE::SUCCESS;
  default:
    // TODO: throw exception for unknown enum value?
    return ERROR_CODE::UNKNOWN;
  }
}

// converting JSR-SDK::DiscoveryStateFlagsC <->
// JSRDotNETSDK::DiscoveryStateFlags
static DiscoveryStateFlagsC
discoveryStateFlagsFromManaged(DiscoveryStateFlags flags) {
  switch (flags) {
  case DiscoveryStateFlags::NONE:
    return DiscoveryStateFlagsC::NONE;
  case DiscoveryStateFlags::ACTIVE:
    return DiscoveryStateFlagsC::ACTIVE;
  case DiscoveryStateFlags::PENDING:
    return DiscoveryStateFlagsC::PENDING;
  case DiscoveryStateFlags::STOPPED_ON_ERR:
    return DiscoveryStateFlagsC::STOPPED_ON_ERR;
  default:
    return DiscoveryStateFlagsC::UNKNOWN;
  }
}

static DiscoveryStateFlags
discoveryStateFlagsToManaged(DiscoveryStateFlagsC flags) {
  switch (flags) {
  case DiscoveryStateFlagsC::NONE:
    return DiscoveryStateFlags::NONE;
  case DiscoveryStateFlagsC::ACTIVE:
    return DiscoveryStateFlags::ACTIVE;
  case DiscoveryStateFlagsC::PENDING:
    return DiscoveryStateFlags::PENDING;
  case DiscoveryStateFlagsC::STOPPED_ON_ERR:
    return DiscoveryStateFlags::STOPPED_ON_ERR;
  default:
    // TODO: throw exception for unknown enum value?
    return DiscoveryStateFlags::NONE;
  }
}

// converting JSR-SDK::NotifyType <-> JSRDotNETSDK::NOTIFY_TYPE
static NotifyType notifyTypeFromManaged(NOTIFY_TYPE type) {
  switch (type) {
  case NOTIFY_TYPE::PULSER_RCVR_DISCOVERED:
    return NotifyType::PULSER_RCVR_DISCOVERED;
  case NOTIFY_TYPE::CURRENT_PULSER_RCVR_CHANGED:
    return NotifyType::CURRENT_PULSER_RCVR_CHANGED;
  case NOTIFY_TYPE::PULSER_RCVR_DETACH:
    return NotifyType::PULSER_RCVR_DETACH;
  case NOTIFY_TYPE::DISCOVERY_STARTED:
    return NotifyType::DISCOVERY_STARTED;
  case NOTIFY_TYPE::DISCOVERY_COMPLETE:
    return NotifyType::DISCOVERY_COMPLETE;
  case NOTIFY_TYPE::PROPERTY_CHANGE:
    return NotifyType::PROPERTY_CHANGE;
  case NOTIFY_TYPE::STATE_CHANGE:
    return NotifyType::STATE_CHANGE;
  case NOTIFY_TYPE::ERROR:
    return NotifyType::ERROR;
  default:
    return NotifyType::UNKNOWN;
  }
}

static NOTIFY_TYPE notifyTypeToManaged(NotifyType type) {
  switch (type) {
  case NotifyType::PULSER_RCVR_DISCOVERED:
    return NOTIFY_TYPE::PULSER_RCVR_DISCOVERED;
  case NotifyType::CURRENT_PULSER_RCVR_CHANGED:
    return NOTIFY_TYPE::CURRENT_PULSER_RCVR_CHANGED;
  case NotifyType::PULSER_RCVR_DETACH:
    return NOTIFY_TYPE::PULSER_RCVR_DETACH;
  case NotifyType::DISCOVERY_STARTED:
    return NOTIFY_TYPE::DISCOVERY_STARTED;
  case NotifyType::DISCOVERY_COMPLETE:
    return NOTIFY_TYPE::DISCOVERY_COMPLETE;
  case NotifyType::PROPERTY_CHANGE:
    return NOTIFY_TYPE::PROPERTY_CHANGE;
  case NotifyType::STATE_CHANGE:
    return NOTIFY_TYPE::STATE_CHANGE;
  case NotifyType::ERROR:
    return NOTIFY_TYPE::ERROR;
  default:
    // TODO: throw exception for unknown enum value?
    return NOTIFY_TYPE::ERROR;
  }
}
