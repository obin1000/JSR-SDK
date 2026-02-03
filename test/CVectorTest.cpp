// CVectorTest.cpp
#include "pch.h"
#include <gtest/gtest.h>

#include "JSR-SDK/boundary/CVector.h"
#include "JSR-SDK/boundary/CString.h"

//--------------------------------------------------------------------------
//  CVector Basic Construction Tests
//--------------------------------------------------------------------------
TEST(CVectorTests, DefaultConstruction_CreatesEmptyVector) {
  CVector<int> vec;
  
  EXPECT_EQ(vec.data, nullptr);
  EXPECT_EQ(vec.size, 0u);
  EXPECT_EQ(vec.capacity, 0u);
}

TEST(CVectorTests, DirectConstruction_StoresPointerSizeCapacity) {
  int data[] = {1, 2, 3};
  CVector<int> vec(data, 3, 3);
  
  EXPECT_EQ(vec.data, data);
  EXPECT_EQ(vec.size, 3u);
  EXPECT_EQ(vec.capacity, 3u);
}

//--------------------------------------------------------------------------
//  CVector from_std_vector Tests
//--------------------------------------------------------------------------
TEST(CVectorTests, FromStdVector_EmptyVector_ReturnsDefault) {
  std::vector<int> empty;
  CVector<int> vec = CVector<int>::from_std_vector(empty);
  
  EXPECT_EQ(vec.data, nullptr);
  EXPECT_EQ(vec.size, 0u);
  EXPECT_EQ(vec.capacity, 0u);
}

TEST(CVectorTests, FromStdVector_IntVector_AllocatesMemory) {
  std::vector<int> test = {1, 2, 3, 4, 5};
  CVector<int> vec = CVector<int>::from_std_vector(test);
  
  ASSERT_NE(vec.data, nullptr);
  EXPECT_EQ(vec.size, 5u);
  EXPECT_EQ(vec.capacity, 5u);
  
  for (size_t i = 0; i < 5; ++i) {
    EXPECT_EQ(vec.data[i], test[i]);
  }
  
  // Clean up
  vec.free_cvector();
}

TEST(CVectorTests, FromStdVector_DoubleVector_CopiesData) {
  std::vector<double> test = {1.1, 2.2, 3.3};
  CVector<double> vec = CVector<double>::from_std_vector(test);
  
  ASSERT_NE(vec.data, nullptr);
  EXPECT_EQ(vec.size, 3u);
  
  for (size_t i = 0; i < 3; ++i) {
    EXPECT_DOUBLE_EQ(vec.data[i], test[i]);
  }
  
  // Clean up
  vec.free_cvector();
}

TEST(CVectorTests, FromStdVector_CopiesIndependently) {
  std::vector<int> test = {10, 20, 30};
  CVector<int> vec = CVector<int>::from_std_vector(test);
  
  // Modify original vector
  test[0] = 999;
  
  // CVector should still have original data
  ASSERT_NE(vec.data, nullptr);
  EXPECT_EQ(vec.data[0], 10);
  
  // Clean up
  vec.free_cvector();
}

//--------------------------------------------------------------------------
//  CVector to_std_vector Tests
//--------------------------------------------------------------------------
TEST(CVectorTests, ToStdVector_EmptyVector_ReturnsEmpty) {
  CVector<int> vec;
  std::vector<int> result = vec.to_std_vector();
  
  EXPECT_TRUE(result.empty());
}

TEST(CVectorTests, ToStdVector_ValidVector_CopiesData) {
  std::vector<int> original = {100, 200, 300, 400};
  CVector<int> vec = CVector<int>::from_std_vector(original);
  
  std::vector<int> result = vec.to_std_vector();
  
  ASSERT_EQ(result.size(), original.size());
  for (size_t i = 0; i < original.size(); ++i) {
    EXPECT_EQ(result[i], original[i]);
  }
  
  // Clean up
  vec.free_cvector();
}

TEST(CVectorTests, ToStdVector_RoundTrip_PreservesData) {
  std::vector<double> original = {1.5, 2.5, 3.5, 4.5, 5.5};
  
  CVector<double> vec = CVector<double>::from_std_vector(original);
  std::vector<double> result = vec.to_std_vector();
  
  ASSERT_EQ(result.size(), original.size());
  for (size_t i = 0; i < original.size(); ++i) {
    EXPECT_DOUBLE_EQ(result[i], original[i]);
  }
  
  // Clean up
  vec.free_cvector();
}

