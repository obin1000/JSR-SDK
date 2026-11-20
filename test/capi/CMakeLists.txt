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

# The DLLs in a folder of their own, found through PATH: JSR-SDK.dll must
# find JSRDotNETSDK.dll and the plugins' dependencies next to itself. The
# program gets its own output folder, so no DLL sits next to it.
set(_elsewhere "${CMAKE_CURRENT_BINARY_DIR}/dlls_elsewhere/$<CONFIG>")
add_executable(capi_dlls_elsewhere dlls_elsewhere_check.c)
target_link_libraries(capi_dlls_elsewhere PRIVATE JSR-SDK)
set_target_properties(capi_dlls_elsewhere PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${_elsewhere}/program")
get_property(_runtime_targets GLOBAL PROPERTY JSR_SDK_RUNTIME_TARGETS)
set(_runtime_files "")
foreach(_runtime_target IN LISTS _runtime_targets)
  list(APPEND _runtime_files "$<TARGET_FILE:${_runtime_target}>")
endforeach()
add_custom_command(TARGET capi_dlls_elsewhere POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory "${_elsewhere}/dlls"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_runtime_files} "${_elsewhere}/dlls"
  VERBATIM)
add_test(NAME capi_dlls_elsewhere COMMAND capi_dlls_elsewhere)
set_tests_properties(capi_dlls_elsewhere PROPERTIES
  TIMEOUT 120
  ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:${_elsewhere}/dlls")

# The integration tests need a real pulser-receiver: run capi_tests by hand
# with one attached for those.
add_test(NAME capi_tests COMMAND capi_tests --gtest_filter=-*IntegrationTest.*)
set_tests_properties(capi_tests PROPERTIES TIMEOUT 300)
