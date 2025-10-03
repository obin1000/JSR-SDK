#pragma once

#include "JSR-SDK/enums/C_CONNECTION_TYPE.h"
#include "JSR-SDK/enums/C_DISCOVERY_STATE_FLAGS.h"
#include "JSR-SDK/enums/C_ERROR_CODE.h"
#include "JSR-SDK/enums/C_IS_PULSING.h"
#include "JSR-SDK/enums/C_MANAGER_STATE.h"
#include "JSR-SDK/enums/C_NOTIFY_TYPE.h"
#include "JSR-SDK/enums/C_POWER_LIMIT.h"
#include "JSR-SDK/enums/C_PROPERTY_UNITS.h"
#include "JSR-SDK/enums/C_PULSER_IMPEDANCE.h"
#include "JSR-SDK/enums/C_PULSER_PROPERTY_DATA_TYPE.h"
#include "JSR-SDK/enums/C_PULSER_PROPERTY_ROLE.h"
#include "JSR-SDK/enums/C_PULSER_RECEIVER_STATE.h"
#include "JSR-SDK/enums/C_RECEIVER_MODE.h"
#include "JSR-SDK/enums/C_STATUS_CHANGE.h"
#include "JSR-SDK/enums/C_TRIGGER_IMPEDANCE.h"
#include "JSR-SDK/enums/C_TRIGGER_POLARITY.h"
#include "JSR-SDK/enums/C_TRIGGER_SOURCE.h"

#include <msclr/marshal_cppstd.h>
#include <stdexcept>

using namespace JSRDotNETSDK;
#undef ERROR

// RECEIVER_MODE <-> C_RECEIVER_MODE
inline constexpr static C_RECEIVER_MODE
receiverModeFromManaged(const RECEIVER_MODE &mode) {
  switch (mode) {
  case RECEIVER_MODE::BOTH:
    return C_RECEIVER_MODE::BOTH;
  case RECEIVER_MODE::ECHO:
    return C_RECEIVER_MODE::ECHO;
  case RECEIVER_MODE::LAST_VAL:
    return C_RECEIVER_MODE::LAST_VAL;
  case RECEIVER_MODE::THRU:
    return C_RECEIVER_MODE::THRU;
  default:
    throw std::invalid_argument("Unknown RECEIVER_MODE value");
  }
}
inline constexpr static RECEIVER_MODE
receiverModeToManaged(const C_RECEIVER_MODE &mode) {
  switch (mode) {
  case C_RECEIVER_MODE::BOTH:
    return RECEIVER_MODE::BOTH;
  case C_RECEIVER_MODE::ECHO:
    return RECEIVER_MODE::ECHO;
  case C_RECEIVER_MODE::LAST_VAL:
    return RECEIVER_MODE::LAST_VAL;
  case C_RECEIVER_MODE::THRU:
    return RECEIVER_MODE::THRU;
  default:
    throw std::invalid_argument("Unknown C_RECEIVER_MODE value");
  }
}

// converting JSRDotNETSDK::TRIGGER_SOURCE <-> C_TRIGGER_SOURCE
inline constexpr static C_TRIGGER_SOURCE
triggerSourceFromManaged(const TRIGGER_SOURCE &source) {
  switch (source) {
  case TRIGGER_SOURCE::INTERNAL:
    return C_TRIGGER_SOURCE::INTERNAL;
  case TRIGGER_SOURCE::EXTERNAL:
    return C_TRIGGER_SOURCE::EXTERNAL;
  case TRIGGER_SOURCE::SLAVE:
    return C_TRIGGER_SOURCE::SLAVE;
  case TRIGGER_SOURCE::LAST_VAL:
    return C_TRIGGER_SOURCE::LAST_VAL;
  default:
    throw std::invalid_argument("Unknown TRIGGER_SOURCE value");
  }
}
inline constexpr static TRIGGER_SOURCE
triggerSourceToManaged(const C_TRIGGER_SOURCE &source) {
  switch (source) {
  case C_TRIGGER_SOURCE::INTERNAL:
    return TRIGGER_SOURCE::INTERNAL;
  case C_TRIGGER_SOURCE::EXTERNAL:
    return TRIGGER_SOURCE::EXTERNAL;
  case C_TRIGGER_SOURCE::SLAVE:
    return TRIGGER_SOURCE::SLAVE;
  case C_TRIGGER_SOURCE::LAST_VAL:
    return TRIGGER_SOURCE::LAST_VAL;
  default:
    throw std::invalid_argument("Unknown C_TRIGGER_SOURCE value");
  }
}

// converting JSRDotNETSDK::TRIGGER_POLARITY <-> C_TRIGGER_POLARITY
inline constexpr static C_TRIGGER_POLARITY
triggerPolarityFromManaged(const TRIGGER_POLARITY &source) {
  switch (source) {
  case TRIGGER_POLARITY::FALLING:
    return C_TRIGGER_POLARITY::FALLING;
  case TRIGGER_POLARITY::RISING:
    return C_TRIGGER_POLARITY::RISING;
  case TRIGGER_POLARITY::LAST_VAL:
    return C_TRIGGER_POLARITY::LAST_VAL;
  default:
    throw std::invalid_argument("Unknown TRIGGER_POLARITY value");
  }
}
inline constexpr static TRIGGER_POLARITY
triggerPolarityToManaged(const C_TRIGGER_POLARITY &source) {
  switch (source) {
  case C_TRIGGER_POLARITY::FALLING:
    return TRIGGER_POLARITY::FALLING;
  case C_TRIGGER_POLARITY::RISING:
    return TRIGGER_POLARITY::RISING;
  case C_TRIGGER_POLARITY::LAST_VAL:
    return TRIGGER_POLARITY::LAST_VAL;
  default:
    throw std::invalid_argument("Unknown C_TRIGGER_POLARITY value");
  }
}

