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
#include <iostream> // Include iostream for std::cerr

#undef ERROR

// Conversions between the C++ enums and the .NET SDK enums. Private to the
// DLL (C++/CLI only); tests reach them through src/ on the include path.
namespace jsr::detail {

using namespace JSRDotNETSDK;

// RECEIVER_MODE <-> C_RECEIVER_MODE
inline constexpr C_RECEIVER_MODE
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
    std::cerr << "[MarshalEnums] Unknown RECEIVER_MODE value in receiverModeFromManaged, returning BOTH" << std::endl;
    return C_RECEIVER_MODE::BOTH;
  }
}
inline constexpr RECEIVER_MODE
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
    std::cerr << "[MarshalEnums] Unknown C_RECEIVER_MODE value in receiverModeToManaged, returning BOTH" << std::endl;
    return RECEIVER_MODE::BOTH;
  }
}

// converting JSRDotNETSDK::TRIGGER_SOURCE <-> C_TRIGGER_SOURCE
inline constexpr C_TRIGGER_SOURCE
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
    std::cerr << "[MarshalEnums] Unknown TRIGGER_SOURCE value in triggerSourceFromManaged, returning INTERNAL" << std::endl;
    return C_TRIGGER_SOURCE::INTERNAL;
  }
}
inline constexpr TRIGGER_SOURCE
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
    std::cerr << "[MarshalEnums] Unknown C_TRIGGER_SOURCE value in triggerSourceToManaged, returning INTERNAL" << std::endl;
    return TRIGGER_SOURCE::INTERNAL;
  }
}

// converting JSRDotNETSDK::TRIGGER_POLARITY <-> C_TRIGGER_POLARITY
inline constexpr C_TRIGGER_POLARITY
triggerPolarityFromManaged(const TRIGGER_POLARITY &source) {
  switch (source) {
  case TRIGGER_POLARITY::FALLING:
    return C_TRIGGER_POLARITY::FALLING;
  case TRIGGER_POLARITY::RISING:
    return C_TRIGGER_POLARITY::RISING;
  case TRIGGER_POLARITY::LAST_VAL:
    return C_TRIGGER_POLARITY::LAST_VAL;
  default:
    std::cerr << "[MarshalEnums] Unknown TRIGGER_POLARITY value in triggerPolarityFromManaged, returning FALLING" << std::endl;
    return C_TRIGGER_POLARITY::FALLING;
  }
}
inline constexpr TRIGGER_POLARITY
triggerPolarityToManaged(const C_TRIGGER_POLARITY &source) {
  switch (source) {
  case C_TRIGGER_POLARITY::FALLING:
    return TRIGGER_POLARITY::FALLING;
  case C_TRIGGER_POLARITY::RISING:
    return TRIGGER_POLARITY::RISING;
  case C_TRIGGER_POLARITY::LAST_VAL:
    return TRIGGER_POLARITY::LAST_VAL;
  default:
    std::cerr << "[MarshalEnums] Unknown C_TRIGGER_POLARITY value in triggerPolarityToManaged, returning FALLING" << std::endl;
    return TRIGGER_POLARITY::FALLING;
  }
}

// converting JSRDotNETSDK::TRIGGER_IMPEDANCE <-> C_TRIGGER_IMPEDANCE
inline constexpr C_TRIGGER_IMPEDANCE
triggerImpedanceFromManaged(const TRIGGER_IMPEDANCE &source) {
  switch (source) {
  case TRIGGER_IMPEDANCE::HIGH_Z:
    return C_TRIGGER_IMPEDANCE::HIGH_Z;
  case TRIGGER_IMPEDANCE::LOW_Z:
    return C_TRIGGER_IMPEDANCE::LOW_Z;
  case TRIGGER_IMPEDANCE::LAST_VAL:
    return C_TRIGGER_IMPEDANCE::LAST_VAL;
  default:
    std::cerr << "[MarshalEnums] Unknown TRIGGER_IMPEDANCE value in triggerImpedanceFromManaged, returning HIGH_Z" << std::endl;
    return C_TRIGGER_IMPEDANCE::HIGH_Z;
  }
}
inline constexpr TRIGGER_IMPEDANCE
triggerImpedanceToManaged(const C_TRIGGER_IMPEDANCE &source) {
  switch (source) {
  case C_TRIGGER_IMPEDANCE::HIGH_Z:
    return TRIGGER_IMPEDANCE::HIGH_Z;
  case C_TRIGGER_IMPEDANCE::LOW_Z:
    return TRIGGER_IMPEDANCE::LOW_Z;
  case C_TRIGGER_IMPEDANCE::LAST_VAL:
    return TRIGGER_IMPEDANCE::LAST_VAL;
  default:
    std::cerr << "[MarshalEnums] Unknown C_TRIGGER_IMPEDANCE value in triggerImpedanceToManaged, returning HIGH_Z" << std::endl;
    return TRIGGER_IMPEDANCE::HIGH_Z;
  }
}

