

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

  void OnStatusChangedEvent(Object ^ sender, EventArgsStatusChange ^ eventData) {
    System::Console::WriteLine("Status changed: {0}", eventData->ToString());
  }

  void OnJsrManagerNotify(Object ^ sender, EventArgsManagerNotify ^ eventData) {
    System::Console::WriteLine("Notified: {0}", eventData->ToString());

  }

public:
  JSRSDKWrapper() { 
      // Create an instance of the JSRDotNETManager
      m_manager = gcnew JSRDotNETManager("");
      // Create the C# event receivers
      // TODO: Only register these when the C++ side has a handler for them
      m_manager->StatusChangeEventHandler +=
          gcnew System::EventHandler<EventArgsStatusChange ^>(
              this, &JSRSDKWrapper::OnStatusChangedEvent);
      m_manager->NotifyEventHandler +=
          gcnew System::EventHandler<EventArgsManagerNotify ^>(
              this, &JSRSDKWrapper::OnJsrManagerNotify);

  }

  property JSRDotNETManager ^
      dotNETManager { JSRDotNETManager ^ get() { return m_manager; } }
};