// converting JSRDotNETSDK::TRIGGER_IMPEDANCE <-> C_TRIGGER_IMPEDANCE
inline constexpr static C_TRIGGER_IMPEDANCE
triggerImpedanceFromManaged(const TRIGGER_IMPEDANCE &source) {
  switch (source) {
  case TRIGGER_IMPEDANCE::HIGH_Z:
    return C_TRIGGER_IMPEDANCE::HIGH_Z;
  case TRIGGER_IMPEDANCE::LOW_Z:
    return C_TRIGGER_IMPEDANCE::LOW_Z;
  case TRIGGER_IMPEDANCE::LAST_VAL:
    return C_TRIGGER_IMPEDANCE::LAST_VAL;
  default:
    throw std::invalid_argument("Unknown TRIGGER_IMPEDANCE value");
  }
}
inline constexpr static TRIGGER_IMPEDANCE
triggerImpedanceToManaged(const C_TRIGGER_IMPEDANCE &source) {
  switch (source) {
  case C_TRIGGER_IMPEDANCE::HIGH_Z:
    return TRIGGER_IMPEDANCE::HIGH_Z;
  case C_TRIGGER_IMPEDANCE::LOW_Z:
    return TRIGGER_IMPEDANCE::LOW_Z;
  case C_TRIGGER_IMPEDANCE::LAST_VAL:
    return TRIGGER_IMPEDANCE::LAST_VAL;
  default:
    throw std::invalid_argument("Unknown C_TRIGGER_IMPEDANCE value");
  }
}

// converting JSRDotNETSDK::PULSER_IMPEDANCE <-> C_PULSER_IMPEDANCE
inline constexpr static C_PULSER_IMPEDANCE
pulserImpedanceFromManaged(const PULSER_IMPEDANCE &source) {
  switch (source) {
  case PULSER_IMPEDANCE::HIGH_Z:
    return C_PULSER_IMPEDANCE::HIGH_Z;
  case PULSER_IMPEDANCE::LOW_Z:
    return C_PULSER_IMPEDANCE::LOW_Z;
  case PULSER_IMPEDANCE::LAST_VAL:
    return C_PULSER_IMPEDANCE::LAST_VAL;
  default:
    throw std::invalid_argument("Unknown PULSER_IMPEDANCE value");
  }
}
inline constexpr static PULSER_IMPEDANCE
pulserImpedanceToManaged(const C_PULSER_IMPEDANCE &source) {
  switch (source) {
  case C_PULSER_IMPEDANCE::HIGH_Z:
    return PULSER_IMPEDANCE::HIGH_Z;
  case C_PULSER_IMPEDANCE::LOW_Z:
    return PULSER_IMPEDANCE::LOW_Z;
  case C_PULSER_IMPEDANCE::LAST_VAL:
    return PULSER_IMPEDANCE::LAST_VAL;
  default:
    throw std::invalid_argument("Unknown C_PULSER_IMPEDANCE value");
  }
}

// converting JSRDotNETSDK::IS_PULSING <-> C_IS_PULSING
inline constexpr static C_IS_PULSING
isPulsingFromManaged(const IS_PULSING &source) {
  switch (source) {
  case IS_PULSING::ACTIVE:
    return C_IS_PULSING::ACTIVE;
  case IS_PULSING::INACTIVE:
    return C_IS_PULSING::INACTIVE;
  case IS_PULSING::UNKNOWN:
    return C_IS_PULSING::UNKNOWN;
  default:
    throw std::invalid_argument("Unknown IS_PULSING value");
  }
}
inline constexpr static IS_PULSING
isPulsingToManaged(const C_IS_PULSING &source) {
  switch (source) {
  case C_IS_PULSING::ACTIVE:
    return IS_PULSING::ACTIVE;
  case C_IS_PULSING::INACTIVE:
    return IS_PULSING::INACTIVE;
  case C_IS_PULSING::UNKNOWN:
    return IS_PULSING::UNKNOWN;
  default:
    throw std::invalid_argument("Unknown C_IS_PULSING value");
  }
}

// converting JSRDotNETSDK::POWER_LIMIT <-> C_POWER_LIMIT
inline constexpr static C_POWER_LIMIT
powerLimitFromManaged(const POWER_LIMIT &source) {
  switch (source) {
  case POWER_LIMIT::OVER_LIMIT:
    return C_POWER_LIMIT::OVER_LIMIT;
  case POWER_LIMIT::WITHIN_LIMIT:
    return C_POWER_LIMIT::WITHIN_LIMIT;
  default:
    throw std::invalid_argument("Unknown POWER_LIMIT value");
  }
}
inline constexpr static POWER_LIMIT
powerLimitToManaged(const C_POWER_LIMIT &source) {
  switch (source) {
  case C_POWER_LIMIT::OVER_LIMIT:
    return POWER_LIMIT::OVER_LIMIT;
  case C_POWER_LIMIT::WITHIN_LIMIT:
    return POWER_LIMIT::WITHIN_LIMIT;
  default:
    throw std::invalid_argument("Unknown C_POWER_LIMIT value");
  }
}

// converting JSRDotNETSDK::MANAGER_STATE <-> C_MANAGER_STATE
inline constexpr static C_MANAGER_STATE
managerStateFromManaged(const JSRDotNETManager::MANAGER_STATE &state) {
  switch (state) {
  case JSRDotNETManager::MANAGER_STATE::NOT_STARTED:
    return C_MANAGER_STATE::NOT_STARTED;
  case JSRDotNETManager::MANAGER_STATE::PLUGINS_LOADED:
    return C_MANAGER_STATE::PLUGINS_LOADED;
  case JSRDotNETManager::MANAGER_STATE::RUNNING:
    return C_MANAGER_STATE::RUNNING;
  case JSRDotNETManager::MANAGER_STATE::SHUTTING_DOWN:
    return C_MANAGER_STATE::SHUTTING_DOWN;
  default:
    throw std::invalid_argument("Unknown MANAGER_STATE value");
  }
}
inline constexpr static JSRDotNETManager::MANAGER_STATE
managerStateToManaged(const C_MANAGER_STATE &state) {
  switch (state) {
  case C_MANAGER_STATE::NOT_STARTED:
    return JSRDotNETManager::MANAGER_STATE::NOT_STARTED;
  case C_MANAGER_STATE::PLUGINS_LOADED:
    return JSRDotNETManager::MANAGER_STATE::PLUGINS_LOADED;
  case C_MANAGER_STATE::RUNNING:
    return JSRDotNETManager::MANAGER_STATE::RUNNING;
  case C_MANAGER_STATE::SHUTTING_DOWN:
    return JSRDotNETManager::MANAGER_STATE::SHUTTING_DOWN;
  default:
    throw std::invalid_argument("Unknown C_MANAGER_STATE value");
  }
}