// converting JSRDotNETSDK::PULSER_IMPEDANCE <-> C_PULSER_IMPEDANCE
inline constexpr C_PULSER_IMPEDANCE
pulserImpedanceFromManaged(const PULSER_IMPEDANCE &source) {
  switch (source) {
  case PULSER_IMPEDANCE::HIGH_Z:
    return C_PULSER_IMPEDANCE::HIGH_Z;
  case PULSER_IMPEDANCE::LOW_Z:
    return C_PULSER_IMPEDANCE::LOW_Z;
  case PULSER_IMPEDANCE::LAST_VAL:
    return C_PULSER_IMPEDANCE::LAST_VAL;
  default:
    std::cerr << "[MarshalEnums] Unknown PULSER_IMPEDANCE value in pulserImpedanceFromManaged, returning HIGH_Z" << std::endl;
    return C_PULSER_IMPEDANCE::HIGH_Z;
  }
}
inline constexpr PULSER_IMPEDANCE
pulserImpedanceToManaged(const C_PULSER_IMPEDANCE &source) {
  switch (source) {
  case C_PULSER_IMPEDANCE::HIGH_Z:
    return PULSER_IMPEDANCE::HIGH_Z;
  case C_PULSER_IMPEDANCE::LOW_Z:
    return PULSER_IMPEDANCE::LOW_Z;
  case C_PULSER_IMPEDANCE::LAST_VAL:
    return PULSER_IMPEDANCE::LAST_VAL;
  default:
    std::cerr << "[MarshalEnums] Unknown C_PULSER_IMPEDANCE value in pulserImpedanceToManaged, returning HIGH_Z" << std::endl;
    return PULSER_IMPEDANCE::HIGH_Z;
  }
}

// converting JSRDotNETSDK::IS_PULSING <-> C_IS_PULSING
inline constexpr C_IS_PULSING
isPulsingFromManaged(const IS_PULSING &source) {
  switch (source) {
  case IS_PULSING::ACTIVE:
    return C_IS_PULSING::ACTIVE;
  case IS_PULSING::INACTIVE:
    return C_IS_PULSING::INACTIVE;
  case IS_PULSING::UNKNOWN:
    return C_IS_PULSING::UNKNOWN;
  default:
    std::cerr << "[MarshalEnums] Unknown IS_PULSING value in isPulsingFromManaged, returning ACTIVE" << std::endl;
    return C_IS_PULSING::ACTIVE;
  }
}
inline constexpr IS_PULSING
isPulsingToManaged(const C_IS_PULSING &source) {
  switch (source) {
  case C_IS_PULSING::ACTIVE:
    return IS_PULSING::ACTIVE;
  case C_IS_PULSING::INACTIVE:
    return IS_PULSING::INACTIVE;
  case C_IS_PULSING::UNKNOWN:
    return IS_PULSING::UNKNOWN;
  default:
    std::cerr << "[MarshalEnums] Unknown C_IS_PULSING value in isPulsingToManaged, returning ACTIVE" << std::endl;
    return IS_PULSING::ACTIVE;
  }
}

