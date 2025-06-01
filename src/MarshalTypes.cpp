#pragma once

#include "MarshalEnums.cpp"

#include "JSR-SDK/ExceptionJSRSDK.h"
#include "JSR-SDK/InstrumentID.h"
#include "JSR-SDK/JSRLibMetadata.h"
#include "JSR-SDK/PulserReceiverID.h"

#include "JSR-SDK/events/NotifyEvent.h"
#include "JSR-SDK/events/StatusChangedEvent.h"

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
 * @brief Converts a managed List (C#) to a std::vector (C++) without any
 *conversions.
 **/
template <typename managedType, typename unmanagedType>
static std::vector<unmanagedType> listToVector(IEnumerable<managedType> ^
                                               list) {
  std::vector<unmanagedType> vec;
  if (list == nullptr)
    return vec;

  // vec.reserve(list->Count);
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

  // vec.reserve(list->Count);
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

  ErrorCode errorCode = errorCodeFromManaged(managedException->ErrorCode);
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

  if (!System::String::IsNullOrEmpty(instrumentIdentity->ModelName))
    unmanaged.ModelName =
        marshal_as<std::string>(instrumentIdentity->ModelName);

  if (!System::String::IsNullOrEmpty(instrumentIdentity->PluginName))
    unmanaged.PluginName =
        marshal_as<std::string>(instrumentIdentity->PluginName);

  if (!System::String::IsNullOrEmpty(instrumentIdentity->Port))
    unmanaged.Port = marshal_as<std::string>(instrumentIdentity->Port);

  if (!System::String::IsNullOrEmpty(instrumentIdentity->SerialNum))
    unmanaged.SerialNum =
        marshal_as<std::string>(instrumentIdentity->SerialNum);

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

  if (!System::String::IsNullOrEmpty(metadata->Name))
    unmanaged.Name = marshal_as<std::string>(metadata->Name);
  if (!System::String::IsNullOrEmpty(metadata->GUID))
    unmanaged.GUID = marshal_as<std::string>(metadata->GUID);
  if (!System::String::IsNullOrEmpty(metadata->FriendlyName))
    unmanaged.FriendlyName = marshal_as<std::string>(metadata->FriendlyName);
  if (!System::String::IsNullOrEmpty(metadata->Version))
    unmanaged.Version = marshal_as<std::string>(metadata->Version);

  unmanaged.InterfaceVersion = metadata->InterfaceVersion;

  if (metadata->SupportedModels != nullptr)
    unmanaged.SupportedModels = listToVectorMarshall < System::String ^,
    std::string > (metadata->SupportedModels);

  if (metadata->OpenOptions != nullptr)
    unmanaged.OpenOptions = listToVectorMarshall < System::String ^,
    std::string > (metadata->OpenOptions);

  if (metadata->ConnectionType != nullptr)
    unmanaged.ConnectionType =
        listToVectorMarshall<CONNECTION_TYPE, ConnectionType>(
            metadata->ConnectionType);

  return unmanaged;
}

// Converting JSR-SDK::StatusChangedEvent <->
// JSRDotNETSDK::EventArgsStatusChange
static StatusChangedEvent statusChangedEventFromManaged(EventArgsStatusChange ^
                                                        managedEvent) {
  StatusChangedEvent unmanagedEvent{};

  if (managedEvent == nullptr)
    return unmanagedEvent;

  unmanagedEvent.pulserProperty =
      marshal_as<std::string>(managedEvent->PulserProperty);

  unmanagedEvent.pulserState =
      pulserReceiverStateFromManaged(managedEvent->PulserState);

  if (managedEvent->NewValue != nullptr)
    unmanagedEvent.newValue =
        marshal_as<std::string>(managedEvent->NewValue->ToString());

  unmanagedEvent.dataType =
      pulserPropertyDataTypeFromManaged(managedEvent->DataType);
  unmanagedEvent.changeType = statusChangeFromManaged(managedEvent->ChangeType);

  if (managedEvent->PulserReceiverId != nullptr)
    unmanagedEvent.pulserReceiverId =
        pulsereceiverFromManaged(managedEvent->PulserReceiverId);

  if (!System::String::IsNullOrEmpty(managedEvent->ErrorMessage))
    unmanagedEvent.errorMessage =
        marshal_as<std::string>(managedEvent->ErrorMessage);

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

  if (!System::String::IsNullOrEmpty(managedEvent->Model))
    unmanagedEvent.model = marshal_as<std::string>(managedEvent->Model);

  unmanagedEvent.state = pulserReceiverStateFromManaged(managedEvent->State);

  if (managedEvent->NewValue != nullptr)
    unmanagedEvent.newValue =
        marshal_as<std::string>(managedEvent->NewValue->ToString());

  if (!System::String::IsNullOrEmpty(managedEvent->PropertyName))
    unmanagedEvent.propertyName =
        marshal_as<std::string>(managedEvent->PropertyName);

  unmanagedEvent.wasSelected = managedEvent->WasSelected;

  if (!System::String::IsNullOrEmpty(managedEvent->ExceptionTypeInfo))
    unmanagedEvent.exceptionTypeInfo =
        marshal_as<std::string>(managedEvent->ExceptionTypeInfo);

  if (!System::String::IsNullOrEmpty(managedEvent->ErrorMsg))
    unmanagedEvent.errorMsg = marshal_as<std::string>(managedEvent->ErrorMsg);

  if (!System::String::IsNullOrEmpty(managedEvent->ErrorText))
    unmanagedEvent.errorText = marshal_as<std::string>(managedEvent->ErrorText);

  unmanagedEvent.maxFrequency = managedEvent->MaxFrequency;

  if (managedEvent->Info != nullptr)
    unmanagedEvent.info = listToVectorMarshall < System::String ^,
    std::string > (managedEvent->Info);

  unmanagedEvent.prIndex = managedEvent->PRIndex;

  if (managedEvent->PulserReceiverId != nullptr)
    unmanagedEvent.pulserReceiverId =
        pulsereceiverFromManaged(managedEvent->PulserReceiverId);

  if (!System::String::IsNullOrEmpty(managedEvent->Serial))
    unmanagedEvent.serial = marshal_as<std::string>(managedEvent->Serial);

  unmanagedEvent.discoverState =
      discoveryStateFlagsFromManaged(managedEvent->DiscoverState);

  if (managedEvent->InstrumentId != nullptr)
    unmanagedEvent.instrumentId =
        instrumentFromManaged(managedEvent->InstrumentId);

  if (!System::String::IsNullOrEmpty(managedEvent->PluginName))
    unmanagedEvent.pluginName =
        marshal_as<std::string>(managedEvent->PluginName);

  unmanagedEvent.notifyType = notifyTypeFromManaged(managedEvent->NotifyType);
  unmanagedEvent.dataType =
      pulserPropertyDataTypeFromManaged(managedEvent->DataType);

  return unmanagedEvent;
}
