// CStringTest.cpp
#include "pch.h"
#include <gtest/gtest.h>

#include "JSR-SDK/boundary/CString.h"

//--------------------------------------------------------------------------
//  CString Basic Construction Tests
//--------------------------------------------------------------------------
TEST(CStringTests, DefaultConstruction_CreatesEmptyString) {
  CString str;
  
  EXPECT_EQ(str.data, nullptr);
  EXPECT_EQ(str.length, 0u);
}

TEST(CStringTests, DirectConstruction_StoresPointerAndLength) {
  const char* test_str = "Hello";
  CString str(test_str, 5);
  
  EXPECT_EQ(str.data, test_str);
  EXPECT_EQ(str.length, 5u);
}

//--------------------------------------------------------------------------
//  CString from_std_string Tests
//--------------------------------------------------------------------------
TEST(CStringTests, FromStdString_EmptyString_ReturnsDefault) {
  std::string empty;
  CString str = CString::from_std_string(empty);
  
  EXPECT_EQ(str.data, nullptr);
  EXPECT_EQ(str.length, 0u);
}

TEST(CStringTests, FromStdString_ValidString_AllocatesMemory) {
  std::string test = "Hello, World!";
  CString str = CString::from_std_string(test);
  
  ASSERT_NE(str.data, nullptr);
  EXPECT_EQ(str.length, test.length());
  EXPECT_STREQ(str.data, "Hello, World!");
  
  // Clean up
  str.free_cstring();
}

TEST(CStringTests, FromStdString_CopiesData) {
  std::string test = "Test String";
  CString str = CString::from_std_string(test);
  
  // Modify original string
  test = "Modified";
  
  // CString should still have original data
  ASSERT_NE(str.data, nullptr);
  EXPECT_STREQ(str.data, "Test String");
  
  // Clean up
  str.free_cstring();
}

TEST(CStringTests, FromStdString_NullTerminated) {
  std::string test = "ABC";
  CString str = CString::from_std_string(test);
  
  ASSERT_NE(str.data, nullptr);
  EXPECT_EQ(str.length, 3u);
  EXPECT_EQ(str.data[3], '\0');
  
  // Clean up
  str.free_cstring();
}

//--------------------------------------------------------------------------
//  CString to_std_string Tests
//--------------------------------------------------------------------------
TEST(CStringTests, ToStdString_EmptyString_ReturnsEmpty) {
  CString str;
  std::string result = str.to_std_string();
  
  EXPECT_TRUE(result.empty());
}

TEST(CStringTests, ToStdString_ValidString_CopiesData) {
  std::string original = "Convert Me";
  CString str = CString::from_std_string(original);
  
  std::string result = str.to_std_string();
  
  EXPECT_EQ(result, original);
  EXPECT_EQ(result.length(), str.length);
  
  // Clean up
  str.free_cstring();
}

TEST(CStringTests, ToStdString_RoundTrip_PreservesData) {
  std::string original = "Round Trip Test 123!@#";
  
  CString str = CString::from_std_string(original);
  std::string result = str.to_std_string();
  
  EXPECT_EQ(result, original);
  
  // Clean up
  str.free_cstring();
}

//--------------------------------------------------------------------------
//  CString Memory Management Tests
//--------------------------------------------------------------------------
TEST(CStringTests, FreeCString_ValidString_ClearsData) {
  CString str = CString::from_std_string("Free Me");
  
  ASSERT_NE(str.data, nullptr);
  ASSERT_GT(str.length, 0u);
  
  str.free_cstring();
  
  EXPECT_EQ(str.data, nullptr);
  EXPECT_EQ(str.length, 0u);
}

TEST(CStringTests, FreeCString_AlreadyEmpty_NoError) {
  CString str;
  
  // Should not crash
  str.free_cstring();
  
  EXPECT_EQ(str.data, nullptr);
  EXPECT_EQ(str.length, 0u);
}

TEST(CStringTests, FreeCString_MultipleCalls_Safe) {
  CString str = CString::from_std_string("Free Multiple Times");
  
  str.free_cstring();
  str.free_cstring();  // Second call should be safe
  
  EXPECT_EQ(str.data, nullptr);
  EXPECT_EQ(str.length, 0u);
}

TEST(CStringTests, FreeCStringFunction_FreesMemory) {
  CString str = CString::from_std_string("Free via function");
  
  ASSERT_NE(str.data, nullptr);
  
  free_cstring(str);
  
  EXPECT_EQ(str.data, nullptr);
  EXPECT_EQ(str.length, 0u);
}

//--------------------------------------------------------------------------
//  CString Copy Constructor Tests
//--------------------------------------------------------------------------
TEST(CStringTests, CopyConstruction_NotProvided_UsesShallowCopy) {
  // Note: CString intentionally does not provide a copy constructor
  // This test documents the expected behavior with default copy
  CString str1 = CString::from_std_string("Original");
  
  // Create a copy (shallow)
  CString str2 = str1;
  
  // Both should point to same data
  EXPECT_EQ(str1.data, str2.data);
  EXPECT_EQ(str1.length, str2.length);
  
  // Only free once to avoid double-free
  str1.free_cstring();
  
  // str2.data is now dangling - don't access it
}

//--------------------------------------------------------------------------
//  CString Special Character Tests
//--------------------------------------------------------------------------
TEST(CStringTests, FromStdString_WithNullBytes_HandlesCorrectly) {
  std::string test("AB\0CD", 5);  // String with embedded null
  CString str = CString::from_std_string(test);
  
  ASSERT_NE(str.data, nullptr);
  EXPECT_EQ(str.length, 5u);
  EXPECT_EQ(str.data[0], 'A');
  EXPECT_EQ(str.data[1], 'B');
  EXPECT_EQ(str.data[2], '\0');
  EXPECT_EQ(str.data[3], 'C');
  EXPECT_EQ(str.data[4], 'D');
  
  // Clean up
  str.free_cstring();
}

TEST(CStringTests, FromStdString_UnicodeString_CopiesBytes) {
  std::string test = "Hello ~#`'";
  CString str = CString::from_std_string(test);
  
  ASSERT_NE(str.data, nullptr);
  EXPECT_EQ(str.length, test.length());
  
  std::string result = str.to_std_string();
  EXPECT_EQ(result, test);
  
  // Clean up
  str.free_cstring();
}

//--------------------------------------------------------------------------
//  CString Edge Cases
//--------------------------------------------------------------------------
TEST(CStringTests, FromStdString_VeryLongString_HandlesCorrectly) {
  std::string test(10000, 'X');
  CString str = CString::from_std_string(test);
  
  ASSERT_NE(str.data, nullptr);
  EXPECT_EQ(str.length, 10000u);
  
  std::string result = str.to_std_string();
  EXPECT_EQ(result.length(), 10000u);
  EXPECT_EQ(result, test);
  
  // Clean up
  str.free_cstring();
}

TEST(CStringTests, ToStdString_NullDataNonZeroLength_ReturnsEmpty) {
  // Create invalid state (shouldn't happen in normal use)
  CString str;
  str.data = nullptr;
  str.length = 10;  // Invalid: length without data
  
  std::string result = str.to_std_string();
  
  EXPECT_TRUE(result.empty());
}

