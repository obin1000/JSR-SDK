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
  unmanaged.SupportedModels = listToVectorMarshall < System::String ^,
  std::string > (metadata->SupportedModels);
  unmanaged.OpenOptions = listToVectorMarshall < System::String ^,
  std::string > (metadata->OpenOptions);
  unmanaged.ConnectionType =
      listToVectorMarshall<CONNECTION_TYPE, ConnectionType>(
          metadata->ConnectionType);

  return unmanaged;
}

// Converting JSR-SDK::StatusChangedEvent <->
// JSRDotNETSDK::EventArgsStatusChange
static StatusChangedEvent statusChangedEventFromManaged(EventArgsStatusChange ^
                                                        managedEvent) {
  StatusChangedEvent unmanagedEvent;
  if (managedEvent == nullptr) {
    return unmanagedEvent;
  }
  unmanagedEvent.pulserProperty =
      marshal_as<std::string>(managedEvent->NewValue->ToString());
  unmanagedEvent.pulserState =
      pulserReceiverStateFromManaged(managedEvent->PulserState);
  unmanagedEvent.newValue =
      marshal_as<std::string>(managedEvent->NewValue->ToString());
  unmanagedEvent.dataType =
      pulserPropertyDataTypeFromManaged(managedEvent->DataType);
  unmanagedEvent.changeType = statusChangeFromManaged(managedEvent->ChangeType);
  unmanagedEvent.pulserReceiverId =
      pulsereceiverFromManaged(managedEvent->PulserReceiverId);

  unmanagedEvent.errorMessage =
      marshal_as<std::string>(managedEvent->ErrorMessage);
  unmanagedEvent.errorCode = errorCodeFromManaged(managedEvent->ErrorCode);
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

  unmanagedEvent.model =
      msclr::interop::marshal_as<std::string>(managedEvent->Model);
  unmanagedEvent.state = pulserReceiverStateFromManaged(managedEvent->State);
  unmanagedEvent.newValue =
      marshal_as<std::string>(managedEvent->NewValue->ToString());
  unmanagedEvent.propertyName =
      msclr::interop::marshal_as<std::string>(managedEvent->PropertyName);
  unmanagedEvent.wasSelected = managedEvent->WasSelected;
  unmanagedEvent.exceptionTypeInfo =
      msclr::interop::marshal_as<std::string>(managedEvent->ExceptionTypeInfo);
  unmanagedEvent.errorMsg =
      msclr::interop::marshal_as<std::string>(managedEvent->ErrorMsg);
  unmanagedEvent.errorText =
      msclr::interop::marshal_as<std::string>(managedEvent->ErrorText);
  unmanagedEvent.maxFrequency = managedEvent->MaxFrequency;
  unmanagedEvent.info = listToVectorMarshall < System::String ^,
  std::string > (managedEvent->Info);
  unmanagedEvent.prIndex = managedEvent->PRIndex;
  unmanagedEvent.pulserReceiverId =
      pulsereceiverFromManaged(managedEvent->PulserReceiverId);
  unmanagedEvent.serial =
      msclr::interop::marshal_as<std::string>(managedEvent->Serial);
  unmanagedEvent.discoverState =
      discoveryStateFlagsFromManaged(managedEvent->DiscoverState);
  unmanagedEvent.instrumentId =
      instrumentFromManaged(managedEvent->InstrumentId);
  unmanagedEvent.pluginName =
      msclr::interop::marshal_as<std::string>(managedEvent->PluginName);
  unmanagedEvent.notifyType = notifyTypeFromManaged(managedEvent->NotifyType);
  unmanagedEvent.dataType =
      pulserPropertyDataTypeFromManaged(managedEvent->DataType);

  return unmanagedEvent;
}
