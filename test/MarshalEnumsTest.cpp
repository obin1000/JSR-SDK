#include "../src/MarshalEnums.cpp"
#include "pch.h"
#include <gtest/gtest.h>

using namespace JSRDotNETSDK;

TEST(EnumConversionTest, ReceiverMode_RoundTrip) {
  RECEIVER_MODE values[] = {RECEIVER_MODE::BOTH, RECEIVER_MODE::ECHO,
                            RECEIVER_MODE::LAST_VAL, RECEIVER_MODE::THRU};
  for (auto v : values) {
    RecieverMode native = receiverModeFromManaged(v);
    RECEIVER_MODE back = receiverModeToManaged(native);
    EXPECT_EQ(v, back);
  }
}

TEST(EnumConversionTest, TriggerSource_RoundTrip) {
  TRIGGER_SOURCE values[] = {TRIGGER_SOURCE::INTERNAL, TRIGGER_SOURCE::EXTERNAL,
                             TRIGGER_SOURCE::LAST_VAL, TRIGGER_SOURCE::SLAVE};
  for (auto v : values) {
    TriggerSource native = triggerSourceFromManaged(v);
    TRIGGER_SOURCE back = triggerSourceToManaged(native);
    EXPECT_EQ(v, back);
  }
}

TEST(EnumConversionTest, TriggerPolarity_RoundTrip) {
  TRIGGER_POLARITY values[] = {TRIGGER_POLARITY::FALLING,
                               TRIGGER_POLARITY::RISING,
                               TRIGGER_POLARITY::LAST_VAL};
  for (auto v : values) {
    TriggerPolarity native = triggerPolarityFromManaged(v);
    TRIGGER_POLARITY back = triggerPolarityToManaged(native);
    EXPECT_EQ(v, back);
  }
}

TEST(EnumConversionTest, TriggerImpedance_RoundTrip) {
  TRIGGER_IMPEDANCE values[] = {TRIGGER_IMPEDANCE::HIGH_Z,
                                TRIGGER_IMPEDANCE::LOW_Z,
                                TRIGGER_IMPEDANCE::LAST_VAL};
  for (auto v : values) {
    TriggerImpedance native = triggerImpedanceFromManaged(v);
    TRIGGER_IMPEDANCE back = triggerImpedanceToManaged(native);
    EXPECT_EQ(v, back);
  }
}

TEST(EnumConversionTest, PulserImpedance_RoundTrip) {
  PULSER_IMPEDANCE values[] = {PULSER_IMPEDANCE::HIGH_Z,
                               PULSER_IMPEDANCE::LOW_Z,
                               PULSER_IMPEDANCE::LAST_VAL};
  for (auto v : values) {
    PulserImpedance native = pulserImpedanceFromManaged(v);
    PULSER_IMPEDANCE back = pulserImpedanceToManaged(native);
    EXPECT_EQ(v, back);
  }
}

TEST(EnumConversionTest, IsPulsing_RoundTrip) {
  IS_PULSING values[] = {IS_PULSING::ACTIVE, IS_PULSING::INACTIVE};
  for (auto v : values) {
    IsPulsing native = isPulsingFromManaged(v);
    IS_PULSING back = isPulsingToManaged(native);
    EXPECT_EQ(v, back);
  }
}

TEST(EnumConversionTest, PowerLimit_RoundTrip) {
  POWER_LIMIT values[] = {POWER_LIMIT::OVER_LIMIT, POWER_LIMIT::WITHIN_LIMIT};
  for (auto v : values) {
    PowerLimit native = powerLimitFromManaged(v);
    POWER_LIMIT back = powerLimitToManaged(native);
    EXPECT_EQ(v, back);
  }
}

TEST(EnumConversionTest, ManagerState_RoundTrip) {
  using MS = JSRDotNETManager::MANAGER_STATE;
  MS values[] = {MS::NOT_STARTED, MS::PLUGINS_LOADED, MS::RUNNING,
                 MS::SHUTTING_DOWN};
  for (auto v : values) {
    ManagerState native = managerStateFromManaged(v);
    MS back = managerStateToManaged(native);
    EXPECT_EQ(v, back);
  }
}

