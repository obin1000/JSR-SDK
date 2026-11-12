# Uses an unpacked JSR-SDK release through find_package(), the way a project
# that doesn't use FetchContent would. ci/consumer builds and runs it with
# ctest --build-and-test, with each compiler CI tests.
cmake_minimum_required(VERSION 3.21)
project(jsr_sdk_find_package LANGUAGES C CXX)

# JSR_SDK_DIR is the unpacked release: find_package() looks in its cmake/.
set(JSR_SDK_DIR "" CACHE PATH "Unpacked JSR-SDK release")
list(APPEND CMAKE_PREFIX_PATH "${JSR_SDK_DIR}")

find_package(JSR-SDK CONFIG REQUIRED)
if(NOT TARGET JSR-SDK::JSR-SDK)
  message(FATAL_ERROR "find_package(JSR-SDK) did not define JSR-SDK::JSR-SDK")
endif()

# The VERSION file is only in release zips, so CI's layout reports 0.0.0;
# then check that a version request is refused, as it should be.
if(JSR-SDK_VERSION VERSION_EQUAL "0.0.0")
  find_package(JSR-SDK 0.1 CONFIG QUIET)
  if(JSR-SDK_FOUND)
    message(FATAL_ERROR "find_package(JSR-SDK 0.1) accepted version 0.0.0")
  endif()
  set(JSR-SDK_FOUND TRUE)
endif()
message(STATUS "Found JSR-SDK ${JSR-SDK_VERSION}")

add_executable(find_package_check main.cpp)
target_link_libraries(find_package_check PRIVATE JSR-SDK::JSR-SDK)
target_compile_features(find_package_check PRIVATE cxx_std_17)
jsr_sdk_copy_runtime(find_package_check)
