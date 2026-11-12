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

# The integration tests need a real pulser-receiver: run capi_tests by hand
# with one attached for those.
add_test(NAME capi_tests COMMAND capi_tests --gtest_filter=-*IntegrationTest.*)
set_tests_properties(capi_tests PROPERTIES TIMEOUT 300)
