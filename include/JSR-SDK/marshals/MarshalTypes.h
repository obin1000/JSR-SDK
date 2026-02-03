#pragma once

#include "MarshalEnums.h"
#include "JSR-SDK/structs/ExceptionJSRSDK.h"
#include "JSR-SDK/structs/InstrumentID.h"
#include "JSR-SDK/structs/JSRLibMetadata.h"
#include "JSR-SDK/structs/PulserReceiverID.h"
#include "JSR-SDK/events/NotifyEvent.h"
#include "JSR-SDK/events/StatusChangedEvent.h"
#include "JSR-SDK/boundary/CString.h"
#include "JSR-SDK/boundary/CVector.h"

#include <msclr/marshal_cppstd.h>

using namespace JSRDotNETSDK;
using namespace System::Collections::Generic;
using namespace msclr::interop;

/**
 * @file MarshalTypes.h
 * @brief This file contains the conversion functions between JSR-SDK types and
 * JSRDotNETSDK types.
 * These can't be exposed through the headers as it contains references to
 * managed objects.
 *
 **/

/**
 * @brief Converts a managed List (C#) to a CVector (C++) without any
 *conversions.
 **/
template <typename managedType, typename unmanagedType>
static CVector<unmanagedType> listToCVector(IEnumerable<managedType> ^list) {
  if (list == nullptr)
    return CVector<unmanagedType>();

  // Count elements first
  int count = 0;
  for each (managedType s in list)
    count++;

  if (count == 0)
    return CVector<unmanagedType>();

  unmanagedType *buffer = new unmanagedType[count];
  int index = 0;
  for each (managedType s in list)
    buffer[index++] = s;

  return CVector<unmanagedType>(buffer, count, count);
}

/**
 * @brief Converts a managed List (C#) to a CVector of CStrings.
 **/
template <typename managedType>
static CVector<CString>
listToCVectorString(IEnumerable<managedType> ^ list) {
  if (list == nullptr)
    return CVector<CString>();

  // Count elements first
  int count = 0;
  for each (managedType s in list)
    count++;

  if (count == 0)
    return CVector<CString>();

  CString *buffer = new CString[count];
  int index = 0;
  for each (managedType s in list) {
    std::string str = marshal_as<std::string>(s);
    buffer[index++] = CString::from_std_string(str);
  }

  return CVector<CString>(buffer, count, count);
}

/**
 * @brief Converts a managed List (C#) to a std::vector (C++) without any
 *conversions.
 **/
template <typename managedType, typename unmanagedType>
static std::vector<unmanagedType> listToVector(IEnumerable<managedType> ^
                                               list) {
  std::vector<unmanagedType> vec;
  if (list == nullptr)
    return vec;

  for each (managedType s in list)
    vec.push_back(s);

  return vec;
}

/**
 * @brief Converts a managed List (C#) to a std::vector (C++) converting using
 * marshal_as.
 **/
template <typename managedType, typename unmanagedType>
static std::vector<unmanagedType>
listToVectorMarshall(IEnumerable<managedType> ^ list) {
  std::vector<unmanagedType> vec;
  if (list == nullptr)
    return vec;

  for each (managedType s in list)
    vec.push_back(marshal_as<unmanagedType>(s));

  return vec;
}

// converting JSR-SDK::ExceptionJSRSDK <-> JSRDotNETSDK::ExceptionJSRDotNET
static ExceptionJSRSDK exceptionFromManaged(ExceptionJSRDotNET ^
                                            managedException) {
  if (managedException == nullptr) {
    return ExceptionJSRSDK();
  }

  C_ERROR_CODE errorCode = errorCodeFromManaged(managedException->ErrorCode);
  std::string message =
      msclr::interop::marshal_as<std::string>(managedException->Message);
  std::string innerMessage =
      managedException->InnerException != nullptr
          ? msclr::interop::marshal_as<std::string>(
                managedException->InnerException->Message)
          : "None";

  return ExceptionJSRSDK(errorCode, message, innerMessage);
}