TEST(EnumConversionTest, ConnectionType_RoundTrip) {
  CONNECTION_TYPE values[] = {
      CONNECTION_TYPE::SOFTWARE, CONNECTION_TYPE::SERIALPORT,
      CONNECTION_TYPE::PCI,      CONNECTION_TYPE::USB,
      CONNECTION_TYPE::ETHERNET, CONNECTION_TYPE::FIREWIRE,
      CONNECTION_TYPE::BLUETOOTH};
  for (auto v : values) {
    ConnectionType native = connectionTypeFromManaged(v);
    CONNECTION_TYPE back = connectionTypeToManaged(native);
    EXPECT_EQ(v, back);
  }
}

TEST(EnumConversionTest, PropertyUnits_RoundTrip) {
  PROPERTY_UNITS values[] = {
      PROPERTY_UNITS::UNIT_NONE,        PROPERTY_UNITS::UNIT_HERTZ,
      PROPERTY_UNITS::UNIT_KILOHERTZ,   PROPERTY_UNITS::UNIT_MEGAHERTZ,
      PROPERTY_UNITS::UNIT_MICROJOULES, PROPERTY_UNITS::UNIT_JOULES,
      PROPERTY_UNITS::UNIT_WATTS,       PROPERTY_UNITS::UNIT_DB,
      PROPERTY_UNITS::UNIT_VOLTS,       PROPERTY_UNITS::UNIT_OHMS,
      PROPERTY_UNITS::UNIT_PICOFARADS};
  for (auto v : values) {
    PropertyUnits native = propertyUnitsFromManaged(v);
    PROPERTY_UNITS back = propertyUnitsToManaged(native);
    EXPECT_EQ(v, back);
  }
}

TEST(EnumConversionTest, PulserPropertyRole_RoundTrip) {
  PulserPropertyRole values[] = {PulserPropertyRole::DIRECT,
                                 PulserPropertyRole::DIRECTSUPPORTED,
                                 PulserPropertyRole::SPECIFICVALUESUPPORTED,
                                 PulserPropertyRole::DIRECTVALUES,
                                 PulserPropertyRole::STEPSIZE,
                                 PulserPropertyRole::STEPSIZESUPPORTED,
                                 PulserPropertyRole::INDEX,
                                 PulserPropertyRole::INDEXMAX,
                                 PulserPropertyRole::INDEXSUPPORTED,
                                 PulserPropertyRole::MAX,
                                 PulserPropertyRole::MIN,
                                 PulserPropertyRole::INDEXVALUES,
                                 PulserPropertyRole::NUMERATOR,
                                 PulserPropertyRole::OTHER};
  for (auto v : values) {
    PulserPropertyRoles native = pulserPropertyRoleFromManaged(v);
    PulserPropertyRole back = pulserPropertyRoleToManaged(native);
    EXPECT_EQ(v, back);
  }
}

TEST(EnumConversionTest, StatusChange_RoundTrip) {
  STATUS_CHANGE values[] = {
      STATUS_CHANGE::INSTRUMENT_DISCONNECT, STATUS_CHANGE::PROPERTY_CHANGE,
      STATUS_CHANGE::OPERATIONAL_FAULT, STATUS_CHANGE::STATE_CHANGE};
  for (auto v : values) {
    StatusChange native = statusChangeFromManaged(v);
    STATUS_CHANGE back = statusChangeToManaged(native);
    EXPECT_EQ(v, back);
  }
}

TEST(EnumConversionTest, PulserReceiverState_RoundTrip) {
  PulserReceiverState values[] = {
      PulserReceiverState::DETECTED,  PulserReceiverState::INITIALIZING,
      PulserReceiverState::READY,     PulserReceiverState::ERROR,
      PulserReceiverState::NO_PULSER, PulserReceiverState::DETACHED};
  for (auto v : values) {
    PulserReceiverStateC native = pulserReceiverStateFromManaged(v);
    PulserReceiverState back = pulserReceiverStateToManaged(native);
    EXPECT_EQ(v, back);
  }
}

