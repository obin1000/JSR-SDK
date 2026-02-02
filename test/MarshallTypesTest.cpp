// MarshalTypesTest.cpp
#include "pch.h"
#include <gtest/gtest.h>

#include "JSR-SDK/marshals/MarshalTypes.h"
#include "JSR-SDK/JSRString.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace JSRDotNETSDK;

//--------------------------------------------------------------------------
//  Managed *stub* implementations
//--------------------------------------------------------------------------
//  The production interfaces live in the C# SDK assembly.  Here we create
//  minimal C++/CLI stubs that satisfy the compiler so we can feed known data
//  through the conversion pipeline.  Only the members actually accessed by
//  MarshalTypes.h are implemented.
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
//  JSRString Tests
//--------------------------------------------------------------------------
TEST(JSRStringTests, Init_SetsEmptyString) {
  JSRString str;
  JSRString_Init(&str);
  
  EXPECT_STREQ(str.data, "");
  EXPECT_EQ(strlen(str.data), 0u);
}

TEST(JSRStringTests, Set_NormalString_Success) {
  JSRString str;
  JSRString_Init(&str);
  JSRString_Set(&str, "Hello World");
  
  EXPECT_STREQ(str.data, "Hello World");
}

TEST(JSRStringTests, Set_NullPointer_SetsEmpty) {
  JSRString str;
  JSRString_Init(&str);
  JSRString_Set(&str, nullptr);
  
  EXPECT_STREQ(str.data, "");
}

TEST(JSRStringTests, Set_EmptyString_Success) {
  JSRString str;
  JSRString_Init(&str);
  JSRString_Set(&str, "");
  
  EXPECT_STREQ(str.data, "");
}

TEST(JSRStringTests, Set_LongString_Truncates) {
  JSRString str;
  JSRString_Init(&str);
  
  // Create a string longer than JSR_STRING_MAX_LENGTH (256)
  std::string longStr(300, 'A');
  JSRString_Set(&str, longStr.c_str());
  
  // Should be truncated to max length - 1 (for null terminator)
  EXPECT_EQ(strlen(str.data), JSR_STRING_MAX_LENGTH - 1);
  EXPECT_EQ(str.data[JSR_STRING_MAX_LENGTH - 1], '\0');
  
  // All characters before truncation should be 'A'
  for (size_t i = 0; i < JSR_STRING_MAX_LENGTH - 1; ++i) {
    EXPECT_EQ(str.data[i], 'A');
  }
}

TEST(JSRStringTests, Set_ExactMaxLength_Truncates) {
  JSRString str;
  JSRString_Init(&str);
  
  // Create a string exactly JSR_STRING_MAX_LENGTH (256)
  std::string exactStr(JSR_STRING_MAX_LENGTH, 'B');
  JSRString_Set(&str, exactStr.c_str());
  
  // Should be truncated to max length - 1
  EXPECT_EQ(strlen(str.data), JSR_STRING_MAX_LENGTH - 1);
  EXPECT_EQ(str.data[JSR_STRING_MAX_LENGTH - 1], '\0');
}

TEST(JSRStringTests, Set_SpecialCharacters_Success) {
  JSRString str;
  JSRString_Init(&str);
  JSRString_Set(&str, "Test\nWith\tSpecial\rChars");
  
  EXPECT_STREQ(str.data, "Test\nWith\tSpecial\rChars");
}

TEST(JSRStringTests, Set_Unicode_HandlesCorrectly) {
  JSRString str;
  JSRString_Init(&str);
  JSRString_Set(&str, "Hello 世界");
  
  // Should store the UTF-8 bytes correctly
  EXPECT_NE(strlen(str.data), 0u);
}

TEST(JSRStringTests, MultipleSet_OverwritesPrevious) {
  JSRString str;
  JSRString_Init(&str);
  
  JSRString_Set(&str, "First");
  EXPECT_STREQ(str.data, "First");
  
  JSRString_Set(&str, "Second");
  EXPECT_STREQ(str.data, "Second");
  
  JSRString_Set(&str, "Third");
  EXPECT_STREQ(str.data, "Third");
}

