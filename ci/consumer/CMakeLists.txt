# Builds small programs against an unpacked JSR-SDK release, the way a
# downstream project such as Raijin consumes it, and runs them with CTest.
# CI builds this with MSVC, clang-cl, MinGW-w64 GCC and MinGW clang.
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

add_subdirectory("${JSR_SDK_DIR}" jsr-sdk)

enable_testing()

# The portable C API unit tests (test/capi), with googletest, so every
# compiler runs them against the release layout, not only MSVC.
include("${CMAKE_CURRENT_LIST_DIR}/../../test/googletest.cmake")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../../test/capi" capi)

# Builds one test program against JSR-SDK, copies the SDK's DLLs and plugins
# next to it and registers it with CTest. STRICT turns warnings into errors.
# RUNTIME_DLLS copies the DLLs with $<TARGET_RUNTIME_DLLS> instead of
# jsr_sdk_copy_runtime(), into a folder of its own, as Raijin does.
function(add_consumer_test name source)
  cmake_parse_arguments(ARG "STRICT;RUNTIME_DLLS" "" "" ${ARGN})
  add_executable(${name} ${source})
  target_link_libraries(${name} PRIVATE JSR-SDK)
  if(MSVC)
    target_compile_options(${name} PRIVATE /W4 $<$<COMPILE_LANGUAGE:CXX>:/permissive->)
    if(ARG_STRICT)
      target_compile_options(${name} PRIVATE /WX)
    endif()
  else()
    target_compile_options(${name} PRIVATE -Wall -Wextra -Wpedantic)
    if(ARG_STRICT)
      target_compile_options(${name} PRIVATE -Werror)
    endif()
  endif()
  if(ARG_RUNTIME_DLLS)
    set_target_properties(${name} PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${name}")
    add_custom_command(TARGET ${name} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
              $<TARGET_RUNTIME_DLLS:${name}> $<TARGET_FILE_DIR:${name}>
      COMMAND_EXPAND_LISTS)
  else()
    jsr_sdk_copy_runtime(${name})
  endif()
  add_test(NAME ${name} COMMAND ${name})
  set_tests_properties(${name} PROPERTIES TIMEOUT 120)
endfunction()

# The C API, compiled as C99.
add_consumer_test(c_smoke c_smoke.c STRICT)
set_target_properties(c_smoke PROPERTIES C_STANDARD 99 C_STANDARD_REQUIRED ON C_EXTENSIONS OFF)

# The header-only C++ wrapper.
add_consumer_test(cpp_smoke cpp_smoke.cpp STRICT)
set_target_properties(cpp_smoke PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)

# The same program, but its DLLs come only from $<TARGET_RUNTIME_DLLS>, as in
# Raijin. Its simulator part needs the JSRDotNET_Diag plugin, so it fails
# unless every plugin DLL is a runtime dependency of JSR-SDK.
add_consumer_test(cpp_smoke_runtime_dlls cpp_smoke.cpp STRICT RUNTIME_DLLS)
set_target_properties(cpp_smoke_runtime_dlls PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)

# The same release through find_package(JSR-SDK) instead of add_subdirectory():
# a separate project, configured, built and run by CTest with this compiler.
set(_find_package_options
  "-DJSR_SDK_DIR=${JSR_SDK_DIR}"
  "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}"
  "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}")
if(CMAKE_BUILD_TYPE)
  list(APPEND _find_package_options "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
endif()
if(CMAKE_TLS_CAINFO)
  list(APPEND _find_package_options "-DCMAKE_TLS_CAINFO=${CMAKE_TLS_CAINFO}")
endif()
add_test(NAME find_package
  COMMAND ${CMAKE_CTEST_COMMAND} --build-and-test
          "${CMAKE_CURRENT_SOURCE_DIR}/find_package"
          "${CMAKE_CURRENT_BINARY_DIR}/find_package"
          --build-generator "${CMAKE_GENERATOR}"
          --build-config $<CONFIG>
          --build-options ${_find_package_options}
          --test-command find_package_check)
set_tests_properties(find_package PROPERTIES TIMEOUT 300)

# JSRSDKManager.hpp is a C++ class interface, so it only works with compilers
# that share MSVC's C++ ABI: MSVC itself and clang-cl.
if(MSVC)
  add_consumer_test(legacy_smoke legacy_smoke.cpp)
  set_target_properties(legacy_smoke PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
endif()
