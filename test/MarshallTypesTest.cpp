// MarshalTypesTest.cpp
#include "pch.h"
#include <gtest/gtest.h>

// Pull in the conversion helpers we want to test.
// NOTE: This file contains several `static`‑scope functions.  Including the
// .cpp directly makes them visible in this translation unit without changing
// the production code.
#include "../src/MarshalTypes.cpp"

using namespace System;
using namespace System::Collections::Generic;
using namespace JSRDotNETSDK;

//--------------------------------------------------------------------------
//  Managed *stub* implementations
//--------------------------------------------------------------------------
//  The production interfaces live in the C# SDK assembly.  Here we create
//  minimal C++/CLI stubs that satisfy the compiler so we can feed known data
//  through the conversion pipeline.  Only the members actually accessed by
//  MarshalTypes.cpp are implemented.
//--------------------------------------------------------------------------

// --- IInstrumentIdentity --------------------------------------------------
public
ref class TestInstrumentIdentity sealed : public IInstrumentIdentity {
private:
  String ^ modelName;
  String ^ serialNum;
  String ^ port;
  String ^ pluginName;

public:
  // Auto properties with explicit backing fields to keep older MSVC happy
  virtual property String ^
      ModelName {
        String ^ get() { return modelName; } void set(String ^ value) {
          modelName = value;
        }
      } virtual property String ^
      SerialNum {
        String ^ get() { return serialNum; } void set(String ^ value) {
          serialNum = value;
        }
      } virtual property String ^
      Port {
        String ^ get() { return port; } void set(String ^ value) {
          port = value;
        }
      } virtual property String ^
      PluginName {
        String ^ get() { return pluginName; } void set(String ^ value) {
          pluginName = value;
        }
      }

      // Equality helpers – not used by marshal code, so a very simple impl is
      // fine.
      virtual bool
      Equals(IInstrumentIdentity ^ other) {
    return ToString()->Equals(other->ToString());
  }
  virtual bool Equals(String ^ other) { return ToString()->Equals(other); }

  virtual String ^ GetDescription() { return ToString(); } virtual String ^
      ToString() override {
    return String::Format("{0} {1}", ModelName, SerialNum);
  }
};

// --- IPulserReceiverIdentity ---------------------------------------------
public
ref class TestPulserReceiverIdentity sealed : public IPulserReceiverIdentity {
private:
  IInstrumentIdentity ^ instrumentId;
  int prIndex;

public:
  virtual property IInstrumentIdentity ^ InstrumentId {
    IInstrumentIdentity ^
        get() { return instrumentId; } void set(IInstrumentIdentity ^ value) {
      instrumentId = value;
    }
  } virtual property int PulserReceiverIndex {
    int get() { return prIndex; }
    void set(int value) { prIndex = value; }
  }

  virtual bool Equals(IPulserReceiverIdentity ^ other) {
    return PulserReceiverIndex == other->PulserReceiverIndex;
  }
  virtual bool Equals(String ^ other) { return ToString()->Equals(other); }

  virtual String ^ GetDescription() { return ToString(); } virtual String ^
      ToString() override {
    return String::Format("PR {0} ({1})", PulserReceiverIndex,
                          InstrumentId == nullptr ? "none"
                                                  : InstrumentId->ToString());
  }
};

