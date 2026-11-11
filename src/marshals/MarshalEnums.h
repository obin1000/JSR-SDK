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

#undef ERROR

// Conversions between the C++ enums (include/JSR-SDK/enums) and the .NET
// SDK's enums. Private to the DLL (C++/CLI only); tests reach them through
// src/ on the include path.
//
// Every C++ enum has the same names and values as its .NET counterpart
// (test/MarshalEnumsTest.cpp checks this by reflection), so the conversions
// are casts:
// - From .NET, a value passes through unchanged. A value the C++ enum doesn't
//   name reaches the caller as that number, never as a different setting.
// - To .NET, the value must be one the SDK defines. LAST_VAL (a count, not a
//   setting) and undefined values throw System::ArgumentException, which the
//   manager reports as INVALID_ARGUMENT. Flag enums accept any combination of
//   the defined bits.
namespace jsr::detail {

using namespace JSRDotNETSDK;

template <class Native, class Managed>
inline Native enumFromManaged(Managed value) {
  return static_cast<Native>(static_cast<int>(value));
}

/// True when value is a setting the SDK's enum Managed defines.
template <class Managed> bool isValidManagedEnumValue(int value) {
  System::Type ^ type = Managed::typeid;
  if (type->IsDefined(System::FlagsAttribute::typeid, false)) {
    int defined = 0;
    for each (System::Object ^ flag in System::Enum::GetValues(type)) {
      defined |= System::Convert::ToInt32(flag);
    }
    return (value & ~defined) == 0;
  }
  System::String ^ name = System::Enum::GetName(type, value);
  return name != nullptr && name != "LAST_VAL";
}

template <class Managed, class Native> Managed enumToManaged(Native value) {
  const int number = static_cast<int>(value);
  if (!isValidManagedEnumValue<Managed>(number)) {
    throw gcnew System::ArgumentException(System::String::Format(
        "{0} is not a valid {1} value", number, Managed::typeid->Name));
  }
  return static_cast<Managed>(number);
}

// The named conversions the manager and the tests use.
#define JSR_ENUM_CONVERSIONS(name, Native, Managed)                            \
  inline Native name##FromManaged(Managed value) {                             \
    return enumFromManaged<Native>(value);                                     \
  }                                                                            \
  inline Managed name##ToManaged(Native value) {                               \
    return enumToManaged<Managed>(value);                                      \
  }

JSR_ENUM_CONVERSIONS(receiverMode, C_RECEIVER_MODE, RECEIVER_MODE)
JSR_ENUM_CONVERSIONS(triggerSource, C_TRIGGER_SOURCE, TRIGGER_SOURCE)
JSR_ENUM_CONVERSIONS(triggerPolarity, C_TRIGGER_POLARITY, TRIGGER_POLARITY)
JSR_ENUM_CONVERSIONS(triggerImpedance, C_TRIGGER_IMPEDANCE, TRIGGER_IMPEDANCE)
JSR_ENUM_CONVERSIONS(pulserImpedance, C_PULSER_IMPEDANCE, PULSER_IMPEDANCE)
JSR_ENUM_CONVERSIONS(isPulsing, C_IS_PULSING, IS_PULSING)
JSR_ENUM_CONVERSIONS(powerLimit, C_POWER_LIMIT, POWER_LIMIT)
JSR_ENUM_CONVERSIONS(managerState, C_MANAGER_STATE,
                     JSRDotNETManager::MANAGER_STATE)
JSR_ENUM_CONVERSIONS(connectionType, C_CONNECTION_TYPE, CONNECTION_TYPE)
JSR_ENUM_CONVERSIONS(propertyUnits, C_PROPERTY_UNITS, PROPERTY_UNITS)
JSR_ENUM_CONVERSIONS(pulserPropertyRole, C_PULSER_PROPERTY_ROLE,
                     PulserPropertyRole)
JSR_ENUM_CONVERSIONS(statusChange, C_STATUS_CHANGE, STATUS_CHANGE)
JSR_ENUM_CONVERSIONS(pulserReceiverState, C_PULSER_RECEIVER_STATE,
                     PulserReceiverState)
JSR_ENUM_CONVERSIONS(pulserPropertyDataType, C_PULSER_PROPERTY_DATA_TYPE,
                     PulserPropertyDataType)
JSR_ENUM_CONVERSIONS(errorCode, C_ERROR_CODE, ERROR_CODE)
JSR_ENUM_CONVERSIONS(discoveryStateFlags, C_DISCOVERY_STATE_FLAGS,
                     DiscoveryStateFlags)
JSR_ENUM_CONVERSIONS(notifyType, C_NOTIFY_TYPE, JSRDotNETSDK::NOTIFY_TYPE)

#undef JSR_ENUM_CONVERSIONS

} // namespace jsr::detail