// converting JSRDotNETSDK::CONNECTION_TYPE <-> C_CONNECTION_TYPE
inline constexpr static C_CONNECTION_TYPE
connectionTypeFromManaged(const CONNECTION_TYPE &connection) {
  switch (connection) {
  case CONNECTION_TYPE::SOFTWARE:
    return C_CONNECTION_TYPE::SOFTWARE;
  case CONNECTION_TYPE::SERIALPORT:
    return C_CONNECTION_TYPE::SERIALPORT;
  case CONNECTION_TYPE::PCI:
    return C_CONNECTION_TYPE::PCI;
  case CONNECTION_TYPE::USB:
    return C_CONNECTION_TYPE::USB;
  case CONNECTION_TYPE::ETHERNET:
    return C_CONNECTION_TYPE::ETHERNET;
  case CONNECTION_TYPE::FIREWIRE:
    return C_CONNECTION_TYPE::FIREWIRE;
  case CONNECTION_TYPE::BLUETOOTH:
    return C_CONNECTION_TYPE::BLUETOOTH;
  default:
    throw std::invalid_argument("Unknown CONNECTION_TYPE value");
  }
}
inline constexpr static CONNECTION_TYPE
connectionTypeToManaged(const C_CONNECTION_TYPE &connection) {
  switch (connection) {
  case C_CONNECTION_TYPE::SOFTWARE:
    return CONNECTION_TYPE::SOFTWARE;
  case C_CONNECTION_TYPE::SERIALPORT:
    return CONNECTION_TYPE::SERIALPORT;
  case C_CONNECTION_TYPE::PCI:
    return CONNECTION_TYPE::PCI;
  case C_CONNECTION_TYPE::USB:
    return CONNECTION_TYPE::USB;
  case C_CONNECTION_TYPE::ETHERNET:
    return CONNECTION_TYPE::ETHERNET;
  case C_CONNECTION_TYPE::FIREWIRE:
    return CONNECTION_TYPE::FIREWIRE;
  case C_CONNECTION_TYPE::BLUETOOTH:
    return CONNECTION_TYPE::BLUETOOTH;
  default:
    throw std::invalid_argument("Unknown C_CONNECTION_TYPE value");
  }
}

// converting JSRDotNETSDK::PROPERTY_UNITS <-> C_PROPERTY_UNITS
inline constexpr static C_PROPERTY_UNITS
propertyUnitsFromManaged(const PROPERTY_UNITS &units) {
  switch (units) {
  case PROPERTY_UNITS::UNIT_NONE:
    return C_PROPERTY_UNITS::UNIT_NONE;
  case PROPERTY_UNITS::UNIT_HERTZ:
    return C_PROPERTY_UNITS::UNIT_HERTZ;
  case PROPERTY_UNITS::UNIT_KILOHERTZ:
    return C_PROPERTY_UNITS::UNIT_KILOHERTZ;
  case PROPERTY_UNITS::UNIT_MEGAHERTZ:
    return C_PROPERTY_UNITS::UNIT_MEGAHERTZ;
  case PROPERTY_UNITS::UNIT_MICROJOULES:
    return C_PROPERTY_UNITS::UNIT_MICROJOULES;
  case PROPERTY_UNITS::UNIT_JOULES:
    return C_PROPERTY_UNITS::UNIT_JOULES;
  case PROPERTY_UNITS::UNIT_WATTS:
    return C_PROPERTY_UNITS::UNIT_WATTS;
  case PROPERTY_UNITS::UNIT_DB:
    return C_PROPERTY_UNITS::UNIT_DB;
  case PROPERTY_UNITS::UNIT_VOLTS:
    return C_PROPERTY_UNITS::UNIT_VOLTS;
  case PROPERTY_UNITS::UNIT_OHMS:
    return C_PROPERTY_UNITS::UNIT_OHMS;
  case PROPERTY_UNITS::UNIT_PICOFARADS:
    return C_PROPERTY_UNITS::UNIT_PICOFARADS;
  default:
    throw std::invalid_argument("Unknown PROPERTY_UNITS value");
  }
}
inline constexpr static PROPERTY_UNITS
propertyUnitsToManaged(const C_PROPERTY_UNITS &units) {
  switch (units) {
  case C_PROPERTY_UNITS::UNIT_NONE:
    return PROPERTY_UNITS::UNIT_NONE;
  case C_PROPERTY_UNITS::UNIT_HERTZ:
    return PROPERTY_UNITS::UNIT_HERTZ;
  case C_PROPERTY_UNITS::UNIT_KILOHERTZ:
    return PROPERTY_UNITS::UNIT_KILOHERTZ;
  case C_PROPERTY_UNITS::UNIT_MEGAHERTZ:
    return PROPERTY_UNITS::UNIT_MEGAHERTZ;
  case C_PROPERTY_UNITS::UNIT_MICROJOULES:
    return PROPERTY_UNITS::UNIT_MICROJOULES;
  case C_PROPERTY_UNITS::UNIT_JOULES:
    return PROPERTY_UNITS::UNIT_JOULES;
  case C_PROPERTY_UNITS::UNIT_WATTS:
    return PROPERTY_UNITS::UNIT_WATTS;
  case C_PROPERTY_UNITS::UNIT_DB:
    return PROPERTY_UNITS::UNIT_DB;
  case C_PROPERTY_UNITS::UNIT_VOLTS:
    return PROPERTY_UNITS::UNIT_VOLTS;
  case C_PROPERTY_UNITS::UNIT_OHMS:
    return PROPERTY_UNITS::UNIT_OHMS;
  case C_PROPERTY_UNITS::UNIT_PICOFARADS:
    return PROPERTY_UNITS::UNIT_PICOFARADS;
  default:
    throw std::invalid_argument("Unknown C_PROPERTY_UNITS value");
  }
}

