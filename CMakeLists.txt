cmake_minimum_required(VERSION 3.14)
project(JSR-SDK LANGUAGES C CXX)

option(JSR_SDK_USE_DYNAMIC_LIB "Use the dynamic library instead of the static" OFF)


if(NOT WIN32)
    message(FATAL_ERROR "JSR-SDK is Windows-only.")
endif()

# Detect if compiling for x64 or x86
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH_DIR "x64")
else()
    set(ARCH_DIR "Win32")
endif()



add_library(JSR-SDK SHARED IMPORTED GLOBAL)

if(JSR_SDK_USE_DYNAMIC_LIB)
   message(STATUS "Adding JSR-SDK dynamic library ${ARCH_DIR}")
   set_target_properties(JSR-SDK PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/${ARCH_DIR}/dynamic/JSR-SDK.dll"
    IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/${ARCH_DIR}/dynamic/JSR-SDK.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
   )
else()
    message(STATUS "Adding JSR-SDK static library ${ARCH_DIR}")
    set_target_properties(JSR-SDK PROPERTIES
      IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/${ARCH_DIR}/static/JSR-SDK.lib"
      INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )
endif()

target_include_directories(JSR-SDK INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

install(IMPORTED_RUNTIME_ARTIFACTS JSRSDK DESTINATION ${CMAKE_INSTALL_BINDIR})