// --- IJSRDotNETLibMetadata -----------------------------------------------
public
ref class TestJSRLibMetadata sealed : public IJSRDotNETLibMetadata {
private:
  String ^ name;
  String ^ guid;
  String ^ friendlyName;
  cli::array<CONNECTION_TYPE> ^ connectionType;
  cli::array<String ^> ^ supportedModels;
  cli::array<String ^> ^ openOptions;
  int interfaceVersion;
  String ^ version;

public:
  virtual property String ^
      Name {
        String ^ get() { return name; } void set(String ^ value) {
          name = value;
        }
      } virtual property String ^
      GUID {
        String ^ get() { return guid; } void set(String ^ value) {
          guid = value;
        }
      } virtual property String ^
      FriendlyName {
        String ^ get() { return friendlyName; } void set(String ^ value) {
          friendlyName = value;
        }
      } virtual property cli::array<CONNECTION_TYPE> ^
      ConnectionType {
        cli::array<CONNECTION_TYPE> ^ get() {
          return connectionType;
        } void set(cli::array<CONNECTION_TYPE> ^ value) {
          connectionType = value;
        }
      } virtual property cli::array<String ^> ^
      SupportedModels {
        cli::array<String ^> ^ get() {
          return supportedModels;
        } void set(cli::array<String ^> ^ value) {
          supportedModels = value;
        }
      } virtual property cli::array<String ^> ^
      OpenOptions {
        cli::array<String ^> ^ get() {
          return openOptions;
        } void set(cli::array<String ^> ^ value) {
          openOptions = value;
        }
      } virtual property int InterfaceVersion {
    int get() { return interfaceVersion; }
    void set(int value) { interfaceVersion = value; }
  }
  virtual property String ^ Version {
    String ^ get() { return version; } void set(String ^ value) {
      version = value;
    }
  }
};

//--------------------------------------------------------------------------
//  listToVector & listToVectorMarshall tests
//--------------------------------------------------------------------------
TEST(MarshalTypesTests, ListToVector_Int_Success) {
  List<int> ^ managed = gcnew List<int>();
  managed->Add(1);
  managed->Add(2);

  std::vector<int> native = listToVector<int, int>(managed);
  ASSERT_EQ(native.size(), 2);
  EXPECT_EQ(native[0], 1);
  EXPECT_EQ(native[1], 2);
}

TEST(MarshalTypesTests, ListToVector_Null_ReturnsEmpty) {
  IEnumerable<int> ^ managed = nullptr;
  std::vector<int> native = listToVector<int, int>(managed);
  EXPECT_TRUE(native.empty());
}

TEST(MarshalTypesTests, ListToVectorMarshall_String_Success) {
  List<String ^> ^ managed = gcnew List<String ^>();
  managed->Add("one");
  managed->Add("two");

  std::vector<std::string> native = listToVectorMarshall < String ^,
                           std::string > (managed);
  ASSERT_EQ(native.size(), 2u);
  EXPECT_EQ(native[0], "one");
  EXPECT_EQ(native[1], "two");
}

//--------------------------------------------------------------------------
//  instrumentFromManaged tests
//--------------------------------------------------------------------------
TEST(MarshalTypesTests, InstrumentFromManaged_FullMapping) {
  TestInstrumentIdentity ^ managed = gcnew TestInstrumentIdentity();
  managed->ModelName = "JSR-PRM";
  managed->SerialNum = "12345";
  managed->Port = "USB";
  managed->PluginName = "TestPlugin";

  InstrumentID native = instrumentFromManaged(managed);

  EXPECT_EQ(native.ModelName, "JSR-PRM");
  EXPECT_EQ(native.SerialNum, "12345");
  EXPECT_EQ(native.Port, "USB");
  EXPECT_EQ(native.PluginName, "TestPlugin");
}

//--------------------------------------------------------------------------
//  pulsereceiverFromManaged tests
//--------------------------------------------------------------------------
TEST(MarshalTypesTests, PulserReceiverFromManaged_MapsFields) {
  TestInstrumentIdentity ^ inst = gcnew TestInstrumentIdentity();
  inst->ModelName = "Test-Model";
  inst->SerialNum = "SN9000";
  inst->Port = "COM3";
  inst->PluginName = "Mock";

  TestPulserReceiverIdentity ^ prManaged = gcnew TestPulserReceiverIdentity();
  prManaged->InstrumentId = inst;
  prManaged->PulserReceiverIndex = 3;

  PulserReceiverID native = pulsereceiverFromManaged(prManaged);

  EXPECT_EQ(native.PulserReceiverIndex, 3);
  EXPECT_EQ(native.InstrumentId.SerialNum, "SN9000");
}

