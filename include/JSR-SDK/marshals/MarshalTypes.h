#pragma once

#include "MarshalEnums.h"
#include "JSR-SDK/structs/ExceptionJSRSDK.h"
#include "JSR-SDK/structs/InstrumentID.h"
#include "JSR-SDK/structs/JSRLibMetadata.h"
#include "JSR-SDK/structs/PulserReceiverID.h"
#include "JSR-SDK/events/NotifyEvent.h"
#include "JSR-SDK/events/StatusChangedEvent.h"

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
    ExceptionJSRSDK ex;
    ExceptionJSRSDK_Init(&ex);
    return ex;
  }

  C_ERROR_CODE errorCode = errorCodeFromManaged(managedException->ErrorCode);
  std::string message =
      msclr::interop::marshal_as<std::string>(managedException->Message);
  std::string innerMessage =
      managedException->InnerException != nullptr
          ? msclr::interop::marshal_as<std::string>(
                managedException->InnerException->Message)
          : "None";

  return ExceptionJSRSDK_CreateWithInner(errorCode, message.c_str(), innerMessage.c_str());
}

// Converting JSR-SDK::InstrumentID <-> JSRDotNETSDK::IInstrumentIdentity^
static InstrumentID instrumentFromManaged(IInstrumentIdentity ^
                                          instrumentIdentity) {
  InstrumentID unmanaged;
  InstrumentID_Init(&unmanaged);

  if (instrumentIdentity == nullptr)
    return unmanaged;

  if (!System::String::IsNullOrEmpty(instrumentIdentity->ModelName))
    JSRString_Set(&unmanaged.ModelName,
        marshal_as<std::string>(instrumentIdentity->ModelName).c_str());

  if (!System::String::IsNullOrEmpty(instrumentIdentity->PluginName))
    JSRString_Set(&unmanaged.PluginName,
        marshal_as<std::string>(instrumentIdentity->PluginName).c_str());

  if (!System::String::IsNullOrEmpty(instrumentIdentity->Port))
    JSRString_Set(&unmanaged.Port,
        marshal_as<std::string>(instrumentIdentity->Port).c_str());

  if (!System::String::IsNullOrEmpty(instrumentIdentity->SerialNum))
    JSRString_Set(&unmanaged.SerialNum,
        marshal_as<std::string>(instrumentIdentity->SerialNum).c_str());

  return unmanaged;
}

// Converting JSR-SDK::PulserReceiverID <->
// JSRDotNETSDK::IPulserReceiverIdentity^
static PulserReceiverID pulsereceiverFromManaged(IPulserReceiverIdentity ^
                                                 instrumentIdentity) {
  PulserReceiverID unmanaged;
  PulserReceiverID_Init(&unmanaged);
  unmanaged.InstrumentId =
      instrumentFromManaged(instrumentIdentity->InstrumentId);
  unmanaged.PulserReceiverIndex = instrumentIdentity->PulserReceiverIndex;
  return unmanaged;
}

// Converting JSR-SDK::JSRLibMetadata <-> JSRDotNETSDK::IJSRDotNETLibMetadata^
static JSRLibMetadata libMetadataFromManaged(IJSRDotNETLibMetadata ^ metadata) {
  JSRLibMetadata unmanaged;
  JSRLibMetadata_Init(&unmanaged);
  
  if (metadata == nullptr) {
    return unmanaged;
  }

  if (!System::String::IsNullOrEmpty(metadata->Name))
    JSRString_Set(&unmanaged.Name, marshal_as<std::string>(metadata->Name).c_str());
  if (!System::String::IsNullOrEmpty(metadata->GUID))
    JSRString_Set(&unmanaged.GUID, marshal_as<std::string>(metadata->GUID).c_str());
  if (!System::String::IsNullOrEmpty(metadata->FriendlyName))
    JSRString_Set(&unmanaged.FriendlyName, marshal_as<std::string>(metadata->FriendlyName).c_str());
  if (!System::String::IsNullOrEmpty(metadata->Version))
    JSRString_Set(&unmanaged.Version, marshal_as<std::string>(metadata->Version).c_str());

  unmanaged.InterfaceVersion = metadata->InterfaceVersion;

  if (metadata->SupportedModels != nullptr) {
    int idx = 0;
    for each (System::String^ model in metadata->SupportedModels) {
      if (idx >= JSR_METADATA_MAX_MODELS) break;
      JSRString_Set(&unmanaged.SupportedModels[idx++], 
                    marshal_as<std::string>(model).c_str());
    }
    unmanaged.SupportedModelsCount = idx;
  }

  if (metadata->OpenOptions != nullptr) {
    int idx = 0;
    for each (System::String^ option in metadata->OpenOptions) {
      if (idx >= JSR_METADATA_MAX_OPTIONS) break;
      JSRString_Set(&unmanaged.OpenOptions[idx++],
                    marshal_as<std::string>(option).c_str());
    }
    unmanaged.OpenOptionsCount = idx;
  }

  if (metadata->ConnectionType != nullptr) {
    int idx = 0;
    for each (CONNECTION_TYPE type in metadata->ConnectionType) {
      if (idx >= JSR_METADATA_MAX_CONNECTIONS) break;
      unmanaged.ConnectionType[idx++] = connectionTypeFromManaged(type);
    }
    unmanaged.ConnectionTypeCount = idx;
  }

  return unmanaged;
}

