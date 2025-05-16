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
add_library(JSRDotNETSDK SHARED IMPORTED GLOBAL)
set_target_properties(JSRDotNETSDK PROPERTIES IMPORTED_IMPLIB "${JSR_SDK_PLUGINS_DIR}/JSRDotNETSDK.lib")
target_link_libraries(JSR-SDK INTERFACE JSRDotNETSDK)


if(JSR_SDK_USE_DYNAMIC_LIB)
   message(STATUS "Adding JSR-SDK dynamic library ${ARCH_DIR}")
   set_target_properties(JSR-SDK PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/${ARCH_DIR}/dynamic/JSR-SDK.dll"
    IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/${ARCH_DIR}/dynamic/JSR-SDK.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${JSR_SDK_INCLUDE_DIR}"
   )
   set_target_properties(JSRDotNETSDK PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/${ARCH_DIR}/dynamic/JSRDotNETSDK.dll")
else()
    message(STATUS "Adding JSR-SDK static library ${ARCH_DIR}")
    set_target_properties(JSR-SDK PROPERTIES
      IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/${ARCH_DIR}/static/JSR-SDK.lib"
      IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/${ARCH_DIR}/static/JSR-SDK.lib"
      INTERFACE_INCLUDE_DIRECTORIES "${JSR_SDK_INCLUDE_DIR}"
    )
    set_target_properties(JSRDotNETSDK PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/${ARCH_DIR}/static/JSRDotNETSDK.dll")
endif()

target_include_directories(JSR-SDK INTERFACE "${JSR_SDK_INCLUDE_DIR}")

install(IMPORTED_RUNTIME_ARTIFACTS JSR-SDK DESTINATION ${CMAKE_INSTALL_BINDIR})
install(IMPORTED_RUNTIME_ARTIFACTS JSRDotNETSDK DESTINATION ${CMAKE_INSTALL_BINDIR})


function(register_jsr_dll dll_name)
    add_library(${dll_name} SHARED IMPORTED GLOBAL)
    set_target_properties(${dll_name} PROPERTIES
        IMPORTED_LOCATION "${JSR_SDK_PLUGINS_DIR}/${dll_name}.dll"
        IMPORTED_IMPLIB   "${JSR_SDK_PLUGINS_DIR}/${dll_name}.lib")

    target_link_libraries(JSR-SDK INTERFACE ${dll_name})
    install(IMPORTED_RUNTIME_ARTIFACTS
        ${dll_name}
        DESTINATION "${CMAKE_INSTALL_BINDIR}") 
endfunction()

register_jsr_dll("CyUSB")
register_jsr_dll("JSRDotNET_Diag")
register_jsr_dll("JSRDotNET_USB_Common")
register_jsr_dll("JSRDotNET_USB_DPR650")
register_jsr_dll("JSRDotNET_USB_PureViewHid")
register_jsr_dll("JSRDotNETDPR")
register_jsr_dll("Newtonsoft.json")
register_jsr_dll("SimpleHID")
