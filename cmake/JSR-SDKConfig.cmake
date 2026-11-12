# find_package(JSR-SDK) support. Point CMake at an unpacked release, for
# example with -DCMAKE_PREFIX_PATH=<unpacked release> or
# -DJSR-SDK_DIR=<unpacked release>/cmake, then:
#
#   find_package(JSR-SDK 0.14 CONFIG REQUIRED)
#   target_link_libraries(my_app PRIVATE JSR-SDK::JSR-SDK)
#   jsr_sdk_copy_runtime(my_app)
#
# Set JSR_SDK_USE_DYNAMIC_LIB and the JSR_SDK_INCLUDE_* options before
# find_package() to change them. cmake/JSR-SDKTargets.cmake describes the
# targets.
cmake_minimum_required(VERSION 3.21)
include("${CMAKE_CURRENT_LIST_DIR}/JSR-SDKTargets.cmake")
set(JSR-SDK_FOUND TRUE)
