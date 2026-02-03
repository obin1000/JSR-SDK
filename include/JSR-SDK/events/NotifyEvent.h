#pragma once

#include "JSR-SDK/enums/C_DISCOVERY_STATE_FLAGS.h"
#include "JSR-SDK/enums/C_NOTIFY_TYPE.h"
#include "JSR-SDK/enums/C_PULSER_PROPERTY_DATA_TYPE.h"
#include "JSR-SDK/enums/C_PULSER_RECEIVER_STATE.h"
#include "JSR-SDK/enums/C_STATUS_CHANGE.h"
#include "JSR-SDK/structs/PulserReceiverID.h"
#include "JSR-SDK/structs/InstrumentID.h"
#include "JSR-SDK/boundary/CString.h"
#include "JSR-SDK/boundary/CVector.h"

#include <functional>

class NotifyEvent {
public:
  CString model;

  C_PULSER_RECEIVER_STATE state;

  CString newValue;

  CString propertyName;

  bool wasSelected;

  CString exceptionTypeInfo;

  CString errorMsg;

  CString errorText;

  double maxFrequency;

  CVector<CString> info;

  int prIndex;

  PulserReceiverID pulserReceiverId;
  CString serial;

  C_DISCOVERY_STATE_FLAGS discoverState;

  InstrumentID instrumentId;

  CString pluginName;

  C_NOTIFY_TYPE notifyType;

  C_PULSER_PROPERTY_DATA_TYPE dataType;

  NotifyEvent()
      : state{}, wasSelected(false), maxFrequency(0.0), prIndex(0),
        discoverState{}, notifyType{}, dataType{} {}

  ~NotifyEvent() {
    model.free_cstring();
    newValue.free_cstring();
    propertyName.free_cstring();
    exceptionTypeInfo.free_cstring();
    errorMsg.free_cstring();
    errorText.free_cstring();
    
    for (size_t i = 0; i < info.size; ++i) {
      info.data[i].free_cstring();
    }
    info.free_cvector();
    
    serial.free_cstring();
    pluginName.free_cstring();
  }

  NotifyEvent(const NotifyEvent &) = delete;
  NotifyEvent &operator=(const NotifyEvent &) = delete;

  NotifyEvent(NotifyEvent &&other) noexcept
      : model(other.model), state(other.state), newValue(other.newValue),
        propertyName(other.propertyName), wasSelected(other.wasSelected),
        exceptionTypeInfo(other.exceptionTypeInfo), errorMsg(other.errorMsg),
        errorText(other.errorText), maxFrequency(other.maxFrequency),
        info(other.info), prIndex(other.prIndex),
        pulserReceiverId(std::move(other.pulserReceiverId)),
        serial(other.serial), discoverState(other.discoverState),
        instrumentId(std::move(other.instrumentId)),
        pluginName(other.pluginName), notifyType(other.notifyType),
        dataType(other.dataType) {
    other.model = CString();
    other.newValue = CString();
    other.propertyName = CString();
    other.exceptionTypeInfo = CString();
    other.errorMsg = CString();
    other.errorText = CString();
    other.info = CVector<CString>();
    other.serial = CString();
    other.pluginName = CString();
  }

  NotifyEvent &operator=(NotifyEvent &&other) noexcept {
    if (this != &other) {
      model.free_cstring();
      newValue.free_cstring();
      propertyName.free_cstring();
      exceptionTypeInfo.free_cstring();
      errorMsg.free_cstring();
      errorText.free_cstring();
      
      for (size_t i = 0; i < info.size; ++i) {
        info.data[i].free_cstring();
      }
      info.free_cvector();
      
      serial.free_cstring();
      pluginName.free_cstring();

      model = other.model;
      state = other.state;
      newValue = other.newValue;
      propertyName = other.propertyName;
      wasSelected = other.wasSelected;
      exceptionTypeInfo = other.exceptionTypeInfo;
      errorMsg = other.errorMsg;
      errorText = other.errorText;
      maxFrequency = other.maxFrequency;
      info = other.info;
      prIndex = other.prIndex;
      pulserReceiverId = std::move(other.pulserReceiverId);
      serial = other.serial;
      discoverState = other.discoverState;
      instrumentId = std::move(other.instrumentId);
      pluginName = other.pluginName;
      notifyType = other.notifyType;
      dataType = other.dataType;

      other.model = CString();
      other.newValue = CString();
      other.propertyName = CString();
      other.exceptionTypeInfo = CString();
      other.errorMsg = CString();
      other.errorText = CString();
      other.info = CVector<CString>();
      other.serial = CString();
      other.pluginName = CString();
    }
    return *this;
  }
};

using NotifyCallback = std::function<void(const NotifyEvent &)>;
