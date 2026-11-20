# Example programs for JSR-SDK, built against an unpacked release:
#
#   cmake -S examples -B build -DCMAKE_PREFIX_PATH=<unpacked release>
#   cmake --build build --config Release
#   ctest --test-dir build -C Release --output-on-failure
#
# See README.md for what each program shows.
cmake_minimum_required(VERSION 3.21)
project(jsr_sdk_examples LANGUAGES C CXX)

# Finds <release>/cmake/JSR-SDKConfig.cmake through CMAKE_PREFIX_PATH and
# defines the JSR-SDK::JSR-SDK target and jsr_sdk_copy_runtime().
find_package(JSR-SDK CONFIG REQUIRED)

# CI turns warnings into errors; the examples have to compile cleanly.
option(JSR_EXAMPLES_WERROR "Treat compiler warnings as errors" OFF)

enable_testing()

# Builds one example, copies JSR-SDK.dll and the plugin DLLs next to it and
# registers it with CTest, so `ctest` runs every example.
function(add_example name source)
  add_executable(${name} ${source})
  target_link_libraries(${name} PRIVATE JSR-SDK::JSR-SDK)
  if(MSVC)
    target_compile_options(${name} PRIVATE /W4 $<$<BOOL:${JSR_EXAMPLES_WERROR}>:/WX>)
  else()
    target_compile_options(${name} PRIVATE -Wall -Wextra -Wpedantic
                           $<$<BOOL:${JSR_EXAMPLES_WERROR}>:-Werror>)
  endif()
  jsr_sdk_copy_runtime(${name})
  add_test(NAME ${name} COMMAND ${name})
  set_tests_properties(${name} PROPERTIES TIMEOUT 120)
endfunction()

# The C API in jsr_sdk.h, as C99.
add_example(c_list_devices c/list_devices.c)
add_example(c_simulator c/simulator.c)
set_target_properties(c_list_devices c_simulator PROPERTIES
  C_STANDARD 99 C_STANDARD_REQUIRED ON C_EXTENSIONS OFF)

# The C++ wrapper in jsr_sdk.hpp, which needs C++17.
add_example(cpp_list_devices cpp/list_devices.cpp)
add_example(cpp_simulator cpp/simulator.cpp)
add_example(cpp_events cpp/events.cpp)
set_target_properties(cpp_list_devices cpp_simulator cpp_events PROPERTIES
  CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)
