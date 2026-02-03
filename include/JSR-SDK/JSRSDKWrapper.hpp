#pragma once

#include "JSR-SDK/events/NotifyEvent.h"
#include "JSR-SDK/events/StatusChangedEvent.h"
#include "JSR-SDK/marshals/MarshalTypes.h"

namespace JSRDotNETSDK {
ref class JSRDotNETManager;
ref class EventArgsStatusChange;
ref class EventArgsManagerNotify;
} // namespace JSRDotNETSDK

/**
 * C++/CLI wrapper class that bridges between native C++ and managed C# code.
 * Provides event handling and callback management for JSRDotNETSDK.
 */
public
ref class JSRSDKWrapper {
private:
  JSRDotNETSDK::JSRDotNETManager ^ m_manager;
  StatusChangeCallback *m_nativeStatusCb;
  NotifyCallback *m_nativeNotifyCb;

  // Event handlers
  void OnStatusChangedEvent(System::Object ^ sender,
                            JSRDotNETSDK::EventArgsStatusChange ^ eventData);
  void OnNotifyEvent(System::Object ^ sender,
                     JSRDotNETSDK::EventArgsManagerNotify ^ eventData);

public:
  // Constructor
  JSRSDKWrapper();

  // Destructor
  ~JSRSDKWrapper();

  // Finalizer
  !JSRSDKWrapper();

  // Callback registration methods
  void SetStatusChangeCallback(StatusChangeCallback *cb);
  void SetNotifyCallback(NotifyCallback *cb);

  // Property to access the managed JSRDotNETManager instance
  property JSRDotNETSDK::JSRDotNETManager ^
      dotNETManager { JSRDotNETSDK::JSRDotNETManager ^ get(); }
};
