# Portable tests for the C API (jsr_sdk.h) and jsr::Manager (jsr_sdk.hpp).
# They use no C++/CLI and no internal headers, so any compiler can build them:
# test/CMakeLists.txt adds them for MSVC, and ci/consumer/CMakeLists.txt for
# every compiler in CI, against the release layout.
#
# Expects the JSR-SDK target and jsr_sdk_copy_runtime() (from the SDK's
# CMakeLists.txt) and GTest::gtest_main (from test/googletest.cmake).
add_executable(capi_tests
  CApiTest.cpp
  CApiSimulatorTest.cpp
  JsrManagerIntegrationTest.cpp)
target_link_libraries(capi_tests PRIVATE JSR-SDK GTest::gtest_main)
target_include_directories(capi_tests PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../common")
target_compile_features(capi_tests PRIVATE cxx_std_17)
if(MSVC)
  target_compile_options(capi_tests PRIVATE /W4)
else()
  target_compile_options(capi_tests PRIVATE -Wall -Wextra)
endif()
jsr_sdk_copy_runtime(capi_tests)

# CI passes the release it built, so the tests check that the DLL reports it.
set(JSR_SDK_EXPECTED_VERSION "" CACHE STRING
  "Release JSR_GetLibraryVersion() must return; empty skips the check")
if(JSR_SDK_EXPECTED_VERSION)
  target_compile_definitions(capi_tests PRIVATE
    "JSR_SDK_EXPECTED_VERSION=\"${JSR_SDK_EXPECTED_VERSION}\"")
endif()

# A program built with the headers of another API version must get a clear
# error from jsr::Manager (#20).
add_executable(capi_version_mismatch version_mismatch_check.cpp)
target_link_libraries(capi_version_mismatch PRIVATE JSR-SDK)
target_compile_features(capi_version_mismatch PRIVATE cxx_std_17)
jsr_sdk_copy_runtime(capi_version_mismatch)
add_test(NAME capi_version_mismatch COMMAND capi_version_mismatch)
set_tests_properties(capi_version_mismatch PROPERTIES TIMEOUT 60)

# The integration tests need a real pulser-receiver: run capi_tests by hand
# with one attached for those.
add_test(NAME capi_tests COMMAND capi_tests --gtest_filter=-*IntegrationTest.*)
set_tests_properties(capi_tests PROPERTIES TIMEOUT 300)