// converting JSRDotNETSDK::PulserPropertyRole <-> C_PULSER_PROPERTY_ROLE
inline constexpr static C_PULSER_PROPERTY_ROLE
pulserPropertyRoleFromManaged(const PulserPropertyRole &role) {
  switch (role) {
  case PulserPropertyRole::DIRECT:
    return C_PULSER_PROPERTY_ROLE::DIRECT;
  case PulserPropertyRole::DIRECTSUPPORTED:
    return C_PULSER_PROPERTY_ROLE::DIRECTSUPPORTED;
  case PulserPropertyRole::SPECIFICVALUESUPPORTED:
    return C_PULSER_PROPERTY_ROLE::SPECIFICVALUESUPPORTED;
  case PulserPropertyRole::DIRECTVALUES:
    return C_PULSER_PROPERTY_ROLE::DIRECTVALUES;
  case PulserPropertyRole::STEPSIZE:
    return C_PULSER_PROPERTY_ROLE::STEPSIZE;
  case PulserPropertyRole::STEPSIZESUPPORTED:
    return C_PULSER_PROPERTY_ROLE::STEPSIZESUPPORTED;
  case PulserPropertyRole::INDEX:
    return C_PULSER_PROPERTY_ROLE::INDEX;
  case PulserPropertyRole::INDEXMAX:
    return C_PULSER_PROPERTY_ROLE::INDEXMAX;
  case PulserPropertyRole::INDEXSUPPORTED:
    return C_PULSER_PROPERTY_ROLE::INDEXSUPPORTED;
  case PulserPropertyRole::MAX:
    return C_PULSER_PROPERTY_ROLE::MAX;
  case PulserPropertyRole::MIN:
    return C_PULSER_PROPERTY_ROLE::MIN;
  case PulserPropertyRole::INDEXVALUES:
    return C_PULSER_PROPERTY_ROLE::INDEXVALUES;
  case PulserPropertyRole::NUMERATOR:
    return C_PULSER_PROPERTY_ROLE::NUMERATOR;
  case PulserPropertyRole::OTHER:
    return C_PULSER_PROPERTY_ROLE::OTHER;
  default:
    throw std::invalid_argument("Unknown PULSER_PROPERTY_ROLES value");
  }
}
inline constexpr static PulserPropertyRole
pulserPropertyRoleToManaged(const C_PULSER_PROPERTY_ROLE &role) {
  switch (role) {
  case C_PULSER_PROPERTY_ROLE::DIRECT:
    return PulserPropertyRole::DIRECT;
  case C_PULSER_PROPERTY_ROLE::DIRECTSUPPORTED:
    return PulserPropertyRole::DIRECTSUPPORTED;
  case C_PULSER_PROPERTY_ROLE::SPECIFICVALUESUPPORTED:
    return PulserPropertyRole::SPECIFICVALUESUPPORTED;
  case C_PULSER_PROPERTY_ROLE::DIRECTVALUES:
    return PulserPropertyRole::DIRECTVALUES;
  case C_PULSER_PROPERTY_ROLE::STEPSIZE:
    return PulserPropertyRole::STEPSIZE;
  case C_PULSER_PROPERTY_ROLE::STEPSIZESUPPORTED:
    return PulserPropertyRole::STEPSIZESUPPORTED;
  case C_PULSER_PROPERTY_ROLE::INDEX:
    return PulserPropertyRole::INDEX;
  case C_PULSER_PROPERTY_ROLE::INDEXMAX:
    return PulserPropertyRole::INDEXMAX;
  case C_PULSER_PROPERTY_ROLE::INDEXSUPPORTED:
    return PulserPropertyRole::INDEXSUPPORTED;
  case C_PULSER_PROPERTY_ROLE::MAX:
    return PulserPropertyRole::MAX;
  case C_PULSER_PROPERTY_ROLE::MIN:
    return PulserPropertyRole::MIN;
  case C_PULSER_PROPERTY_ROLE::INDEXVALUES:
    return PulserPropertyRole::INDEXVALUES;
  case C_PULSER_PROPERTY_ROLE::NUMERATOR:
    return PulserPropertyRole::NUMERATOR;
  case C_PULSER_PROPERTY_ROLE::OTHER:
    return PulserPropertyRole::OTHER;
  default:
    throw std::invalid_argument("Unknown C_PULSER_PROPERTY_ROLE value");
  }
}

// converting JSRDotNETSDK::STATUS_CHANGE <-> C_STATUS_CHANGE
inline constexpr static C_STATUS_CHANGE
statusChangeFromManaged(const STATUS_CHANGE &change) {
  switch (change) {
  case STATUS_CHANGE::INSTRUMENT_DISCONNECT:
    return C_STATUS_CHANGE::INSTRUMENT_DISCONNECT;
  case STATUS_CHANGE::PROPERTY_CHANGE:
    return C_STATUS_CHANGE::PROPERTY_CHANGE;
  case STATUS_CHANGE::OPERATIONAL_FAULT:
    return C_STATUS_CHANGE::OPERATIONAL_FAULT;
  case STATUS_CHANGE::STATE_CHANGE:
    return C_STATUS_CHANGE::STATE_CHANGE;
  default:
    throw std::invalid_argument("Unknown STATUS_CHANGE value");
  }
}
inline constexpr static STATUS_CHANGE
statusChangeToManaged(const C_STATUS_CHANGE &change) {
  switch (change) {
  case C_STATUS_CHANGE::INSTRUMENT_DISCONNECT:
    return STATUS_CHANGE::INSTRUMENT_DISCONNECT;
  case C_STATUS_CHANGE::PROPERTY_CHANGE:
    return STATUS_CHANGE::PROPERTY_CHANGE;
  case C_STATUS_CHANGE::OPERATIONAL_FAULT:
    return STATUS_CHANGE::OPERATIONAL_FAULT;
  case C_STATUS_CHANGE::STATE_CHANGE:
    return STATUS_CHANGE::STATE_CHANGE;
  default:
    throw std::invalid_argument("Unknown C_STATUS_CHANGE value");
  }
}

// converting JSRDotNETSDK::PulserReceiverState <-> C_PULSER_RECEIVER_STATE
inline constexpr static C_PULSER_RECEIVER_STATE
pulserReceiverStateFromManaged(const PulserReceiverState &state) {
  switch (state) {
  case PulserReceiverState::DETECTED:
    return C_PULSER_RECEIVER_STATE::DETECTED;
  case PulserReceiverState::INITIALIZING:
    return C_PULSER_RECEIVER_STATE::INITIALIZING;
  case PulserReceiverState::READY:
    return C_PULSER_RECEIVER_STATE::READY;
  case PulserReceiverState::ERROR:
    return C_PULSER_RECEIVER_STATE::ERRORC;
  case PulserReceiverState::NO_PULSER:
    return C_PULSER_RECEIVER_STATE::NO_PULSER;
  case PulserReceiverState::DETACHED:
    return C_PULSER_RECEIVER_STATE::DETACHED;
  default:
    throw std::invalid_argument("Unknown PulserReceiverState value");
  }
}
inline constexpr static PulserReceiverState
pulserReceiverStateToManaged(const C_PULSER_RECEIVER_STATE &state) {
  switch (state) {
  case C_PULSER_RECEIVER_STATE::DETECTED:
    return PulserReceiverState::DETECTED;
  case C_PULSER_RECEIVER_STATE::INITIALIZING:
    return PulserReceiverState::INITIALIZING;
  case C_PULSER_RECEIVER_STATE::READY:
    return PulserReceiverState::READY;
  case C_PULSER_RECEIVER_STATE::ERRORC:
    return PulserReceiverState::ERROR;
  case C_PULSER_RECEIVER_STATE::NO_PULSER:
    return PulserReceiverState::NO_PULSER;
  case C_PULSER_RECEIVER_STATE::DETACHED:
    return PulserReceiverState::DETACHED;
  default:
    throw std::invalid_argument("Unknown C_PULSER_RECEIVER_STATE value");
  }
}

