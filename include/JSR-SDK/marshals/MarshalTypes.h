#pragma once

#include "MarshalEnums.h"
#include "JSR-SDK/structs/ExceptionJSRSDK.h"
#include "JSR-SDK/structs/InstrumentID.h"
#include "JSR-SDK/structs/JSRLibMetadata.h"
#include "JSR-SDK/structs/PulserReceiverID.h"
#include "JSR-SDK/events/NotifyEvent.h"
#include "JSR-SDK/events/StatusChangedEvent.h"

#include <msclr/marshal_cppstd.h>
#include <cstring>

using namespace JSRDotNETSDK;
using namespace System::Collections::Generic;
using namespace msclr::interop;

/**
 * @file MarshalTypes.h
 * @brief This file contains the conversion functions between JSR-SDK types and
 * JSRDotNETSDK types.
 * These can't be exposed through the headers as it contains references to
 * managed objects.
 **/

/**
 * @brief Helper to safely copy a managed string to a fixed-size buffer.
 */
static void copyManagedString(System::String ^ src, char *dest, size_t destSize) {
  if (System::String::IsNullOrEmpty(src) || dest == nullptr || destSize == 0) {
    if (dest != nullptr && destSize > 0) {
      dest[0] = '\0';
    }
    return;
  }
  
  std::string str = marshal_as<std::string>(src);
  size_t len = str.length();
  if (len >= destSize) {
    len = destSize - 1;
  }
  std::memcpy(dest, str.c_str(), len);
  dest[len] = '\0';
}

/**
 * @brief Helper to copy std::string to caller-provided buffer.
 * @return The required size (including null terminator)
 */
static size_t copyStringToBuffer(const std::string& src, char *buffer, size_t bufferSize) {
  if (buffer == nullptr || bufferSize == 0) {
    return src.size() + 1;
  }
  
  size_t len = src.length();
  if (len >= bufferSize) {
    len = bufferSize - 1;
  }
  std::memcpy(buffer, src.c_str(), len);
  buffer[len] = '\0';
  
  return src.size() + 1;
}

/**
 * @brief Helper to copy managed string to caller-provided buffer.
 * @return The required size (including null terminator)
 */
static size_t copyManagedStringToBuffer(System::String^ src, char *buffer, size_t bufferSize) {
  if (System::String::IsNullOrEmpty(src)) {
    if (buffer != nullptr && bufferSize > 0) {
      buffer[0] = '\0';
    }
    return 1; // Just null terminator
  }
  
  std::string str = marshal_as<std::string>(src);
  return copyStringToBuffer(str, buffer, bufferSize);
}

/**
 * @brief Helper to copy managed string list to caller buffer.
 * @return The actual number of strings
 */
static size_t copyStringList(System::Collections::Generic::IEnumerable<System::String^>^ list,
                             char **buffer, size_t bufferCount, size_t stringLength) {
  if (list == nullptr) return 0;
  
  size_t count = 0;
  for each (System::String^ s in list) count++;
  
  if (buffer == nullptr || bufferCount == 0) {
    return count;
  }
  
  size_t index = 0;
  for each (System::String^ s in list) {
    if (index >= bufferCount) break;
    copyManagedString(s, buffer[index], stringLength);
    index++;
  }
  
  return count;
}

/**
 * @brief Helper to copy double array to caller buffer.
 * @return The actual number of values
 */