//--------------------------------------------------------------------------
//  libMetadataFromManaged tests
//--------------------------------------------------------------------------
TEST(MarshalTypesTests, LibMetadataFromManaged_MapsCollections) {
  TestJSRLibMetadata ^ managed = gcnew TestJSRLibMetadata();
  managed->Name = "JSRLib";
  managed->GUID = "01234567-89AB-CDEF-0123-456789ABCDEF";
  managed->FriendlyName = "JSR Test Library";
  managed->Version = "2.1.0";
  managed->InterfaceVersion = 42;
  managed->SupportedModels = gcnew cli::array<String ^>{"M1", "M2"};
  managed->OpenOptions = gcnew cli::array<String ^>{"OptA"};
  managed->ConnectionType =
      gcnew cli::array<CONNECTION_TYPE>{CONNECTION_TYPE::USB};

  JSRLibMetadata native = libMetadataFromManaged(managed);

  EXPECT_EQ(native.Name, "JSRLib");
  EXPECT_EQ(native.GUID, "01234567-89AB-CDEF-0123-456789ABCDEF");
  EXPECT_EQ(native.FriendlyName, "JSR Test Library");
  EXPECT_EQ(native.Version, "2.1.0");
  EXPECT_EQ(native.InterfaceVersion, 42);
  ASSERT_EQ(native.SupportedModels.size(), 2u);
  EXPECT_EQ(native.SupportedModels[1], "M2");
  ASSERT_EQ(native.ConnectionType.size(), 1u);
}

//--------------------------------------------------------------------------
//  statusChangedEventFromManaged tests
//--------------------------------------------------------------------------
TEST(MarshalTypesTests, StatusChangedEventFromManaged_MapsStateChange) {
  // Arrange
  TestInstrumentIdentity ^ inst = gcnew TestInstrumentIdentity();
  inst->ModelName = "Model-A";
  inst->SerialNum = "SN001";
  TestPulserReceiverIdentity ^ prManaged = gcnew TestPulserReceiverIdentity();
  prManaged->InstrumentId = inst;
  prManaged->PulserReceiverIndex = 1;

  EventArgsStatusChange ^ managed =
      gcnew EventArgsStatusChange(prManaged, PulserReceiverState::READY);

  // Act
  StatusChangedEvent native = statusChangedEventFromManaged(managed);

  // Assert
  EXPECT_EQ(static_cast<int>(native.pulserState),
            static_cast<int>(PulserReceiverState::READY));
  EXPECT_EQ(native.pulserReceiverId.PulserReceiverIndex, 1);
}

//--------------------------------------------------------------------------
//  notifyEventFromManaged tests
//--------------------------------------------------------------------------
TEST(MarshalTypesTests, NotifyEventFromManaged_MapsPropertyChange) {
  // Arrange
  TestInstrumentIdentity ^ inst = gcnew TestInstrumentIdentity();
  inst->ModelName = "Model-B";
  inst->SerialNum = "SN002";
  TestPulserReceiverIdentity ^ prManaged = gcnew TestPulserReceiverIdentity();
  prManaged->InstrumentId = inst;
  prManaged->PulserReceiverIndex = 2;
  String ^ plugin = "MockPlugin";

  EventArgsManagerNotify ^ managed =
      EventArgsManagerNotify::CreatePropertyChangeEvent(
          plugin, prManaged, "Gain", 12, PulserPropertyDataType::INTEGER);

  // Act
  NotifyEvent native = notifyEventFromManaged(managed);

  // Assert
  EXPECT_EQ(native.propertyName, "Gain");
  EXPECT_EQ(native.newValue, "12");
  EXPECT_EQ(static_cast<int>(native.notifyType),
            static_cast<int>(NOTIFY_TYPE::PROPERTY_CHANGE));
  EXPECT_EQ(native.pulserReceiverId.PulserReceiverIndex, 2);
}


//--------------------------------------------------------------------------
//  main entry point (only if one is not already supplied by gtest).
//--------------------------------------------------------------------------
#ifndef RUN_ALL_TESTS_WITH_MAIN
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
#endif
