# Builds small programs against an unpacked JSR-SDK release, the way a
# downstream project such as Raijin consumes it, and runs them with CTest.
# CI builds this with every supported compiler.
#
#   cmake -S ci/consumer -B build -G Ninja -DJSR_SDK_DIR=<unpacked release>
#   cmake --build build
#   ctest --test-dir build --output-on-failure
cmake_minimum_required(VERSION 3.21)
project(jsr_sdk_consumer LANGUAGES C CXX)

set(JSR_SDK_DIR "" CACHE PATH "Unpacked JSR-SDK release: include/, lib/ and CMakeLists.txt")
if(NOT EXISTS "${JSR_SDK_DIR}/CMakeLists.txt")
  message(FATAL_ERROR "Set JSR_SDK_DIR to an unpacked JSR-SDK release (got '${JSR_SDK_DIR}').")
endif()

include(GNUInstallDirs)
set(JSR_SDK_USE_DYNAMIC_LIB ON CACHE BOOL "" FORCE)
add_subdirectory("${JSR_SDK_DIR}" jsr-sdk)

enable_testing()

# Builds one test program against JSR-SDK, copies the SDK's DLLs and plugins
# next to it and registers it with CTest.
function(add_consumer_test name)
  add_executable(${name} ${ARGN})
  target_link_libraries(${name} PRIVATE JSR-SDK)
  if(MSVC)
    target_compile_options(${name} PRIVATE /W4 /permissive-)
  else()
    target_compile_options(${name} PRIVATE -Wall -Wextra)
  endif()
  add_custom_command(TARGET ${name} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:${name}> $<TARGET_FILE_DIR:${name}>
    COMMAND_EXPAND_LISTS)
  add_test(NAME ${name} COMMAND ${name})
  set_tests_properties(${name} PROPERTIES TIMEOUT 120)
endfunction()

# JSRSDKManager.hpp is a C++ class interface, so it only works with compilers
# that share MSVC's C++ ABI: MSVC itself and clang-cl.
if(MSVC)
  add_consumer_test(legacy_smoke legacy_smoke.cpp)
  target_compile_features(legacy_smoke PRIVATE cxx_std_17)
endif()