// converting JSRDotNETSDK::PulserPropertyDataType <->
// C_PULSER_PROPERTY_DATA_TYPE
inline constexpr static C_PULSER_PROPERTY_DATA_TYPE
pulserPropertyDataTypeFromManaged(const PulserPropertyDataType &type) {
  switch (type) {
  case PulserPropertyDataType::INTEGER:
    return C_PULSER_PROPERTY_DATA_TYPE::INTEGER;
  case PulserPropertyDataType::BYTE:
    return C_PULSER_PROPERTY_DATA_TYPE::BYTE;
  case PulserPropertyDataType::DOUBLE:
    return C_PULSER_PROPERTY_DATA_TYPE::DOUBLE;
  case PulserPropertyDataType::BOOLEAN:
    return C_PULSER_PROPERTY_DATA_TYPE::BOOLEAN;
  case PulserPropertyDataType::STRING:
    return C_PULSER_PROPERTY_DATA_TYPE::STRING;
  case PulserPropertyDataType::BYTE_ARRAY:
    return C_PULSER_PROPERTY_DATA_TYPE::BYTE_ARRAY;
  case PulserPropertyDataType::INTEGER_ARRAY:
    return C_PULSER_PROPERTY_DATA_TYPE::INTEGER_ARRAY;
  case PulserPropertyDataType::DOUBLE_ARRAY:
    return C_PULSER_PROPERTY_DATA_TYPE::DOUBLE_ARRAY;
  case PulserPropertyDataType::BOOLEAN_ARRAY:
    return C_PULSER_PROPERTY_DATA_TYPE::BOOLEAN_ARRAY;
  case PulserPropertyDataType::STRING_ARRAY:
    return C_PULSER_PROPERTY_DATA_TYPE::STRING_ARRAY;
  case PulserPropertyDataType::ENUM:
    return C_PULSER_PROPERTY_DATA_TYPE::ENUM;
  case PulserPropertyDataType::ID:
    return C_PULSER_PROPERTY_DATA_TYPE::ID;
  case PulserPropertyDataType::OTHER:
    return C_PULSER_PROPERTY_DATA_TYPE::OTHER;
  default:
    throw std::invalid_argument("Unknown PulserPropertyDataType value");
  }
}
inline constexpr static PulserPropertyDataType
pulserPropertyDataTypeToManaged(const C_PULSER_PROPERTY_DATA_TYPE &type) {
  switch (type) {
  case C_PULSER_PROPERTY_DATA_TYPE::INTEGER:
    return PulserPropertyDataType::INTEGER;
  case C_PULSER_PROPERTY_DATA_TYPE::BYTE:
    return PulserPropertyDataType::BYTE;
  case C_PULSER_PROPERTY_DATA_TYPE::DOUBLE:
    return PulserPropertyDataType::DOUBLE;
  case C_PULSER_PROPERTY_DATA_TYPE::BOOLEAN:
    return PulserPropertyDataType::BOOLEAN;
  case C_PULSER_PROPERTY_DATA_TYPE::STRING:
    return PulserPropertyDataType::STRING;
  case C_PULSER_PROPERTY_DATA_TYPE::BYTE_ARRAY:
    return PulserPropertyDataType::BYTE_ARRAY;
  case C_PULSER_PROPERTY_DATA_TYPE::INTEGER_ARRAY:
    return PulserPropertyDataType::INTEGER_ARRAY;
  case C_PULSER_PROPERTY_DATA_TYPE::DOUBLE_ARRAY:
    return PulserPropertyDataType::DOUBLE_ARRAY;
  case C_PULSER_PROPERTY_DATA_TYPE::BOOLEAN_ARRAY:
    return PulserPropertyDataType::BOOLEAN_ARRAY;
  case C_PULSER_PROPERTY_DATA_TYPE::STRING_ARRAY:
    return PulserPropertyDataType::STRING_ARRAY;
  case C_PULSER_PROPERTY_DATA_TYPE::ENUM:
    return PulserPropertyDataType::ENUM;
  case C_PULSER_PROPERTY_DATA_TYPE::ID:
    return PulserPropertyDataType::ID;
  case C_PULSER_PROPERTY_DATA_TYPE::OTHER:
    return PulserPropertyDataType::OTHER;
  default:
    throw std::invalid_argument("Unknown C_PULSER_PROPERTY_DATA_TYPE value");
  }
}