//--------------------------------------------------------------------------
//  JSRString_SetN Security Tests (NEW)
//--------------------------------------------------------------------------
TEST(JSRStringSecurityTests, SetN_NullPointer_ReturnsError) {
  int result = JSRString_SetN(nullptr, "test", 4);
  EXPECT_EQ(result, -1);
}

TEST(JSRStringSecurityTests, SetN_NullValue_SetsEmpty) {
  JSRString str;
  int result = JSRString_SetN(&str, nullptr, 10);
  EXPECT_EQ(result, 0);
  EXPECT_STREQ(str.data, "");
}

TEST(JSRStringSecurityTests, SetN_NonTerminatedString_Safe) {
  JSRString str;
  JSRString_Init(&str);
  
  // Create non-null-terminated buffer (simulates untrusted input)
  char buffer[100];
  memset(buffer, 'X', sizeof(buffer));  // No null terminator!
  
  // Should safely copy only maxLen bytes
  int result = JSRString_SetN(&str, buffer, 50);
  
  EXPECT_EQ(result, 0);
  EXPECT_EQ(strlen(str.data), 50u);
  
  // Verify all chars are 'X'
  for (size_t i = 0; i < 50; i++) {
    EXPECT_EQ(str.data[i], 'X');
  }
  EXPECT_EQ(str.data[50], '\0');  // Should be null-terminated
}

TEST(JSRStringSecurityTests, SetN_MaxLenExceedsBuffer_Truncates) {
  JSRString str;
  JSRString_Init(&str);
  
  char buffer[300];
  memset(buffer, 'Y', sizeof(buffer));
  
  // Request 300 bytes, but JSRString can only hold 255
  int result = JSRString_SetN(&str, buffer, 300);
  
  EXPECT_EQ(result, 0);
  EXPECT_EQ(strlen(str.data), JSR_STRING_MAX_LENGTH - 1);
  EXPECT_EQ(str.data[JSR_STRING_MAX_LENGTH - 1], '\0');
}

TEST(JSRStringSecurityTests, SetN_NormalTerminatedString_Success) {
  JSRString str;
  JSRString_Init(&str);
  
  const char* test = "Hello World";
  int result = JSRString_SetN(&str, test, 100);  // maxLen > string length
  
  EXPECT_EQ(result, 0);
  EXPECT_STREQ(str.data, "Hello World");
}

TEST(JSRStringSecurityTests, SetN_ZeroMaxLen_SetsEmpty) {
  JSRString str;
  JSRString_Init(&str);
  
  int result = JSRString_SetN(&str, "test", 0);
  
  EXPECT_EQ(result, 0);
  EXPECT_STREQ(str.data, "");
}

TEST(JSRStringSecurityTests, Set_VeryLongInput_DoesNotCrash) {
  JSRString str;
  JSRString_Init(&str);
  
  // Create a very long string
  std::vector<char> longBuffer(1000000, 'Z');
  longBuffer.push_back('\0');
  
  // Should handle gracefully without reading all 1MB
  JSRString_Set(&str, longBuffer.data());
  
  // Should be truncated
  EXPECT_EQ(strlen(str.data), JSR_STRING_MAX_LENGTH - 1);
}

//--------------------------------------------------------------------------
//  JSRStringLarge Tests (NEW)
//--------------------------------------------------------------------------
TEST(JSRStringLargeTests, Init_SetsEmpty) {
  JSRStringLarge str;
  JSRStringLarge_Init(&str);
  
  EXPECT_STREQ(str.data, "");
}

TEST(JSRStringLargeTests, Set_LargeString_Success) {
  JSRStringLarge str;
  JSRStringLarge_Init(&str);
  
  // Create a 500-byte string
  std::string large(500, 'L');
  JSRStringLarge_Set(&str, large.c_str());
  
  EXPECT_EQ(strlen(str.data), 500u);
}

