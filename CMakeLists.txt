cmake_minimum_required(VERSION 3.14)
project(JSR-SDK LANGUAGES C CXX)

option(JSR_SDK_USE_DYNAMIC_LIB "Use the dynamic library instead of the static" ON)


if(NOT WIN32)
    message(FATAL_ERROR "JSR-SDK is Windows-only.")
endif()

# Detect if compiling for x64 or x86
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH_DIR "x64")
else()
    set(ARCH_DIR "Win32")
endif()

set(JSR_SDK_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(JSR_SDK_PLUGINS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/plugins")


add_library(JSR-SDK SHARED IMPORTED GLOBAL)

if(JSR_SDK_USE_DYNAMIC_LIB)
   message(STATUS "Adding JSR-SDK dynamic library ${ARCH_DIR}")
   set_target_properties(JSR-SDK PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/${ARCH_DIR}/dynamic/JSR-SDK.dll"
    IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/${ARCH_DIR}/dynamic/JSR-SDK.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${JSR_SDK_INCLUDE_DIR}"
   )
else()
    message(STATUS "Adding JSR-SDK static library ${ARCH_DIR}")
    set_target_properties(JSR-SDK PROPERTIES
      IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/${ARCH_DIR}/static/JSR-SDK.lib"
      IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/${ARCH_DIR}/static/JSR-SDK.lib"
      INTERFACE_INCLUDE_DIRECTORIES "${JSR_SDK_INCLUDE_DIR}"
    )
endif()

target_include_directories(JSR-SDK INTERFACE "${JSR_SDK_INCLUDE_DIR}")

install(IMPORTED_RUNTIME_ARTIFACTS JSR-SDK DESTINATION ${CMAKE_INSTALL_BINDIR})


add_library(CyUSB SHARED IMPORTED GLOBAL)
set_target_properties(CyUSB PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/plugins/CyUSB.dll"
    IMPORTED_IMPLIB   "${CMAKE_CURRENT_SOURCE_DIR}/lib/plugins/CyUSB.lib")

target_link_libraries(JSR-SDK INTERFACE CyUSB)

add_library(JSRDotNET_Diag SHARED IMPORTED GLOBAL)
set_target_properties(JSRDotNET_Diag PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/plugins/JSRDotNET_Diag.dll"
    IMPORTED_IMPLIB   "${CMAKE_CURRENT_SOURCE_DIR}/lib/plugins/JSRDotNET_Diag.lib")

target_link_libraries(JSR-SDK INTERFACE JSRDotNET_Diag)


add_library(JSRDotNET_USB_Common SHARED IMPORTED GLOBAL)
set_target_properties(JSRDotNET_USB_Common PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/plugins/JSRDotNET_USB_Common.dll"
    IMPORTED_IMPLIB   "${CMAKE_CURRENT_SOURCE_DIR}/lib/plugins/JSRDotNET_USB_Common.lib")

target_link_libraries(JSR-SDK INTERFACE JSRDotNET_USB_Common)

add_library(JSRDotNET_USB_DPR650 SHARED IMPORTED GLOBAL)
set_target_properties(JSRDotNET_USB_DPR650 PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/plugins/JSRDotNET_USB_DPR650.dll"
    IMPORTED_IMPLIB   "${CMAKE_CURRENT_SOURCE_DIR}/lib/plugins/JSRDotNET_USB_DPR650.lib")

target_link_libraries(JSR-SDK INTERFACE JSRDotNET_USB_DPR650)

add_library(JSRDotNET_USB_PureViewHid SHARED IMPORTED GLOBAL)
set_target_properties(JSRDotNET_USB_PureViewHid PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/plugins/JSRDotNET_USB_PureViewHid.dll"
    IMPORTED_IMPLIB   "${CMAKE_CURRENT_SOURCE_DIR}/lib/plugins/JSRDotNET_USB_PureViewHid.lib")

target_link_libraries(JSR-SDK INTERFACE JSRDotNET_USB_PureViewHid)

add_library(JSRDotNETDPR SHARED IMPORTED GLOBAL)
set_target_properties(JSRDotNETDPR PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/plugins/JSRDotNETDPR.dll"
    IMPORTED_IMPLIB   "${CMAKE_CURRENT_SOURCE_DIR}/lib/plugins/JSRDotNETDPR.lib")

target_link_libraries(JSR-SDK INTERFACE JSRDotNETDPR)


add_library(Newtonsoft.Json SHARED IMPORTED GLOBAL)
set_target_properties(Newtonsoft.Json PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/plugins/Newtonsoft.Json.dll"
    IMPORTED_IMPLIB   "${CMAKE_CURRENT_SOURCE_DIR}/lib/plugins/Newtonsoft.Json.lib")

target_link_libraries(JSR-SDK INTERFACE Newtonsoft.Json)


add_library(SimpleHID SHARED IMPORTED GLOBAL)
set_target_properties(SimpleHID PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/plugins/SimpleHID.dll"
    IMPORTED_IMPLIB   "${CMAKE_CURRENT_SOURCE_DIR}/lib/plugins/SimpleHID.lib")

target_link_libraries(JSR-SDK INTERFACE SimpleHID)
