cmake_minimum_required(VERSION 3.14)
project(JSR-SDK LANGUAGES C CXX)

if(NOT WIN32)
    message(FATAL_ERROR "JSR-SDK is Windows-only.")
endif()

# Paths to SDK libraries in release zip
set(WRAPPER_DLL "${CMAKE_CURRENT_SOURCE_DIR}/lib/JSR-SDK.dll")
set(WRAPPER_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/JSR-SDK.lib")

# Check if binaries exist; instruct if not
if(NOT EXISTS "${WRAPPER_DLL}" OR NOT EXISTS "${WRAPPER_LIB}")
    message(FATAL_ERROR "JSR-SDK binaries not found. Include has failed")
endif()

# Imported target definition
add_library(JSR-SDK SHARED IMPORTED GLOBAL)

set_target_properties(JSR-SDK PROPERTIES
    IMPORTED_LOCATION "${WRAPPER_DLL}"
    IMPORTED_IMPLIB "${WRAPPER_LIB}"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_include_directories(JSR-SDK INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

install(IMPORTED_RUNTIME_ARTIFACTS JSRSDK DESTINATION ${CMAKE_INSTALL_BINDIR})

