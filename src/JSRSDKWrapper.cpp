#include "JSR-SDK/events/NotifyEvent.h"
#include "JSR-SDK/events/StatusChangedEvent.h"
#include "MarshalTypes.cpp"

using namespace JSRDotNETSDK;
using namespace System::Runtime::InteropServices;

/**
 * This wrapper holds the instance of JSRDotNETSDKManager and manages events and
 * callbacks.
 * This is the C# side of the wrapper.
 **/
public
ref class JSRSDKWrapper {
private:
  JSRDotNETManager ^ m_manager;

  StatusChangeCallback *m_nativeStatusCb = nullptr;
  NotifyCallback *m_nativeNotifyCb = nullptr;

  void OnStatusChangedEvent(Object ^ sender,
                            EventArgsStatusChange ^ eventData) {
    if (m_nativeStatusCb) {
      StatusChangedEvent nativeEvt = statusChangedEventFromManaged(eventData);
      (*m_nativeStatusCb)(nativeEvt);
    }
  }

  void OnNotifyEvent(Object ^ sender, EventArgsManagerNotify ^ eventData) {
    if (m_nativeNotifyCb) {
      NotifyEvent nativeEvt = notifyEventFromManaged(eventData);
      (*m_nativeNotifyCb)(nativeEvt);
    }
  }

public:
  JSRSDKWrapper() {
    // Create an instance of the JSRDotNETManager
    m_manager = gcnew JSRDotNETManager("");

    m_manager->StatusChangeEventHandler +=
        gcnew System::EventHandler<EventArgsStatusChange ^>(
            this, &JSRSDKWrapper::OnStatusChangedEvent);
    m_manager->NotifyEventHandler +=
        gcnew System::EventHandler<EventArgsManagerNotify ^>(
            this, &JSRSDKWrapper::OnNotifyEvent);
  }

  ~JSRSDKWrapper() {
    // Unsubscribe from events
    m_manager->StatusChangeEventHandler -=
        gcnew System::EventHandler<EventArgsStatusChange ^>(
            this, &JSRSDKWrapper::OnStatusChangedEvent);
    m_manager->NotifyEventHandler -=
        gcnew System::EventHandler<EventArgsManagerNotify ^>(
            this, &JSRSDKWrapper::OnNotifyEvent);
  }

  // Called from the unmanaged adapter
  void SetStatusChangeCallback(StatusChangeCallback *cb) {
    m_nativeStatusCb = cb;
  }

  void SetNotifyCallback(NotifyCallback *cb) { m_nativeNotifyCb = cb; }

  property JSRDotNETManager ^
      dotNETManager { JSRDotNETManager ^ get() { return m_manager; } }
};
