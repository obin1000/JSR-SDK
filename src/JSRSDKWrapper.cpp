#include "JSR-SDK/JSRSDKWrapper.hpp"

#include <msclr/lock.h>

using namespace JSRDotNETSDK;
using namespace System;
using namespace System::Runtime::InteropServices;

// Constructor
JSRSDKWrapper::JSRSDKWrapper()
    : m_nativeStatusCb(nullptr), m_nativeNotifyCb(nullptr) {
  m_statusLock = gcnew Object();
  m_notifyLock = gcnew Object();

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

// Event handler for status change events. The lock stays held during the
// call, so SetStatusChangeCallback() waits until the callback has returned.
void JSRSDKWrapper::OnStatusChangedEvent(Object ^ sender,
                                         EventArgsStatusChange ^ eventData) {
  msclr::lock lock(m_statusLock);
  if (m_nativeStatusCb == nullptr) {
    return;
  }
  StatusChangedEvent nativeEvt = statusChangedEventFromManaged(eventData);
  try {
    (*m_nativeStatusCb)(nativeEvt);
  } catch (...) {
    // Never let a callback's exception into the SDK, which raised the event
    // inside one of its own calls.
  }
}

// Event handler for notify events; see OnStatusChangedEvent.
void JSRSDKWrapper::OnNotifyEvent(Object ^ sender,
                                  EventArgsManagerNotify ^ eventData) {
  msclr::lock lock(m_notifyLock);
  if (m_nativeNotifyCb == nullptr) {
    return;
  }
  NotifyEvent nativeEvt = notifyEventFromManaged(eventData);
  try {
    (*m_nativeNotifyCb)(nativeEvt);
  } catch (...) {
    // See OnStatusChangedEvent.
  }
}

// Set the native status change callback
void JSRSDKWrapper::SetStatusChangeCallback(StatusChangeCallback *cb) {
  msclr::lock lock(m_statusLock);
  m_nativeStatusCb = cb;
}

// Set the native notify callback
void JSRSDKWrapper::SetNotifyCallback(NotifyCallback *cb) {
  msclr::lock lock(m_notifyLock);
  m_nativeNotifyCb = cb;
}

// Property getter for DotNETManager
JSRDotNETManager ^ JSRSDKWrapper::dotNETManager::get() {
  if (m_manager == nullptr) {
    throw std::runtime_error("C# dotNETManager was not correctly initialized");
  }
  return m_manager;
}