// Converting JSR-SDK::InstrumentID <-> JSRDotNETSDK::IInstrumentIdentity^
static InstrumentID instrumentFromManaged(IInstrumentIdentity ^
                                          instrumentIdentity) {
  InstrumentID unmanaged{};

  if (instrumentIdentity == nullptr)
    return unmanaged;

  if (!System::String::IsNullOrEmpty(instrumentIdentity->ModelName)) {
    std::string str = marshal_as<std::string>(instrumentIdentity->ModelName);
    unmanaged.ModelName = CString::from_std_string(str);
  }

  if (!System::String::IsNullOrEmpty(instrumentIdentity->PluginName)) {
    std::string str = marshal_as<std::string>(instrumentIdentity->PluginName);
    unmanaged.PluginName = CString::from_std_string(str);
  }

  if (!System::String::IsNullOrEmpty(instrumentIdentity->Port)) {
    std::string str = marshal_as<std::string>(instrumentIdentity->Port);
    unmanaged.Port = CString::from_std_string(str);
  }

  if (!System::String::IsNullOrEmpty(instrumentIdentity->SerialNum)) {
    std::string str = marshal_as<std::string>(instrumentIdentity->SerialNum);
    unmanaged.SerialNum = CString::from_std_string(str);
  }

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
  if (metadata == nullptr) {
    return unmanaged;
  }

  if (!System::String::IsNullOrEmpty(metadata->Name)) {
    std::string str = marshal_as<std::string>(metadata->Name);
    unmanaged.Name = CString::from_std_string(str);
  }

  if (!System::String::IsNullOrEmpty(metadata->GUID)) {
    std::string str = marshal_as<std::string>(metadata->GUID);
    unmanaged.GUID = CString::from_std_string(str);
  }

  if (!System::String::IsNullOrEmpty(metadata->FriendlyName)) {
    std::string str = marshal_as<std::string>(metadata->FriendlyName);
    unmanaged.FriendlyName = CString::from_std_string(str);
  }

  if (!System::String::IsNullOrEmpty(metadata->Version)) {
    std::string str = marshal_as<std::string>(metadata->Version);
    unmanaged.Version = CString::from_std_string(str);
  }

  unmanaged.InterfaceVersion = metadata->InterfaceVersion;

  if (metadata->SupportedModels != nullptr)
    unmanaged.SupportedModels = listToCVectorString<System::String^>(metadata->SupportedModels);

  if (metadata->OpenOptions != nullptr)
    unmanaged.OpenOptions = listToCVectorString<System::String^>(metadata->OpenOptions);

  if (metadata->ConnectionType != nullptr) {
    // Count elements first
    int count = 0;
    for each (CONNECTION_TYPE ct in metadata->ConnectionType)
      count++;

    if (count > 0) {
      C_CONNECTION_TYPE *buffer = new C_CONNECTION_TYPE[count];
      int index = 0;
      for each (CONNECTION_TYPE ct in metadata->ConnectionType)
        buffer[index++] = connectionTypeFromManaged(ct);

      unmanaged.ConnectionType = CVector<C_CONNECTION_TYPE>(buffer, count, count);
    }
  }

  return unmanaged;
}

// Converting JSR-SDK::StatusChangedEvent <->
// JSRDotNETSDK::EventArgsStatusChange
static StatusChangedEvent statusChangedEventFromManaged(EventArgsStatusChange ^
                                                        managedEvent) {
  StatusChangedEvent unmanagedEvent{};

  if (managedEvent == nullptr)
    return unmanagedEvent;

  if (!System::String::IsNullOrEmpty(managedEvent->PulserProperty)) {
    std::string str = marshal_as<std::string>(managedEvent->PulserProperty);
    unmanagedEvent.pulserProperty = CString::from_std_string(str);
  }

  unmanagedEvent.pulserState =
      pulserReceiverStateFromManaged(managedEvent->PulserState);

  if (managedEvent->NewValue != nullptr) {
    std::string str = marshal_as<std::string>(managedEvent->NewValue->ToString());
    unmanagedEvent.newValue = CString::from_std_string(str);
  }

  unmanagedEvent.dataType =
      pulserPropertyDataTypeFromManaged(managedEvent->DataType);
  unmanagedEvent.changeType = statusChangeFromManaged(managedEvent->ChangeType);

  if (managedEvent->PulserReceiverId != nullptr)
    unmanagedEvent.pulserReceiverId =
        pulsereceiverFromManaged(managedEvent->PulserReceiverId);

  if (!System::String::IsNullOrEmpty(managedEvent->ErrorMessage)) {
    std::string str = marshal_as<std::string>(managedEvent->ErrorMessage);
    unmanagedEvent.errorMessage = CString::from_std_string(str);
  }

  unmanagedEvent.errorCode = errorCodeFromManaged(managedEvent->ErrorCode);

  if (managedEvent->ThrownException != nullptr)
    unmanagedEvent.thrownException =
        exceptionFromManaged(managedEvent->ThrownException);

  return unmanagedEvent;
}