//--------------------------------------------------------------------------
//  CVector Memory Management Tests
//--------------------------------------------------------------------------
TEST(CVectorTests, FreeCVector_ValidVector_ClearsData) {
  std::vector<int> test = {1, 2, 3};
  CVector<int> vec = CVector<int>::from_std_vector(test);
  
  ASSERT_NE(vec.data, nullptr);
  ASSERT_GT(vec.size, 0u);
  
  vec.free_cvector();
  
  EXPECT_EQ(vec.data, nullptr);
  EXPECT_EQ(vec.size, 0u);
  EXPECT_EQ(vec.capacity, 0u);
}

TEST(CVectorTests, FreeCVector_AlreadyEmpty_NoError) {
  CVector<int> vec;
  
  // Should not crash
  vec.free_cvector();
  
  EXPECT_EQ(vec.data, nullptr);
  EXPECT_EQ(vec.size, 0u);
  EXPECT_EQ(vec.capacity, 0u);
}

TEST(CVectorTests, FreeCVector_MultipleCalls_Safe) {
  std::vector<int> test = {1, 2, 3};
  CVector<int> vec = CVector<int>::from_std_vector(test);
  
  vec.free_cvector();
  vec.free_cvector();  // Second call should be safe
  
  EXPECT_EQ(vec.data, nullptr);
  EXPECT_EQ(vec.size, 0u);
  EXPECT_EQ(vec.capacity, 0u);
}

//--------------------------------------------------------------------------
//  CVector Indexing Operator Tests
//--------------------------------------------------------------------------
TEST(CVectorTests, IndexOperator_ValidIndex_ReturnsElement) {
  std::vector<int> test = {10, 20, 30, 40, 50};
  CVector<int> vec = CVector<int>::from_std_vector(test);
  
  EXPECT_EQ(vec[0], 10);
  EXPECT_EQ(vec[2], 30);
  EXPECT_EQ(vec[4], 50);
  
  // Clean up
  vec.free_cvector();
}

TEST(CVectorTests, IndexOperator_Modification_ChangesValue) {
  std::vector<int> test = {1, 2, 3};
  CVector<int> vec = CVector<int>::from_std_vector(test);
  
  vec[1] = 999;
  
  EXPECT_EQ(vec[1], 999);
  EXPECT_EQ(vec[0], 1);  // Others unchanged
  EXPECT_EQ(vec[2], 3);
  
  // Clean up
  vec.free_cvector();
}

TEST(CVectorTests, IndexOperator_Const_ReturnsConstReference) {
  std::vector<int> test = {5, 10, 15};
  const CVector<int> vec = CVector<int>::from_std_vector(test);
  
  EXPECT_EQ(vec[0], 5);
  EXPECT_EQ(vec[1], 10);
  EXPECT_EQ(vec[2], 15);
  
  // Clean up (need const_cast for cleanup in this test context)
  const_cast<CVector<int>&>(vec).free_cvector();
}

//--------------------------------------------------------------------------
//  CVector with Different Types Tests
//--------------------------------------------------------------------------
TEST(CVectorTests, FromStdVector_StringVector_Works) {
  std::vector<std::string> test = {"hello", "world", "test"};
  CVector<std::string> vec = CVector<std::string>::from_std_vector(test);
  
  ASSERT_NE(vec.data, nullptr);
  EXPECT_EQ(vec.size, 3u);
  EXPECT_EQ(vec[0], "hello");
  EXPECT_EQ(vec[1], "world");
  EXPECT_EQ(vec[2], "test");
  
  // Clean up
  vec.free_cvector();
}

TEST(CVectorTests, FromStdVector_BoolVector_Works) {
  std::vector<bool> test = {true, false, true, true, false};
  
  // Note: std::vector<bool> is specialized, so we convert to regular bool array
  std::vector<char> test_chars;
  for (bool b : test) {
    test_chars.push_back(b ? 1 : 0);
  }
  
  CVector<char> vec = CVector<char>::from_std_vector(test_chars);
  
  ASSERT_NE(vec.data, nullptr);
  EXPECT_EQ(vec.size, 5u);
  EXPECT_EQ(vec[0], 1);
  EXPECT_EQ(vec[1], 0);
  EXPECT_EQ(vec[2], 1);
  
  // Clean up
  vec.free_cvector();
}