// converting JSRDotNETSDK::POWER_LIMIT <-> C_POWER_LIMIT
inline constexpr C_POWER_LIMIT
powerLimitFromManaged(const POWER_LIMIT &source) {
  switch (source) {
  case POWER_LIMIT::OVER_LIMIT:
    return C_POWER_LIMIT::OVER_LIMIT;
  case POWER_LIMIT::WITHIN_LIMIT:
    return C_POWER_LIMIT::WITHIN_LIMIT;
  default:
    std::cerr << "[MarshalEnums] Unknown POWER_LIMIT value in powerLimitFromManaged, returning WITHIN_LIMIT" << std::endl;
    return C_POWER_LIMIT::WITHIN_LIMIT;
  }
}
inline constexpr POWER_LIMIT
powerLimitToManaged(const C_POWER_LIMIT &source) {
  switch (source) {
  case C_POWER_LIMIT::OVER_LIMIT:
    return POWER_LIMIT::OVER_LIMIT;
  case C_POWER_LIMIT::WITHIN_LIMIT:
    return POWER_LIMIT::WITHIN_LIMIT;
  default:
    std::cerr << "[MarshalEnums] Unknown C_POWER_LIMIT value in powerLimitToManaged, returning WITHIN_LIMIT" << std::endl;
    return POWER_LIMIT::WITHIN_LIMIT;
  }
}

// converting JSRDotNETSDK::MANAGER_STATE <-> C_MANAGER_STATE
inline constexpr C_MANAGER_STATE
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
    std::cerr << "[MarshalEnums] Unknown MANAGER_STATE value in managerStateFromManaged, returning NOT_STARTED" << std::endl;
    return C_MANAGER_STATE::NOT_STARTED;
  }
}
inline constexpr JSRDotNETManager::MANAGER_STATE
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
    std::cerr << "[MarshalEnums] Unknown C_MANAGER_STATE value in managerStateToManaged, returning NOT_STARTED" << std::endl;
    return JSRDotNETManager::MANAGER_STATE::NOT_STARTED;
  }
}

// converting JSRDotNETSDK::CONNECTION_TYPE <-> C_CONNECTION_TYPE
inline constexpr C_CONNECTION_TYPE
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
    std::cerr << "[MarshalEnums] Unknown CONNECTION_TYPE value in connectionTypeFromManaged, returning SOFTWARE" << std::endl;
    return C_CONNECTION_TYPE::SOFTWARE;
  }
}
inline constexpr CONNECTION_TYPE
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
    std::cerr << "[MarshalEnums] Unknown C_CONNECTION_TYPE value in connectionTypeToManaged, returning SOFTWARE" << std::endl;
    return CONNECTION_TYPE::SOFTWARE;
  }
}

// converting JSRDotNETSDK::PROPERTY_UNITS <-> C_PROPERTY_UNITS
inline constexpr C_PROPERTY_UNITS
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
    std::cerr << "[MarshalEnums] Unknown PROPERTY_UNITS value in propertyUnitsFromManaged, returning UNIT_NONE" << std::endl;
    return C_PROPERTY_UNITS::UNIT_NONE;
  }
}
inline constexpr PROPERTY_UNITS
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
    std::cerr << "[MarshalEnums] Unknown C_PROPERTY_UNITS value in propertyUnitsToManaged, returning UNIT_NONE" << std::endl;
    return PROPERTY_UNITS::UNIT_NONE;
  }
}

// PulserPropertyRole <-> C_PULSER_PROPERTY_ROLE
inline constexpr C_PULSER_PROPERTY_ROLE
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
    std::cerr << "[MarshalEnums] Unknown PulserPropertyRole value in pulserPropertyRoleFromManaged, returning DIRECT" << std::endl;
    return C_PULSER_PROPERTY_ROLE::DIRECT;
  }
}
inline constexpr PulserPropertyRole
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
    std::cerr << "[MarshalEnums] Unknown C_PULSER_PROPERTY_ROLE value in pulserPropertyRoleToManaged, returning DIRECT" << std::endl;
    return PulserPropertyRole::DIRECT;
  }
}

