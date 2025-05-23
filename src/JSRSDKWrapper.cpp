#include "JSR-SDK/events/NotifyEvent.h"
#include "JSR-SDK/events/StatusChangedEvent.h"
#include "MarshalTypes.cpp"

using namespace JSRDotNETSDK;

/**
 * This wrapper holds the instance of JSRDotNETSDKManager and manages events and
 * callbacks.
 * This is the C# side of the wrapper.
 **/
public
ref class JSRSDKWrapper {
private:
  JSRDotNETManager ^ m_manager;

  StatusChangeCallback *statusCallback = nullptr;
  NotifyCallback *notifyCallback = nullptr;

  void OnStatusChangedEvent(Object ^ sender,
                            EventArgsStatusChange ^ eventData) {
    // Call the callback if it is set
    if (statusCallback != nullptr) {
      // Convert the managed event data to unmanaged data
      StatusChangedEvent unmanagedEvent =
          statusChangedEventFromManaged(eventData);
      (*statusCallback)(unmanagedEvent);
    }
  }

  void OnNotifyEvent(Object ^ sender, EventArgsManagerNotify ^ eventData) {
    // Call the callback if it is set
    if (notifyCallback != nullptr) {
      // Convert the managed event data to unmanaged data
      NotifyEvent unmanagedEvent = notifyEventFromManaged(eventData);
      (*notifyCallback)(unmanagedEvent);
    }
  }

public:
  JSRSDKWrapper() {
    // Create an instance of the JSRDotNETManager
    m_manager = gcnew JSRDotNETManager("");

    // Register the C# event handlers
    m_manager->StatusChangeEventHandler +=
        gcnew System::EventHandler<EventArgsStatusChange ^>(
            this, &JSRSDKWrapper::OnStatusChangedEvent);
    m_manager->NotifyEventHandler +=
        gcnew System::EventHandler<EventArgsManagerNotify ^>(
            this, &JSRSDKWrapper::OnNotifyEvent);
  }

  // Method to set the status change callback
  void SetStatusChangeCallback(StatusChangeCallback *callback) {
    statusCallback = callback;
  }

  // Method to set the notify callback
  void SetNotifyCallback(NotifyCallback *callback) {
    notifyCallback = callback;
  }

  property JSRDotNETManager ^
      dotNETManager { JSRDotNETManager ^ get() { return m_manager; } }
};