// Converting JSR-SDK::NotifyEvent <-> JSRDotNETSDK::EventArgsManagerNotify
static NotifyEvent notifyEventFromManaged(EventArgsManagerNotify ^
                                          managedEvent) {
  NotifyEvent unmanagedEvent;
  if (managedEvent == nullptr) {
    return unmanagedEvent;
  }

  if (!System::String::IsNullOrEmpty(managedEvent->Model)) {
    std::string str = marshal_as<std::string>(managedEvent->Model);
    unmanagedEvent.model = CString::from_std_string(str);
  }

  unmanagedEvent.state = pulserReceiverStateFromManaged(managedEvent->State);

  if (managedEvent->NewValue != nullptr) {
    std::string str = marshal_as<std::string>(managedEvent->NewValue->ToString());
    unmanagedEvent.newValue = CString::from_std_string(str);
  }

  if (!System::String::IsNullOrEmpty(managedEvent->PropertyName)) {
    std::string str = marshal_as<std::string>(managedEvent->PropertyName);
    unmanagedEvent.propertyName = CString::from_std_string(str);
  }

  unmanagedEvent.wasSelected = managedEvent->WasSelected;

  if (!System::String::IsNullOrEmpty(managedEvent->ExceptionTypeInfo)) {
    std::string str = marshal_as<std::string>(managedEvent->ExceptionTypeInfo);
    unmanagedEvent.exceptionTypeInfo = CString::from_std_string(str);
  }

  if (!System::String::IsNullOrEmpty(managedEvent->ErrorMsg)) {
    std::string str = marshal_as<std::string>(managedEvent->ErrorMsg);
    unmanagedEvent.errorMsg = CString::from_std_string(str);
  }

  if (!System::String::IsNullOrEmpty(managedEvent->ErrorText)) {
    std::string str = marshal_as<std::string>(managedEvent->ErrorText);
    unmanagedEvent.errorText = CString::from_std_string(str);
  }

  unmanagedEvent.maxFrequency = managedEvent->MaxFrequency;

  if (managedEvent->Info != nullptr)
    unmanagedEvent.info = listToCVectorString<System::String^>(managedEvent->Info);

  unmanagedEvent.prIndex = managedEvent->PRIndex;

  if (managedEvent->PulserReceiverId != nullptr)
    unmanagedEvent.pulserReceiverId =
        pulsereceiverFromManaged(managedEvent->PulserReceiverId);

  if (!System::String::IsNullOrEmpty(managedEvent->Serial)) {
    std::string str = marshal_as<std::string>(managedEvent->Serial);
    unmanagedEvent.serial = CString::from_std_string(str);
  }

  unmanagedEvent.discoverState =
      discoveryStateFlagsFromManaged(managedEvent->DiscoverState);

  if (managedEvent->InstrumentId != nullptr)
    unmanagedEvent.instrumentId =
        instrumentFromManaged(managedEvent->InstrumentId);

  if (!System::String::IsNullOrEmpty(managedEvent->PluginName)) {
    std::string str = marshal_as<std::string>(managedEvent->PluginName);
    unmanagedEvent.pluginName = CString::from_std_string(str);
  }

  unmanagedEvent.notifyType = notifyTypeFromManaged(managedEvent->NotifyType);
  unmanagedEvent.dataType =
      pulserPropertyDataTypeFromManaged(managedEvent->DataType);

  return unmanagedEvent;
}

// Marshal template specializations for complex types
namespace msclr::interop {

// ExceptionJSRSDK marshals
template <>
inline ExceptionJSRSDK
marshal_as<ExceptionJSRSDK, ExceptionJSRDotNET^>(ExceptionJSRDotNET^ const& from) {
  return exceptionFromManaged(from);
}

// InstrumentID marshals
template <>
inline InstrumentID
marshal_as<InstrumentID, IInstrumentIdentity^>(IInstrumentIdentity^ const& from) {
  return instrumentFromManaged(from);
}

// PulserReceiverID marshals
template <>
inline PulserReceiverID
marshal_as<PulserReceiverID, IPulserReceiverIdentity^>(IPulserReceiverIdentity^ const& from) {
  return pulsereceiverFromManaged(from);
}

// JSRLibMetadata marshals
template <>
inline JSRLibMetadata
marshal_as<JSRLibMetadata, IJSRDotNETLibMetadata^>(IJSRDotNETLibMetadata^ const& from) {
  return libMetadataFromManaged(from);
}

// StatusChangedEvent marshals
template <>
inline StatusChangedEvent
marshal_as<StatusChangedEvent, EventArgsStatusChange^>(EventArgsStatusChange^ const& from) {
  return statusChangedEventFromManaged(from);
}

// NotifyEvent marshals
template <>
inline NotifyEvent
marshal_as<NotifyEvent, EventArgsManagerNotify^>(EventArgsManagerNotify^ const& from) {
  return notifyEventFromManaged(from);
}

} // namespace msclr::interop