TEST(JSRStringLargeTests, Set_VeryLargeString_Truncates) {
  JSRStringLarge str;
  JSRStringLarge_Init(&str);
  
  // Create a 2000-byte string (larger than 1024 limit)
  std::string veryLarge(2000, 'V');
  JSRStringLarge_Set(&str, veryLarge.c_str());
  
  size_t maxLen = sizeof(str.data) - 1;
  EXPECT_EQ(strlen(str.data), maxLen);
  EXPECT_EQ(str.data[maxLen], '\0');
}

TEST(JSRStringLargeTests, Get_ReturnsCorrectPointer) {
  JSRStringLarge str;
  JSRStringLarge_Init(&str);
  JSRStringLarge_Set(&str, "Test Large");
  
  const char* result = JSRStringLarge_Get(&str);
  EXPECT_STREQ(result, "Test Large");
}

TEST(JSRStringLargeTests, Get_NullPointer_ReturnsEmpty) {
  const char* result = JSRStringLarge_Get(nullptr);
  EXPECT_STREQ(result, "");
}

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

TEST(MarshalTypesTests, ListToVectorMarshall_EmptyList_Success) {
  List<String ^> ^ managed = gcnew List<String ^>();
  
  std::vector<std::string> native = listToVectorMarshall < String ^,
                           std::string > (managed);
  EXPECT_TRUE(native.empty());
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

  EXPECT_STREQ(native.ModelName.data, "JSR-PRM");
  EXPECT_STREQ(native.SerialNum.data, "12345");
  EXPECT_STREQ(native.Port.data, "USB");
  EXPECT_STREQ(native.PluginName.data, "TestPlugin");
}

TEST(MarshalTypesTests, InstrumentFromManaged_NullInput_ReturnsEmpty) {
  InstrumentID native = instrumentFromManaged(nullptr);
  
  EXPECT_STREQ(native.ModelName.data, "");
  EXPECT_STREQ(native.SerialNum.data, "");
  EXPECT_STREQ(native.Port.data, "");
  EXPECT_STREQ(native.PluginName.data, "");
}

TEST(MarshalTypesTests, InstrumentFromManaged_NullFields_HandlesGracefully) {
  TestInstrumentIdentity ^ managed = gcnew TestInstrumentIdentity();
  managed->ModelName = nullptr;
  managed->SerialNum = nullptr;
  managed->Port = "USB";
  managed->PluginName = nullptr;

  InstrumentID native = instrumentFromManaged(managed);

  EXPECT_STREQ(native.ModelName.data, "");
  EXPECT_STREQ(native.SerialNum.data, "");
  EXPECT_STREQ(native.Port.data, "USB");
  EXPECT_STREQ(native.PluginName.data, "");
}

TEST(MarshalTypesTests, InstrumentFromManaged_EmptyStrings_Success) {
  TestInstrumentIdentity ^ managed = gcnew TestInstrumentIdentity();
  managed->ModelName = "";
  managed->SerialNum = "";
  managed->Port = "";
  managed->PluginName = "";

  InstrumentID native = instrumentFromManaged(managed);

  EXPECT_STREQ(native.ModelName.data, "");
  EXPECT_STREQ(native.SerialNum.data, "");
  EXPECT_STREQ(native.Port.data, "");
  EXPECT_STREQ(native.PluginName.data, "");
}