TEST(CVectorTests, FromStdVector_StructVector_Works) {
  struct TestStruct {
    int x;
    double y;
  };
  
  std::vector<TestStruct> test = {{1, 1.5}, {2, 2.5}, {3, 3.5}};
  CVector<TestStruct> vec = CVector<TestStruct>::from_std_vector(test);
  
  ASSERT_NE(vec.data, nullptr);
  EXPECT_EQ(vec.size, 3u);
  EXPECT_EQ(vec[0].x, 1);
  EXPECT_DOUBLE_EQ(vec[0].y, 1.5);
  EXPECT_EQ(vec[2].x, 3);
  EXPECT_DOUBLE_EQ(vec[2].y, 3.5);
  
  // Clean up
  vec.free_cvector();
}

//--------------------------------------------------------------------------
//  CVector Edge Cases
//--------------------------------------------------------------------------
TEST(CVectorTests, FromStdVector_LargeVector_HandlesCorrectly) {
  std::vector<int> test(10000, 42);
  CVector<int> vec = CVector<int>::from_std_vector(test);
  
  ASSERT_NE(vec.data, nullptr);
  EXPECT_EQ(vec.size, 10000u);
  EXPECT_EQ(vec.capacity, 10000u);
  
  // Spot check
  EXPECT_EQ(vec[0], 42);
  EXPECT_EQ(vec[5000], 42);
  EXPECT_EQ(vec[9999], 42);
  
  // Clean up
  vec.free_cvector();
}

TEST(CVectorTests, FromStdVector_SingleElement_Works) {
  std::vector<int> test = {123};
  CVector<int> vec = CVector<int>::from_std_vector(test);
  
  ASSERT_NE(vec.data, nullptr);
  EXPECT_EQ(vec.size, 1u);
  EXPECT_EQ(vec[0], 123);
  
  // Clean up
  vec.free_cvector();
}

TEST(CVectorTests, ToStdVector_NullDataNonZeroSize_ReturnsEmpty) {
  // Create invalid state (shouldn't happen in normal use)
  CVector<int> vec;
  vec.data = nullptr;
  vec.size = 10;  // Invalid: size without data
  
  std::vector<int> result = vec.to_std_vector();
  
  EXPECT_TRUE(result.empty());
}

//--------------------------------------------------------------------------
//  CVector with CString Tests (Nested ABI-safe types)
//--------------------------------------------------------------------------
TEST(CVectorTests, FromStdVector_CStringVector_Works) {
  // Create vector of CStrings
  std::vector<CString> test;
  test.push_back(CString::from_std_string("first"));
  test.push_back(CString::from_std_string("second"));
  test.push_back(CString::from_std_string("third"));
  
  CVector<CString> vec = CVector<CString>::from_std_vector(test);
  
  ASSERT_NE(vec.data, nullptr);
  EXPECT_EQ(vec.size, 3u);
  EXPECT_STREQ(vec[0].data, "first");
  EXPECT_STREQ(vec[1].data, "second");
  EXPECT_STREQ(vec[2].data, "third");
  
  // Clean up CStrings
  for (size_t i = 0; i < test.size(); ++i) {
    test[i].free_cstring();
  }
  
  // Clean up CVector
  vec.free_cvector();
}

//--------------------------------------------------------------------------
//  CVector Copy Constructor Tests
//--------------------------------------------------------------------------
TEST(CVectorTests, CopyConstruction_NotProvided_UsesShallowCopy) {
  // Note: CVector intentionally does not provide a copy constructor
  // This test documents the expected behavior with default copy
  std::vector<int> test = {1, 2, 3};
  CVector<int> vec1 = CVector<int>::from_std_vector(test);
  
  // Create a copy (shallow)
  CVector<int> vec2 = vec1;
  
  // Both should point to same data
  EXPECT_EQ(vec1.data, vec2.data);
  EXPECT_EQ(vec1.size, vec2.size);
  EXPECT_EQ(vec1.capacity, vec2.capacity);
  
  // Only free once to avoid double-free
  vec1.free_cvector();
  
  // vec2.data is now dangling - don't access it
}