// converting JSRDotNETSDK::ErrorCode <-> C_ERROR_CODE
inline constexpr static C_ERROR_CODE
errorCodeFromManaged(const ERROR_CODE &code) {
  switch (code) {
  case ERROR_CODE::UNKNOWN:
    return C_ERROR_CODE::UNKNOWN;
  case ERROR_CODE::DEVICE_NOT_FOUND:
    return C_ERROR_CODE::DEVICE_NOT_FOUND;
  case ERROR_CODE::PLUGINS_NOT_LOADED:
    return C_ERROR_CODE::PLUGINS_NOT_LOADED;
  case ERROR_CODE::NOT_FOUND:
    return C_ERROR_CODE::NOT_FOUND;
  case ERROR_CODE::COMMUNICATION_FAILURE:
    return C_ERROR_CODE::COMMUNICATION_FAILURE;
  case ERROR_CODE::RESP_ADDRESS_MISMATCH:
    return C_ERROR_CODE::RESP_ADDRESS_MISMATCH;
  case ERROR_CODE::RESP_INVALID_LENGTH:
    return C_ERROR_CODE::RESP_INVALID_LENGTH;
  case ERROR_CODE::PORT_OPEN_FAILED:
    return C_ERROR_CODE::PORT_OPEN_FAILED;
  case ERROR_CODE::TIMEOUT:
    return C_ERROR_CODE::TIMEOUT;
  case ERROR_CODE::VALUE_OUT_RANGE:
    return C_ERROR_CODE::VALUE_OUT_RANGE;
  case ERROR_CODE::OPERATION_NOT_SUPPORTED:
    return C_ERROR_CODE::OPERATION_NOT_SUPPORTED;
  case ERROR_CODE::HARDWARE_FAILURE:
    return C_ERROR_CODE::HARDWARE_FAILURE;
  case ERROR_CODE::INTERNAL_SOFTWARE_FAULT:
    return C_ERROR_CODE::INTERNAL_SOFTWARE_FAULT;
  case ERROR_CODE::PROPERTY_WRITE_NOT_FINISHED_YET:
    return C_ERROR_CODE::PROPERTY_WRITE_NOT_FINISHED_YET;
  case ERROR_CODE::NO_PULSER_RCVR_SELECTED:
    return C_ERROR_CODE::NO_PULSER_RCVR_SELECTED;
  case ERROR_CODE::INVALID_ARGUMENT:
    return C_ERROR_CODE::INVALID_ARGUMENT;
  case ERROR_CODE::NULL_POINTER:
    return C_ERROR_CODE::NULL_POINTER;
  case ERROR_CODE::OPERATION_FAILED:
    return C_ERROR_CODE::OPERATION_FAILED;
  case ERROR_CODE::VERSION_MISMATCH:
    return C_ERROR_CODE::VERSION_MISMATCH;
  case ERROR_CODE::PLUGIN_LOAD_ERROR:
    return C_ERROR_CODE::PLUGIN_LOAD_ERROR;
  case ERROR_CODE::DEVICE_DISCOVERY_ERROR:
    return C_ERROR_CODE::DEVICE_DISCOVERY_ERROR;
  case ERROR_CODE::DEVICE_NOT_READY:
    return C_ERROR_CODE::DEVICE_NOT_READY;
  case ERROR_CODE::SUCCESS:
    return C_ERROR_CODE::SUCCESS;
  default:
    throw std::invalid_argument("Unknown ERROR_CODE value");
  }
}
inline constexpr static ERROR_CODE
errorCodeToManaged(const C_ERROR_CODE &code) {
  switch (code) {
  case C_ERROR_CODE::UNKNOWN:
    return ERROR_CODE::UNKNOWN;
  case C_ERROR_CODE::DEVICE_NOT_FOUND:
    return ERROR_CODE::DEVICE_NOT_FOUND;
  case C_ERROR_CODE::PLUGINS_NOT_LOADED:
    return ERROR_CODE::PLUGINS_NOT_LOADED;
  case C_ERROR_CODE::NOT_FOUND:
    return ERROR_CODE::NOT_FOUND;
  case C_ERROR_CODE::COMMUNICATION_FAILURE:
    return ERROR_CODE::COMMUNICATION_FAILURE;
  case C_ERROR_CODE::RESP_ADDRESS_MISMATCH:
    return ERROR_CODE::RESP_ADDRESS_MISMATCH;
  case C_ERROR_CODE::RESP_INVALID_LENGTH:
    return ERROR_CODE::RESP_INVALID_LENGTH;
  case C_ERROR_CODE::PORT_OPEN_FAILED:
    return ERROR_CODE::PORT_OPEN_FAILED;
  case C_ERROR_CODE::TIMEOUT:
    return ERROR_CODE::TIMEOUT;
  case C_ERROR_CODE::VALUE_OUT_RANGE:
    return ERROR_CODE::VALUE_OUT_RANGE;
  case C_ERROR_CODE::OPERATION_NOT_SUPPORTED:
    return ERROR_CODE::OPERATION_NOT_SUPPORTED;
  case C_ERROR_CODE::HARDWARE_FAILURE:
    return ERROR_CODE::HARDWARE_FAILURE;
  case C_ERROR_CODE::INTERNAL_SOFTWARE_FAULT:
    return ERROR_CODE::INTERNAL_SOFTWARE_FAULT;
  case C_ERROR_CODE::PROPERTY_WRITE_NOT_FINISHED_YET:
    return ERROR_CODE::PROPERTY_WRITE_NOT_FINISHED_YET;
  case C_ERROR_CODE::NO_PULSER_RCVR_SELECTED:
    return ERROR_CODE::NO_PULSER_RCVR_SELECTED;
  case C_ERROR_CODE::INVALID_ARGUMENT:
    return ERROR_CODE::INVALID_ARGUMENT;
  case C_ERROR_CODE::NULL_POINTER:
    return ERROR_CODE::NULL_POINTER;
  case C_ERROR_CODE::OPERATION_FAILED:
    return ERROR_CODE::OPERATION_FAILED;
  case C_ERROR_CODE::VERSION_MISMATCH:
    return ERROR_CODE::VERSION_MISMATCH;
  case C_ERROR_CODE::PLUGIN_LOAD_ERROR:
    return ERROR_CODE::PLUGIN_LOAD_ERROR;
  case C_ERROR_CODE::DEVICE_DISCOVERY_ERROR:
    return ERROR_CODE::DEVICE_DISCOVERY_ERROR;
  case C_ERROR_CODE::DEVICE_NOT_READY:
    return ERROR_CODE::DEVICE_NOT_READY;
  case C_ERROR_CODE::SUCCESS:
    return ERROR_CODE::SUCCESS;
  default:
    throw std::invalid_argument("Unknown C_ERROR_CODE value");
  }
}