static size_t copyDoubleArray(cli::array<System::Double>^ src, double *buffer, size_t bufferCount) {
  if (src == nullptr) return 0;
  
  size_t count = static_cast<size_t>(src->Length);
  
  if (buffer == nullptr || bufferCount == 0) {
    return count;
  }
  
  size_t copyCount = (count < bufferCount) ? count : bufferCount;
  for (size_t i = 0; i < copyCount; ++i) {
    buffer[i] = src[static_cast<int>(i)];
  }
  
  return count;
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

  copyManagedString(instrumentIdentity->ModelName, unmanaged.ModelName, InstrumentID::MAX_STRING_LENGTH);
  copyManagedString(instrumentIdentity->PluginName, unmanaged.PluginName, InstrumentID::MAX_STRING_LENGTH);
  copyManagedString(instrumentIdentity->Port, unmanaged.Port, InstrumentID::MAX_STRING_LENGTH);
  copyManagedString(instrumentIdentity->SerialNum, unmanaged.SerialNum, InstrumentID::MAX_STRING_LENGTH);

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

  copyManagedString(metadata->Name, unmanaged.Name, JSRLibMetadata::MAX_STRING_LENGTH);
  copyManagedString(metadata->GUID, unmanaged.GUID, JSRLibMetadata::MAX_STRING_LENGTH);
  copyManagedString(metadata->FriendlyName, unmanaged.FriendlyName, JSRLibMetadata::MAX_STRING_LENGTH);
  copyManagedString(metadata->Version, unmanaged.Version, JSRLibMetadata::MAX_STRING_LENGTH);

  unmanaged.InterfaceVersion = metadata->InterfaceVersion;

  // Copy SupportedModels
  if (metadata->SupportedModels != nullptr) {
    int index = 0;
    for each (System::String^ model in metadata->SupportedModels) {
      if (index >= JSRLibMetadata::MAX_ARRAY_SIZE) break;
      copyManagedString(model, unmanaged.SupportedModels[index], JSRLibMetadata::MAX_STRING_LENGTH);
      index++;
    }
    unmanaged.SupportedModelsCount = index;
  }

  // Copy OpenOptions
  if (metadata->OpenOptions != nullptr) {
    int index = 0;
    for each (System::String^ option in metadata->OpenOptions) {
      if (index >= JSRLibMetadata::MAX_ARRAY_SIZE) break;
      copyManagedString(option, unmanaged.OpenOptions[index], JSRLibMetadata::MAX_STRING_LENGTH);
      index++;
    }
    unmanaged.OpenOptionsCount = index;
  }

  // Copy ConnectionType
  if (metadata->ConnectionType != nullptr) {
    int index = 0;
    for each (CONNECTION_TYPE ct in metadata->ConnectionType) {
      if (index >= JSRLibMetadata::MAX_ARRAY_SIZE) break;
      unmanaged.ConnectionType[index] = connectionTypeFromManaged(ct);
      index++;
    }
    unmanaged.ConnectionTypeCount = index;
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

  copyManagedString(managedEvent->PulserProperty, unmanagedEvent.pulserProperty, StatusChangedEvent::MAX_STRING_LENGTH);

  unmanagedEvent.pulserState =
      pulserReceiverStateFromManaged(managedEvent->PulserState);

  if (managedEvent->NewValue != nullptr) {
    copyManagedString(managedEvent->NewValue->ToString(), unmanagedEvent.newValue, StatusChangedEvent::MAX_STRING_LENGTH);
  }

  unmanagedEvent.dataType =
      pulserPropertyDataTypeFromManaged(managedEvent->DataType);
  unmanagedEvent.changeType = statusChangeFromManaged(managedEvent->ChangeType);

  if (managedEvent->PulserReceiverId != nullptr)
    unmanagedEvent.pulserReceiverId =
        pulsereceiverFromManaged(managedEvent->PulserReceiverId);

  copyManagedString(managedEvent->ErrorMessage, unmanagedEvent.errorMessage, StatusChangedEvent::MAX_STRING_LENGTH);

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

  copyManagedString(managedEvent->Model, unmanagedEvent.model, NotifyEvent::MAX_STRING_LENGTH);

  unmanagedEvent.state = pulserReceiverStateFromManaged(managedEvent->State);

  if (managedEvent->NewValue != nullptr) {
    copyManagedString(managedEvent->NewValue->ToString(), unmanagedEvent.newValue, NotifyEvent::MAX_STRING_LENGTH);
  }

  copyManagedString(managedEvent->PropertyName, unmanagedEvent.propertyName, NotifyEvent::MAX_STRING_LENGTH);

  unmanagedEvent.wasSelected = managedEvent->WasSelected;

  copyManagedString(managedEvent->ExceptionTypeInfo, unmanagedEvent.exceptionTypeInfo, NotifyEvent::MAX_STRING_LENGTH);
  copyManagedString(managedEvent->ErrorMsg, unmanagedEvent.errorMsg, NotifyEvent::MAX_STRING_LENGTH);
  copyManagedString(managedEvent->ErrorText, unmanagedEvent.errorText, NotifyEvent::MAX_STRING_LENGTH);

  unmanagedEvent.maxFrequency = managedEvent->MaxFrequency;

  // Copy Info array
  if (managedEvent->Info != nullptr) {
    int index = 0;
    for each (System::String^ infoStr in managedEvent->Info) {
      if (index >= NotifyEvent::MAX_INFO_ARRAY) break;
      copyManagedString(infoStr, unmanagedEvent.info[index], NotifyEvent::MAX_STRING_LENGTH);
      index++;
    }
    unmanagedEvent.infoCount = index;
  }

  unmanagedEvent.prIndex = managedEvent->PRIndex;

  if (managedEvent->PulserReceiverId != nullptr)
    unmanagedEvent.pulserReceiverId =
        pulsereceiverFromManaged(managedEvent->PulserReceiverId);

  copyManagedString(managedEvent->Serial, unmanagedEvent.serial, NotifyEvent::MAX_STRING_LENGTH);

  unmanagedEvent.discoverState =
      discoveryStateFlagsFromManaged(managedEvent->DiscoverState);

  if (managedEvent->InstrumentId != nullptr)
    unmanagedEvent.instrumentId =
        instrumentFromManaged(managedEvent->InstrumentId);

  copyManagedString(managedEvent->PluginName, unmanagedEvent.pluginName, NotifyEvent::MAX_STRING_LENGTH);

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


