# The JSR-SDK unit tests:
#   clr_tests   C++/CLI tests of the DLL's internals, and of the C API against
#               the DLL's implementation of the SDK's methods (clr/). MSVC only.
#   capi_tests  portable tests of the C API and jsr::Manager (capi/).
#
# Build the library first, for the same platform (x64 or Win32):
#   msbuild JSR-SDK.vcxproj -p:Configuration=dynamic -p:Platform=x64
# then:
#   cmake -S test -B build/test-x64 -A x64
#   cmake --build build/test-x64 --config Release
#   ctest --test-dir build/test-x64 -C Release --output-on-failure
# CMake only builds C++/CLI with a Visual Studio generator. The tests run
# against the SDK's simulated pulser-receivers; the *IntegrationTest suites
# need real hardware and run only when you start the programs by hand.
cmake_minimum_required(VERSION 3.21)
project(jsr_sdk_tests LANGUAGES C CXX)

if(NOT CMAKE_GENERATOR MATCHES "^Visual Studio")
  message(FATAL_ERROR "The C++/CLI tests need a Visual Studio generator (got ${CMAKE_GENERATOR}).")
endif()

set(JSR_SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(JSR_SDK_TEST_ARCH "x64")
else()
  set(JSR_SDK_TEST_ARCH "Win32")
endif()
if(NOT EXISTS "${JSR_SDK_ROOT}/lib/${JSR_SDK_TEST_ARCH}/dynamic/JSR-SDK.dll")
  message(FATAL_ERROR
    "lib/${JSR_SDK_TEST_ARCH}/dynamic/JSR-SDK.dll is missing. Build it first: "
    "msbuild JSR-SDK.vcxproj -p:Configuration=dynamic -p:Platform=${JSR_SDK_TEST_ARCH}")
endif()

# The SDK's own CMakeLists.txt describes the built DLL in lib/, exactly as a
# consumer of the release zip sees it.
set(JSR_SDK_USE_DYNAMIC_LIB ON CACHE BOOL "" FORCE)
add_subdirectory("${JSR_SDK_ROOT}" jsr-sdk)

include(googletest.cmake)
enable_testing()

add_subdirectory(capi)

# clr_tests compiles the DLL's implementation of the SDK's methods
# (jsr::detail::SdkManager) itself, so it can test it directly and compare
# the C API with it; the DLL only exports the C API.
add_executable(clr_tests
  "${JSR_SDK_ROOT}/src/CallbackGuard.cpp"
  "${JSR_SDK_ROOT}/src/JSRSDKManager.cpp"
  "${JSR_SDK_ROOT}/src/JSRSDKWrapper.cpp"
  clr/CApiParityTest.cpp
  clr/EnumParityTest.cpp
  clr/JSRSDKManagerIntegrationTest.cpp
  clr/ManagedErrorsTest.cpp
  clr/ManagerShutdownTest.cpp
  clr/MarshalEnumsTest.cpp
  clr/MarshallTypesTest.cpp
  clr/SimulatorTest.cpp)
target_link_libraries(clr_tests PRIVATE JSR-SDK GTest::gtest_main)
target_include_directories(clr_tests PRIVATE
  "${JSR_SDK_ROOT}/src"   # the DLL's internals: marshals/, SdkError.hpp, ...
  common)
target_compile_features(clr_tests PRIVATE cxx_std_20)
set_target_properties(clr_tests PROPERTIES
  COMMON_LANGUAGE_RUNTIME ""
  DOTNET_TARGET_FRAMEWORK_VERSION "v4.8"
  VS_DOTNET_REFERENCE_JSRDotNETSDK
    "${JSR_SDK_ROOT}/external/JSRDotNETSDK_1.2.14/Libs/JSRDotNETSDK.dll")
jsr_sdk_copy_runtime(clr_tests)

# A .NET program that must exit after destroying its managers (#13);
# ManagerShutdownTest runs it.
add_executable(shutdown_check clr/shutdown_check.cpp)
target_link_libraries(shutdown_check PRIVATE JSR-SDK)
target_compile_features(shutdown_check PRIVATE cxx_std_17)
set_target_properties(shutdown_check PROPERTIES
  COMMON_LANGUAGE_RUNTIME ""
  DOTNET_TARGET_FRAMEWORK_VERSION "v4.8")
jsr_sdk_copy_runtime(shutdown_check)
add_dependencies(clr_tests shutdown_check)
target_compile_definitions(clr_tests PRIVATE
  "JSR_SHUTDOWN_CHECK_EXE=\"$<TARGET_FILE:shutdown_check>\"")

add_test(NAME clr_tests COMMAND clr_tests --gtest_filter=-*IntegrationTest.*)
set_tests_properties(clr_tests PROPERTIES TIMEOUT 300)