// converting JSRDotNETSDK::DiscoveryStateFlags <-> C_DISCOVERY_STATE_FLAGS
inline constexpr static C_DISCOVERY_STATE_FLAGS
discoveryStateFlagsFromManaged(const DiscoveryStateFlags &flags) {
  switch (flags) {
  case DiscoveryStateFlags::NONE:
    return C_DISCOVERY_STATE_FLAGS::NONE;
  case DiscoveryStateFlags::ACTIVE:
    return C_DISCOVERY_STATE_FLAGS::ACTIVE;
  case DiscoveryStateFlags::PENDING:
    return C_DISCOVERY_STATE_FLAGS::PENDING;
  case DiscoveryStateFlags::STOPPED_ON_ERR:
    return C_DISCOVERY_STATE_FLAGS::STOPPED_ON_ERR;
  default:
    throw std::invalid_argument("Unknown DiscoveryStateFlags value");
  }
}
inline constexpr static DiscoveryStateFlags
discoveryStateFlagsToManaged(const C_DISCOVERY_STATE_FLAGS &flags) {
  switch (flags) {
  case C_DISCOVERY_STATE_FLAGS::NONE:
    return DiscoveryStateFlags::NONE;
  case C_DISCOVERY_STATE_FLAGS::ACTIVE:
    return DiscoveryStateFlags::ACTIVE;
  case C_DISCOVERY_STATE_FLAGS::PENDING:
    return DiscoveryStateFlags::PENDING;
  case C_DISCOVERY_STATE_FLAGS::STOPPED_ON_ERR:
    return DiscoveryStateFlags::STOPPED_ON_ERR;
  default:
    throw std::invalid_argument("Unknown C_DISCOVERY_STATE_FLAGS value");
  }
}

// converting JSRDotNETSDK::NotifyType <-> C_NOTIFY_TYPE
inline constexpr static C_NOTIFY_TYPE
notifyTypeFromManaged(const JSRDotNETSDK::NOTIFY_TYPE &type) {
  switch (type) {
  case JSRDotNETSDK::NOTIFY_TYPE::PULSER_RCVR_DISCOVERED:
    return C_NOTIFY_TYPE::PULSER_RCVR_DISCOVERED;
  case JSRDotNETSDK::NOTIFY_TYPE::CURRENT_PULSER_RCVR_CHANGED:
    return C_NOTIFY_TYPE::CURRENT_PULSER_RCVR_CHANGED;
  case JSRDotNETSDK::NOTIFY_TYPE::PULSER_RCVR_DETACH:
    return C_NOTIFY_TYPE::PULSER_RCVR_DETACH;
  case JSRDotNETSDK::NOTIFY_TYPE::DISCOVERY_STARTED:
    return C_NOTIFY_TYPE::DISCOVERY_STARTED;
  case JSRDotNETSDK::NOTIFY_TYPE::DISCOVERY_COMPLETE:
    return C_NOTIFY_TYPE::DISCOVERY_COMPLETE;
  case JSRDotNETSDK::NOTIFY_TYPE::PROPERTY_CHANGE:
    return C_NOTIFY_TYPE::PROPERTY_CHANGE;
  case JSRDotNETSDK::NOTIFY_TYPE::STATE_CHANGE:
    return C_NOTIFY_TYPE::STATE_CHANGE;
  case JSRDotNETSDK::NOTIFY_TYPE::ERROR:
    return C_NOTIFY_TYPE::ERROR;
  default:
    throw std::invalid_argument("Unknown NOTIFY_TYPE value");
  }
}
inline constexpr static JSRDotNETSDK::NOTIFY_TYPE
notifyTypeToManaged(const C_NOTIFY_TYPE &type) {
  switch (type) {
  case C_NOTIFY_TYPE::PULSER_RCVR_DISCOVERED:
    return JSRDotNETSDK::NOTIFY_TYPE::PULSER_RCVR_DISCOVERED;
  case C_NOTIFY_TYPE::CURRENT_PULSER_RCVR_CHANGED:
    return JSRDotNETSDK::NOTIFY_TYPE::CURRENT_PULSER_RCVR_CHANGED;
  case C_NOTIFY_TYPE::PULSER_RCVR_DETACH:
    return JSRDotNETSDK::NOTIFY_TYPE::PULSER_RCVR_DETACH;
  case C_NOTIFY_TYPE::DISCOVERY_STARTED:
    return JSRDotNETSDK::NOTIFY_TYPE::DISCOVERY_STARTED;
  case C_NOTIFY_TYPE::DISCOVERY_COMPLETE:
    return JSRDotNETSDK::NOTIFY_TYPE::DISCOVERY_COMPLETE;
  case C_NOTIFY_TYPE::PROPERTY_CHANGE:
    return JSRDotNETSDK::NOTIFY_TYPE::PROPERTY_CHANGE;
  case C_NOTIFY_TYPE::STATE_CHANGE:
    return JSRDotNETSDK::NOTIFY_TYPE::STATE_CHANGE;
  case C_NOTIFY_TYPE::ERROR:
    return JSRDotNETSDK::NOTIFY_TYPE::ERROR;
  default:
    throw std::invalid_argument("Unknown C_NOTIFY_TYPE value");
  }
}

