# JSR-SDK for add_subdirectory() and FetchContent. The targets themselves are
# in cmake/JSR-SDKTargets.cmake, which find_package(JSR-SDK) uses as well.
# 3.21 for install(IMPORTED_RUNTIME_ARTIFACTS).
cmake_minimum_required(VERSION 3.21)
project(JSR-SDK LANGUAGES C CXX)

include(GNUInstallDirs)
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/JSR-SDKTargets.cmake")

# Installing a program that uses the SDK installs the DLLs it needs too.
get_property(_jsr_sdk_runtime_targets GLOBAL PROPERTY JSR_SDK_RUNTIME_TARGETS)
foreach(_jsr_sdk_target IN LISTS _jsr_sdk_runtime_targets)
    install(IMPORTED_RUNTIME_ARTIFACTS ${_jsr_sdk_target}
        DESTINATION "${CMAKE_INSTALL_BINDIR}")
endforeach()