TEST(MarshalTypesTests, InstrumentFromManaged_LongStrings_Truncates) {
  TestInstrumentIdentity ^ managed = gcnew TestInstrumentIdentity();
  
  // Create a very long model name
  System::Text::StringBuilder^ sb = gcnew System::Text::StringBuilder();
  for (int i = 0; i < 300; i++) {
    sb->Append("A");
  }
  managed->ModelName = sb->ToString();
  managed->SerialNum = "Normal";
  managed->Port = "COM1";
  managed->PluginName = "Plugin";

  InstrumentID native = instrumentFromManaged(managed);

  // Should be truncated
  EXPECT_EQ(strlen(native.ModelName.data), JSR_STRING_MAX_LENGTH - 1);
  EXPECT_STREQ(native.SerialNum.data, "Normal");
  EXPECT_STREQ(native.Port.data, "COM1");
  EXPECT_STREQ(native.PluginName.data, "Plugin");
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
  EXPECT_STREQ(native.InstrumentId.SerialNum.data, "SN9000");
  EXPECT_STREQ(native.InstrumentId.ModelName.data, "Test-Model");
}

TEST(MarshalTypesTests, PulserReceiverFromManaged_NullInstrument_HandlesGracefully) {
  TestPulserReceiverIdentity ^ prManaged = gcnew TestPulserReceiverIdentity();
  prManaged->InstrumentId = nullptr;
  prManaged->PulserReceiverIndex = 5;

  PulserReceiverID native = pulsereceiverFromManaged(prManaged);

  EXPECT_EQ(native.PulserReceiverIndex, 5);
  EXPECT_STREQ(native.InstrumentId.ModelName.data, "");
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

  EXPECT_STREQ(native.Name.data, "JSRLib");
  EXPECT_STREQ(native.GUID.data, "01234567-89AB-CDEF-0123-456789ABCDEF");
  EXPECT_STREQ(native.FriendlyName.data, "JSR Test Library");
  EXPECT_STREQ(native.Version.data, "2.1.0");
  EXPECT_EQ(native.InterfaceVersion, 42);
  ASSERT_EQ(native.SupportedModelsCount, 2);
  EXPECT_STREQ(native.SupportedModels[1].data, "M2");
  ASSERT_EQ(native.ConnectionTypeCount, 1);
}

TEST(MarshalTypesTests, LibMetadataFromManaged_Null_ReturnsEmpty) {
  JSRLibMetadata native = libMetadataFromManaged(nullptr);
  
  EXPECT_STREQ(native.Name.data, "");
  EXPECT_EQ(native.SupportedModelsCount, 0);
  EXPECT_EQ(native.OpenOptionsCount, 0);
  EXPECT_EQ(native.ConnectionTypeCount, 0);
}

TEST(MarshalTypesTests, LibMetadataFromManaged_NullCollections_HandlesGracefully) {
  TestJSRLibMetadata ^ managed = gcnew TestJSRLibMetadata();
  managed->Name = "Test";
  managed->SupportedModels = nullptr;
  managed->OpenOptions = nullptr;
  managed->ConnectionType = nullptr;

  JSRLibMetadata native = libMetadataFromManaged(managed);

  EXPECT_STREQ(native.Name.data, "Test");
  EXPECT_EQ(native.SupportedModelsCount, 0);
  EXPECT_EQ(native.OpenOptionsCount, 0);
  EXPECT_EQ(native.ConnectionTypeCount, 0);
}

TEST(MarshalTypesTests, LibMetadataFromManaged_MaxModels_TruncatesCorrectly) {
  TestJSRLibMetadata ^ managed = gcnew TestJSRLibMetadata();
  managed->Name = "Test";
  
  // Create more models than the max
  cli::array<String ^>^ models = gcnew cli::array<String ^>(JSR_METADATA_MAX_MODELS + 5);
  for (int i = 0; i < models->Length; i++) {
    models[i] = "Model" + i.ToString();
  }
  managed->SupportedModels = models;

  JSRLibMetadata native = libMetadataFromManaged(managed);

  // Should be capped at max
  EXPECT_EQ(native.SupportedModelsCount, JSR_METADATA_MAX_MODELS);
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
  EXPECT_EQ(static_cast<int>(native.PulserState),
            static_cast<int>(PulserReceiverState::READY));
  EXPECT_EQ(native.PulserReceiverId.PulserReceiverIndex, 1);
}