TEST(EnumConversionTest, PulserPropertyDataType_RoundTrip) {
  PulserPropertyDataType values[] = {PulserPropertyDataType::INTEGER,
                                     PulserPropertyDataType::BYTE,
                                     PulserPropertyDataType::DOUBLE,
                                     PulserPropertyDataType::BOOLEAN,
                                     PulserPropertyDataType::STRING,
                                     PulserPropertyDataType::BYTE_ARRAY,
                                     PulserPropertyDataType::INTEGER_ARRAY,
                                     PulserPropertyDataType::DOUBLE_ARRAY,
                                     PulserPropertyDataType::BOOLEAN_ARRAY,
                                     PulserPropertyDataType::STRING_ARRAY,
                                     PulserPropertyDataType::ENUM,
                                     PulserPropertyDataType::ID,
                                     PulserPropertyDataType::OTHER};
  for (auto v : values) {
    PulserPropertyDataTypeC native = pulserPropertyDataTypeFromManaged(v);
    PulserPropertyDataType back = pulserPropertyDataTypeToManaged(native);
    EXPECT_EQ(v, back);
  }
}

TEST(EnumConversionTest, ErrorCode_RoundTrip) {
  ERROR_CODE values[] = {ERROR_CODE::UNKNOWN,
                         ERROR_CODE::DEVICE_NOT_FOUND,
                         ERROR_CODE::PLUGINS_NOT_LOADED,
                         ERROR_CODE::NOT_FOUND,
                         ERROR_CODE::COMMUNICATION_FAILURE,
                         ERROR_CODE::RESP_ADDRESS_MISMATCH,
                         ERROR_CODE::RESP_INVALID_LENGTH,
                         ERROR_CODE::PORT_OPEN_FAILED,
                         ERROR_CODE::TIMEOUT,
                         ERROR_CODE::VALUE_OUT_RANGE,
                         ERROR_CODE::OPERATION_NOT_SUPPORTED,
                         ERROR_CODE::HARDWARE_FAILURE,
                         ERROR_CODE::INTERNAL_SOFTWARE_FAULT,
                         ERROR_CODE::PROPERTY_WRITE_NOT_FINISHED_YET,
                         ERROR_CODE::NO_PULSER_RCVR_SELECTED,
                         ERROR_CODE::INVALID_ARGUMENT,
                         ERROR_CODE::NULL_POINTER,
                         ERROR_CODE::OPERATION_FAILED,
                         ERROR_CODE::VERSION_MISMATCH,
                         ERROR_CODE::PLUGIN_LOAD_ERROR,
                         ERROR_CODE::DEVICE_DISCOVERY_ERROR,
                         ERROR_CODE::DEVICE_NOT_READY,
                         ERROR_CODE::SUCCESS};
  for (auto v : values) {
    ErrorCode native = errorCodeFromManaged(v);
    ERROR_CODE back = errorCodeToManaged(native);
    EXPECT_EQ(v, back);
  }
}

TEST(EnumConversionTest, DiscoveryStateFlags_RoundTrip) {
  DiscoveryStateFlags values[] = {
      DiscoveryStateFlags::NONE, DiscoveryStateFlags::ACTIVE,
      DiscoveryStateFlags::PENDING, DiscoveryStateFlags::STOPPED_ON_ERR};
  for (auto v : values) {
    DiscoveryStateFlagsC native = discoveryStateFlagsFromManaged(v);
    DiscoveryStateFlags back = discoveryStateFlagsToManaged(native);
    EXPECT_EQ(v, back);
  }
}

TEST(EnumConversionTest, NotifyType_RoundTrip) {
  NOTIFY_TYPE values[] = {NOTIFY_TYPE::PULSER_RCVR_DISCOVERED,
                          NOTIFY_TYPE::CURRENT_PULSER_RCVR_CHANGED,
                          NOTIFY_TYPE::PULSER_RCVR_DETACH,
                          NOTIFY_TYPE::DISCOVERY_STARTED,
                          NOTIFY_TYPE::DISCOVERY_COMPLETE,
                          NOTIFY_TYPE::PROPERTY_CHANGE,
                          NOTIFY_TYPE::STATE_CHANGE,
                          NOTIFY_TYPE::ERROR};
  for (auto v : values) {
    NotifyType native = notifyTypeFromManaged(v);
    NOTIFY_TYPE back = notifyTypeToManaged(native);
    EXPECT_EQ(v, back);
  }
}
