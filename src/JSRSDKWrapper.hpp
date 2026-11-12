#pragma once

// Private to the DLL (C++/CLI only).
#include "JSR-SDK/events/NotifyEvent.h"
#include "JSR-SDK/events/StatusChangedEvent.h"

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

  // Held while a callback runs and while it is replaced, so that once
  // Set...Callback() returns, the previous callback is neither running nor
  // called again, and its owner may free it.
  System::Object ^ m_statusLock;
  System::Object ^ m_notifyLock;

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

  // Callback registration methods. They wait for a callback that is running
  // on another thread to return. Exceptions a callback throws are dropped:
  // the SDK raises events inside its own calls, so they would otherwise make
  // that call fail.
  void SetStatusChangeCallback(StatusChangeCallback *cb);
  void SetNotifyCallback(NotifyCallback *cb);

  // Property to access the managed JSRDotNETManager instance
  property JSRDotNETSDK::JSRDotNETManager ^ dotNETManager {
    JSRDotNETSDK::JSRDotNETManager ^ get();
  }
};