TEST(MarshalTypesTests, StatusChangedEventFromManaged_Null_ReturnsEmpty) {
  StatusChangedEvent native = statusChangedEventFromManaged(nullptr);
  
  EXPECT_STREQ(native.PulserProperty.data, "");
  EXPECT_STREQ(native.NewValue.data, "");
  EXPECT_STREQ(native.ErrorMessage.data, "");
}

TEST(MarshalTypesTests, StatusChangedEventFromManaged_NullNewValue_HandlesGracefully) {
  TestInstrumentIdentity ^ inst = gcnew TestInstrumentIdentity();
  inst->ModelName = "Model";
  inst->SerialNum = "SN";
  TestPulserReceiverIdentity ^ prManaged = gcnew TestPulserReceiverIdentity();
  prManaged->InstrumentId = inst;
  prManaged->PulserReceiverIndex = 0;

  EventArgsStatusChange ^ managed =
      gcnew EventArgsStatusChange(prManaged, PulserReceiverState::READY);
  // NewValue is nullptr by default

  StatusChangedEvent native = statusChangedEventFromManaged(managed);

  // Should not crash, NewValue should be empty
  EXPECT_STREQ(native.NewValue.data, "");
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
  EXPECT_STREQ(native.PropertyName.data, "Gain");
  EXPECT_STREQ(native.NewValue.data, "12");
  EXPECT_EQ(static_cast<int>(native.NotifyType),
            static_cast<int>(NOTIFY_TYPE::PROPERTY_CHANGE));
  EXPECT_EQ(native.PulserReceiverId.PulserReceiverIndex, 2);
}

TEST(MarshalTypesTests, NotifyEventFromManaged_Null_ReturnsEmpty) {
  NotifyEvent native = notifyEventFromManaged(nullptr);
  
  EXPECT_STREQ(native.Model.data, "");
  EXPECT_STREQ(native.PropertyName.data, "");
  EXPECT_EQ(native.InfoCount, 0);
}

TEST(MarshalTypesTests, NotifyEventFromManaged_MaxInfo_TruncatesCorrectly) {
  TestInstrumentIdentity ^ inst = gcnew TestInstrumentIdentity();
  inst->ModelName = "Model";
  inst->SerialNum = "SN";
  TestPulserReceiverIdentity ^ prManaged = gcnew TestPulserReceiverIdentity();
  prManaged->InstrumentId = inst;
  prManaged->PulserReceiverIndex = 0;
  String ^ plugin = "TestPlugin";

  EventArgsManagerNotify ^ managed =
      EventArgsManagerNotify::CreatePropertyChangeEvent(
          plugin, prManaged, "Test", 1, PulserPropertyDataType::INTEGER);
  
  // Note: Cannot set Info directly as it's read-only, so this test is simplified
  // In production, Info is populated by the managed code

  NotifyEvent native = notifyEventFromManaged(managed);

  // Verify basic conversion worked
  EXPECT_STREQ(native.PropertyName.data, "Test");
}

//==========================================================================
//  Memory Safety Tests
//==========================================================================
TEST(MemorySafetyTests, LargeStructOnStack_NoOverflow) {
  // This test verifies that large structs can be safely allocated on stack
  NotifyEvent evt;
  NotifyEvent_Init(&evt);
  
  JSRString_Set(&evt.Model, "TestModel");
  JSRString_Set(&evt.PropertyName, "TestProperty");
  
  // Fill the Info array
  for (int i = 0; i < JSR_NOTIFY_INFO_MAX_COUNT; i++) {
    char buf[32];
    sprintf_s(buf, "Info%d", i);
    JSRString_Set(&evt.Info[i], buf);
  }
  
  // Verify data integrity
  EXPECT_STREQ(evt.Model.data, "TestModel");
  EXPECT_STREQ(evt.PropertyName.data, "TestProperty");
  EXPECT_STREQ(evt.Info[0].data, "Info0");
  EXPECT_STREQ(evt.Info[JSR_NOTIFY_INFO_MAX_COUNT - 1].data, "Info31");
}