// STATUS_CHANGE <-> C_STATUS_CHANGE
inline constexpr C_STATUS_CHANGE
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
    std::cerr << "[MarshalEnums] Unknown STATUS_CHANGE value in statusChangeFromManaged, returning INSTRUMENT_DISCONNECT" << std::endl;
    return C_STATUS_CHANGE::INSTRUMENT_DISCONNECT;
  }
}
inline constexpr STATUS_CHANGE
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
    std::cerr << "[MarshalEnums] Unknown C_STATUS_CHANGE value in statusChangeToManaged, returning INSTRUMENT_DISCONNECT" << std::endl;
    return STATUS_CHANGE::INSTRUMENT_DISCONNECT;
  }
}

// PulserReceiverState <-> C_PULSER_RECEIVER_STATE
inline constexpr C_PULSER_RECEIVER_STATE
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
    std::cerr << "[MarshalEnums] Unknown PulserReceiverState value in pulserReceiverStateFromManaged, returning DETECTED" << std::endl;
    return C_PULSER_RECEIVER_STATE::DETECTED;
  }
}
inline constexpr PulserReceiverState
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
    std::cerr << "[MarshalEnums] Unknown C_PULSER_RECEIVER_STATE value in pulserReceiverStateToManaged, returning DETECTED" << std::endl;
    return PulserReceiverState::DETECTED;
  }
}

// PulserPropertyDataType <-> C_PULSER_PROPERTY_DATA_TYPE
inline constexpr C_PULSER_PROPERTY_DATA_TYPE
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
    std::cerr << "[MarshalEnums] Unknown PulserPropertyDataType value in pulserPropertyDataTypeFromManaged, returning INTEGER" << std::endl;
    return C_PULSER_PROPERTY_DATA_TYPE::INTEGER;
  }
}
inline constexpr PulserPropertyDataType
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
    std::cerr << "[MarshalEnums] Unknown C_PULSER_PROPERTY_DATA_TYPE value in pulserPropertyDataTypeToManaged, returning INTEGER" << std::endl;
    return PulserPropertyDataType::INTEGER;
  }
}

// ErrorCode <-> C_ERROR_CODE
inline constexpr C_ERROR_CODE
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
    std::cerr << "[MarshalEnums] Unknown ERROR_CODE value in errorCodeFromManaged, returning UNKNOWN" << std::endl;
    return C_ERROR_CODE::UNKNOWN;
  }
}
inline constexpr ERROR_CODE
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
    std::cerr << "[MarshalEnums] Unknown C_ERROR_CODE value in errorCodeToManaged, returning UNKNOWN" << std::endl;
    return ERROR_CODE::UNKNOWN;
  }
}

// DiscoveryStateFlags <-> C_DISCOVERY_STATE_FLAGS
inline constexpr C_DISCOVERY_STATE_FLAGS
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
    std::cerr << "[MarshalEnums] Unknown DiscoveryStateFlags value in discoveryStateFlagsFromManaged, returning NONE" << std::endl;
    return C_DISCOVERY_STATE_FLAGS::NONE;
  }
}
inline constexpr DiscoveryStateFlags
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
    std::cerr << "[MarshalEnums] Unknown C_DISCOVERY_STATE_FLAGS value in discoveryStateFlagsToManaged, returning NONE" << std::endl;
    return DiscoveryStateFlags::NONE;
  }
}

// NotifyType <-> C_NOTIFY_TYPE
inline constexpr C_NOTIFY_TYPE
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
    std::cerr << "[MarshalEnums] Unknown NOTIFY_TYPE value in notifyTypeFromManaged, returning PULSER_RCVR_DISCOVERED" << std::endl;
    return C_NOTIFY_TYPE::PULSER_RCVR_DISCOVERED;
  }
}
inline constexpr JSRDotNETSDK::NOTIFY_TYPE
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
    std::cerr << "[MarshalEnums] Unknown C_NOTIFY_TYPE value in notifyTypeToManaged, returning PULSER_RCVR_DISCOVERED" << std::endl;
    return JSRDotNETSDK::NOTIFY_TYPE::PULSER_RCVR_DISCOVERED;
  }
}

} // namespace jsr::detail