// Converting JSR-SDK::StatusChangedEvent <->
// JSRDotNETSDK::EventArgsStatusChange
static StatusChangedEvent statusChangedEventFromManaged(EventArgsStatusChange ^
                                                        managedEvent) {
  StatusChangedEvent unmanagedEvent;
  StatusChangedEvent_Init(&unmanagedEvent);

  if (managedEvent == nullptr)
    return unmanagedEvent;

  JSRString_Set(&unmanagedEvent.PulserProperty,
      marshal_as<std::string>(managedEvent->PulserProperty).c_str());

  unmanagedEvent.PulserState =
      pulserReceiverStateFromManaged(managedEvent->PulserState);

  if (managedEvent->NewValue != nullptr)
    JSRString_Set(&unmanagedEvent.NewValue,
        marshal_as<std::string>(managedEvent->NewValue->ToString()).c_str());

  unmanagedEvent.DataType =
      pulserPropertyDataTypeFromManaged(managedEvent->DataType);
  unmanagedEvent.ChangeType = statusChangeFromManaged(managedEvent->ChangeType);

  if (managedEvent->PulserReceiverId != nullptr)
    unmanagedEvent.PulserReceiverId =
        pulsereceiverFromManaged(managedEvent->PulserReceiverId);

  if (!System::String::IsNullOrEmpty(managedEvent->ErrorMessage))
    JSRString_Set(&unmanagedEvent.ErrorMessage,
        marshal_as<std::string>(managedEvent->ErrorMessage).c_str());

  unmanagedEvent.ErrorCode = errorCodeFromManaged(managedEvent->ErrorCode);

  if (managedEvent->ThrownException != nullptr)
    unmanagedEvent.ThrownException =
        exceptionFromManaged(managedEvent->ThrownException);

  return unmanagedEvent;
}

// Converting JSR-SDK::NotifyEvent <-> JSRDotNETSDK::EventArgsManagerNotify
static NotifyEvent notifyEventFromManaged(EventArgsManagerNotify ^
                                          managedEvent) {
  NotifyEvent unmanagedEvent;
  NotifyEvent_Init(&unmanagedEvent);
  
  if (managedEvent == nullptr) {
    return unmanagedEvent;
  }

  if (!System::String::IsNullOrEmpty(managedEvent->Model))
    JSRString_Set(&unmanagedEvent.Model, 
                  marshal_as<std::string>(managedEvent->Model).c_str());

  unmanagedEvent.State = pulserReceiverStateFromManaged(managedEvent->State);

  if (managedEvent->NewValue != nullptr)
    JSRString_Set(&unmanagedEvent.NewValue,
        marshal_as<std::string>(managedEvent->NewValue->ToString()).c_str());

  if (!System::String::IsNullOrEmpty(managedEvent->PropertyName))
    JSRString_Set(&unmanagedEvent.PropertyName,
        marshal_as<std::string>(managedEvent->PropertyName).c_str());

  unmanagedEvent.WasSelected = managedEvent->WasSelected ? 1 : 0;

  if (!System::String::IsNullOrEmpty(managedEvent->ExceptionTypeInfo))
    JSRString_Set(&unmanagedEvent.ExceptionTypeInfo,
        marshal_as<std::string>(managedEvent->ExceptionTypeInfo).c_str());

  if (!System::String::IsNullOrEmpty(managedEvent->ErrorMsg))
    JSRString_Set(&unmanagedEvent.ErrorMsg,
                  marshal_as<std::string>(managedEvent->ErrorMsg).c_str());

  if (!System::String::IsNullOrEmpty(managedEvent->ErrorText))
    JSRString_Set(&unmanagedEvent.ErrorText,
                  marshal_as<std::string>(managedEvent->ErrorText).c_str());

  unmanagedEvent.MaxFrequency = managedEvent->MaxFrequency;

  if (managedEvent->Info != nullptr) {
    int idx = 0;
    for each (System::String^ info in managedEvent->Info) {
      if (idx >= JSR_NOTIFY_INFO_MAX_COUNT) break;
      JSRString_Set(&unmanagedEvent.Info[idx++],
                    marshal_as<std::string>(info).c_str());
    }
    unmanagedEvent.InfoCount = idx;
  }

  unmanagedEvent.PrIndex = managedEvent->PRIndex;

  if (managedEvent->PulserReceiverId != nullptr)
    unmanagedEvent.PulserReceiverId =
        pulsereceiverFromManaged(managedEvent->PulserReceiverId);

  if (!System::String::IsNullOrEmpty(managedEvent->Serial))
    JSRString_Set(&unmanagedEvent.Serial,
                  marshal_as<std::string>(managedEvent->Serial).c_str());

  unmanagedEvent.DiscoverState =
      discoveryStateFlagsFromManaged(managedEvent->DiscoverState);

  if (managedEvent->InstrumentId != nullptr)
    unmanagedEvent.InstrumentId =
        instrumentFromManaged(managedEvent->InstrumentId);

  if (!System::String::IsNullOrEmpty(managedEvent->PluginName))
    JSRString_Set(&unmanagedEvent.PluginName,
                  marshal_as<std::string>(managedEvent->PluginName).c_str());

  unmanagedEvent.NotifyType = notifyTypeFromManaged(managedEvent->NotifyType);
  unmanagedEvent.DataType =
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