namespace msclr::interop {

// RecieverMode marshals
template <>
inline C_RECEIVER_MODE
marshal_as<C_RECEIVER_MODE, RECEIVER_MODE>(const RECEIVER_MODE &from) {
  return receiverModeFromManaged(from);
}
template <>
inline RECEIVER_MODE
marshal_as<RECEIVER_MODE, C_RECEIVER_MODE>(const C_RECEIVER_MODE &from) {
  return receiverModeToManaged(from);
}

// ManagerState marshals
template <>
inline C_MANAGER_STATE
marshal_as<C_MANAGER_STATE, JSRDotNETManager::MANAGER_STATE>(
    const JSRDotNETManager::MANAGER_STATE &from) {
  return managerStateFromManaged(from);
}
template <>
inline JSRDotNETManager::MANAGER_STATE
marshal_as<JSRDotNETManager::MANAGER_STATE, C_MANAGER_STATE>(
    const C_MANAGER_STATE &from) {
  return managerStateToManaged(from);
}

// TriggerSource marshals
template <>
inline C_TRIGGER_SOURCE
marshal_as<C_TRIGGER_SOURCE, TRIGGER_SOURCE>(const TRIGGER_SOURCE &from) {
  return triggerSourceFromManaged(from);
}
template <>
inline TRIGGER_SOURCE
marshal_as<TRIGGER_SOURCE, C_TRIGGER_SOURCE>(const C_TRIGGER_SOURCE &from) {
  return triggerSourceToManaged(from);
}

// TriggerPolarity marshals
template <>
inline C_TRIGGER_POLARITY
marshal_as<C_TRIGGER_POLARITY, TRIGGER_POLARITY>(const TRIGGER_POLARITY &from) {
  return triggerPolarityFromManaged(from);
}
template <>
inline TRIGGER_POLARITY marshal_as<TRIGGER_POLARITY, C_TRIGGER_POLARITY>(
    const C_TRIGGER_POLARITY &from) {
  return triggerPolarityToManaged(from);
}

// TriggerImpedance marshals
template <>
inline C_TRIGGER_IMPEDANCE marshal_as<C_TRIGGER_IMPEDANCE, TRIGGER_IMPEDANCE>(
    const TRIGGER_IMPEDANCE &from) {
  return triggerImpedanceFromManaged(from);
}
template <>
inline TRIGGER_IMPEDANCE marshal_as<TRIGGER_IMPEDANCE, C_TRIGGER_IMPEDANCE>(
    const C_TRIGGER_IMPEDANCE &from) {
  return triggerImpedanceToManaged(from);
}

// PulserImpedance marshals
template <>
inline C_PULSER_IMPEDANCE
marshal_as<C_PULSER_IMPEDANCE, PULSER_IMPEDANCE>(const PULSER_IMPEDANCE &from) {
  return pulserImpedanceFromManaged(from);
}
template <>
inline PULSER_IMPEDANCE marshal_as<PULSER_IMPEDANCE, C_PULSER_IMPEDANCE>(
    const C_PULSER_IMPEDANCE &from) {
  return pulserImpedanceToManaged(from);
}

// IsPulsing marshals
template <>
inline C_IS_PULSING
marshal_as<C_IS_PULSING, IS_PULSING>(const IS_PULSING &from) {
  return isPulsingFromManaged(from);
}
template <>
inline IS_PULSING
marshal_as<IS_PULSING, C_IS_PULSING>(const C_IS_PULSING &from) {
  return isPulsingToManaged(from);
}

// PowerLimit marshals
template <>
inline C_POWER_LIMIT
marshal_as<C_POWER_LIMIT, POWER_LIMIT>(const POWER_LIMIT &from) {
  return powerLimitFromManaged(from);
}
template <>
inline POWER_LIMIT
marshal_as<POWER_LIMIT, C_POWER_LIMIT>(const C_POWER_LIMIT &from) {
  return powerLimitToManaged(from);
}

// ConnectionType marshals
template <>
inline C_CONNECTION_TYPE
marshal_as<C_CONNECTION_TYPE, CONNECTION_TYPE>(const CONNECTION_TYPE &from) {
  return connectionTypeFromManaged(from);
}
template <>
inline CONNECTION_TYPE
marshal_as<CONNECTION_TYPE, C_CONNECTION_TYPE>(const C_CONNECTION_TYPE &from) {
  return connectionTypeToManaged(from);
}

// PropertyUnits marshals
template <>
inline C_PROPERTY_UNITS
marshal_as<C_PROPERTY_UNITS, PROPERTY_UNITS>(const PROPERTY_UNITS &from) {
  return propertyUnitsFromManaged(from);
}
template <>
inline PROPERTY_UNITS
marshal_as<PROPERTY_UNITS, C_PROPERTY_UNITS>(const C_PROPERTY_UNITS &from) {
  return propertyUnitsToManaged(from);
}

// PulserPropertyRoles marshals
template <>
inline C_PULSER_PROPERTY_ROLE
marshal_as<C_PULSER_PROPERTY_ROLE, PulserPropertyRole>(
    const PulserPropertyRole &from) {
  return pulserPropertyRoleFromManaged(from);
}
template <>
inline PulserPropertyRole
marshal_as<PulserPropertyRole, C_PULSER_PROPERTY_ROLE>(
    const C_PULSER_PROPERTY_ROLE &from) {
  return pulserPropertyRoleToManaged(from);
}

// StatusChange marshals
template <>
inline C_STATUS_CHANGE
marshal_as<C_STATUS_CHANGE, STATUS_CHANGE>(const STATUS_CHANGE &from) {
  return statusChangeFromManaged(from);
}
template <>
inline STATUS_CHANGE
marshal_as<STATUS_CHANGE, C_STATUS_CHANGE>(const C_STATUS_CHANGE &from) {
  return statusChangeToManaged(from);
}

// PulserReceiverState marshals
template <>
inline C_PULSER_RECEIVER_STATE
marshal_as<C_PULSER_RECEIVER_STATE, PulserReceiverState>(
    const PulserReceiverState &from) {
  return pulserReceiverStateFromManaged(from);
}
template <>
inline PulserReceiverState
marshal_as<PulserReceiverState, C_PULSER_RECEIVER_STATE>(
    const C_PULSER_RECEIVER_STATE &from) {
  return pulserReceiverStateToManaged(from);
}

// PulserPropertyDataType marshals
template <>
inline C_PULSER_PROPERTY_DATA_TYPE
marshal_as<C_PULSER_PROPERTY_DATA_TYPE, PulserPropertyDataType>(
    const PulserPropertyDataType &from) {
  return pulserPropertyDataTypeFromManaged(from);
}
template <>
inline PulserPropertyDataType
marshal_as<PulserPropertyDataType, C_PULSER_PROPERTY_DATA_TYPE>(
    const C_PULSER_PROPERTY_DATA_TYPE &from) {
  return pulserPropertyDataTypeToManaged(from);
}

// ErrorCode marshals
template <>
inline C_ERROR_CODE
marshal_as<C_ERROR_CODE, ERROR_CODE>(const ERROR_CODE &from) {
  return errorCodeFromManaged(from);
}
template <>
inline ERROR_CODE
marshal_as<ERROR_CODE, C_ERROR_CODE>(const C_ERROR_CODE &from) {
  return errorCodeToManaged(from);
}

// DiscoveryStateFlags marshals
template <>
inline C_DISCOVERY_STATE_FLAGS
marshal_as<C_DISCOVERY_STATE_FLAGS, DiscoveryStateFlags>(
    const DiscoveryStateFlags &from) {
  return discoveryStateFlagsFromManaged(from);
}
template <>
inline DiscoveryStateFlags
marshal_as<DiscoveryStateFlags, C_DISCOVERY_STATE_FLAGS>(
    const C_DISCOVERY_STATE_FLAGS &from) {
  return discoveryStateFlagsToManaged(from);
}

// NotifyType marshals
template <>
inline C_NOTIFY_TYPE
marshal_as<C_NOTIFY_TYPE, NOTIFY_TYPE>(const NOTIFY_TYPE &from) {
  return notifyTypeFromManaged(from);
}
template <>
inline NOTIFY_TYPE
marshal_as<NOTIFY_TYPE, C_NOTIFY_TYPE>(const C_NOTIFY_TYPE &from) {
  return notifyTypeToManaged(from);
}

} // namespace msclr::interop
