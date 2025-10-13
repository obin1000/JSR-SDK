#include "JSR-SDK/JSRSDKWrapper.h"

using namespace JSRDotNETSDK;
using namespace System;
using namespace System::Runtime::InteropServices;

// Constructor
JSRSDKWrapper::JSRSDKWrapper()
    : m_nativeStatusCb(nullptr), m_nativeNotifyCb(nullptr) {
  // Create an instance of the JSRDotNETManager
  m_manager = gcnew JSRDotNETManager("");

  // Subscribe to events
  m_manager->StatusChangeEventHandler +=
      gcnew EventHandler<EventArgsStatusChange ^>(
          this, &JSRSDKWrapper::OnStatusChangedEvent);

  m_manager->NotifyEventHandler += gcnew EventHandler<EventArgsManagerNotify ^>(
      this, &JSRSDKWrapper::OnNotifyEvent);
}

// Destructor
JSRSDKWrapper::~JSRSDKWrapper() {
  if (m_manager != nullptr) {
    // Unsubscribe from events
    m_manager->StatusChangeEventHandler -=
        gcnew EventHandler<EventArgsStatusChange ^>(
            this, &JSRSDKWrapper::OnStatusChangedEvent);

    m_manager->NotifyEventHandler -=
        gcnew EventHandler<EventArgsManagerNotify ^>(
            this, &JSRSDKWrapper::OnNotifyEvent);
    m_manager->Shutdown();
  }

  // Call finalizer to clean up unmanaged resources
  this->!JSRSDKWrapper();
}

// Finalizer
JSRSDKWrapper::!JSRSDKWrapper() {
  // Clean up unmanaged resources
  m_nativeStatusCb = nullptr;
  m_nativeNotifyCb = nullptr;
}

// Event handler for status change events
void JSRSDKWrapper::OnStatusChangedEvent(Object ^ sender,
                                         EventArgsStatusChange ^ eventData) {
  if (m_nativeStatusCb != nullptr) {
    StatusChangedEvent nativeEvt;
    // Marshal managed event data to native if needed
    // TODO: Populate nativeEvt from eventData
    (*m_nativeStatusCb)(nativeEvt);
  }
}

// Event handler for notify events
void JSRSDKWrapper::OnNotifyEvent(Object ^ sender,
                                  EventArgsManagerNotify ^ eventData) {
  if (m_nativeNotifyCb != nullptr) {
    NotifyEvent nativeEvt = notifyEventFromManaged(eventData);
    (*m_nativeNotifyCb)(nativeEvt);
  }
}

// Set the native status change callback
void JSRSDKWrapper::SetStatusChangeCallback(StatusChangeCallback *cb) {
  m_nativeStatusCb = cb;
}

// Set the native notify callback
void JSRSDKWrapper::SetNotifyCallback(NotifyCallback *cb) {
  m_nativeNotifyCb = cb;
}

// Property getter for DotNETManager
JSRDotNETManager ^ JSRSDKWrapper::dotNETManager::get() {
  if (m_manager == nullptr) {
    throw std::runtime_error("C# dotNETManager was not correctly initialized");
  }
  return m_manager;
}