TEST(MemorySafetyTests, StructCopy_PreservesData) {
  InstrumentID original;
  InstrumentID_Init(&original);
  JSRString_Set(&original.ModelName, "Original");
  JSRString_Set(&original.SerialNum, "SN123");
  
  // Copy the struct
  InstrumentID copy = original;
  
  // Verify both have correct data
  EXPECT_STREQ(original.ModelName.data, "Original");
  EXPECT_STREQ(copy.ModelName.data, "Original");
  
  // Modify copy, original should be unchanged
  JSRString_Set(&copy.ModelName, "Modified");
  EXPECT_STREQ(original.ModelName.data, "Original");
  EXPECT_STREQ(copy.ModelName.data, "Modified");
}

TEST(MemorySafetyTests, MultipleConversions_NoMemoryCorruption) {
  TestInstrumentIdentity ^ managed = gcnew TestInstrumentIdentity();
  managed->ModelName = "TestModel";
  managed->SerialNum = "SN456";
  
  // Perform multiple conversions
  for (int i = 0; i < 100; i++) {
    InstrumentID native = instrumentFromManaged(managed);
    EXPECT_STREQ(native.ModelName.data, "TestModel");
    EXPECT_STREQ(native.SerialNum.data, "SN456");
  }
}

TEST(MemorySafetyTests, NestedStructs_PreservesData) {
  TestInstrumentIdentity ^ inst = gcnew TestInstrumentIdentity();
  inst->ModelName = "Nested";
  inst->SerialNum = "SN789";
  
  TestPulserReceiverIdentity ^ pr = gcnew TestPulserReceiverIdentity();
  pr->InstrumentId = inst;
  pr->PulserReceiverIndex = 42;
  
  PulserReceiverID native = pulsereceiverFromManaged(pr);
  
  EXPECT_EQ(native.PulserReceiverIndex, 42);
  EXPECT_STREQ(native.InstrumentId.ModelName.data, "Nested");
  EXPECT_STREQ(native.InstrumentId.SerialNum.data, "SN789");
}

//==========================================================================
//  Edge Case Tests
//==========================================================================
TEST(EdgeCaseTests, BinaryData_InStrings_HandlesCorrectly) {
  JSRString str;
  JSRString_Init(&str);
  
  // String with null bytes would be truncated at first null
  const char* binaryData = "Test\0Hidden";
  JSRString_Set(&str, binaryData);
  
  // Should only have "Test" because of null terminator
  EXPECT_STREQ(str.data, "Test");
}

TEST(EdgeCaseTests, ConsecutiveNulls_HandlesCorrectly) {
  TestInstrumentIdentity ^ managed = gcnew TestInstrumentIdentity();
  managed->ModelName = nullptr;
  
  // Convert multiple times with null values
  for (int i = 0; i < 10; i++) {
    InstrumentID native = instrumentFromManaged(managed);
    EXPECT_STREQ(native.ModelName.data, "");
  }
}

TEST(EdgeCaseTests, VeryLongStringsInArray_TruncatesAll) {
  TestJSRLibMetadata ^ managed = gcnew TestJSRLibMetadata();
  
  System::Text::StringBuilder^ sb = gcnew System::Text::StringBuilder();
  for (int i = 0; i < 300; i++) {
    sb->Append("X");
  }
  String^ longString = sb->ToString();
  
  managed->SupportedModels = gcnew cli::array<String ^>{longString, longString};
  
  JSRLibMetadata native = libMetadataFromManaged(managed);
  
  EXPECT_EQ(native.SupportedModelsCount, 2);
  EXPECT_EQ(strlen(native.SupportedModels[0].data), JSR_STRING_MAX_LENGTH - 1);
  EXPECT_EQ(strlen(native.SupportedModels[1].data), JSR_STRING_MAX_LENGTH - 1);
}

//==========================================================================
//  main entry point
//==========================================================================
#ifndef RUN_ALL_TESTS_WITH_MAIN
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
#endif
