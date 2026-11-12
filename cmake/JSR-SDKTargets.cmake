# The JSR-SDK targets, for an unpacked release (include/, lib/) or a checkout
# with the library built into lib/. Both ways to use the SDK include this
# file:
#   - add_subdirectory() or FetchContent, through the root CMakeLists.txt;
#   - find_package(JSR-SDK), through cmake/JSR-SDKConfig.cmake.
#
# It defines:
#   JSR-SDK (and its alias JSR-SDK::JSR-SDK)
#               the library: link it to use jsr_sdk.h, jsr_sdk.hpp or
#               JSRSDKManager.hpp;
#   jsr_sdk_copy_runtime(<target>)
#               copies the DLLs a program needs next to it after each build.
#
# Options (set them before including): JSR_SDK_USE_DYNAMIC_LIB and the
# JSR_SDK_INCLUDE_* switches for the plugin DLLs.
if(TARGET JSR-SDK)
    return()
endif()

if(NOT WIN32)
    message(FATAL_ERROR "JSR-SDK is Windows-only.")
endif()

get_filename_component(JSR_SDK_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

option(JSR_SDK_USE_DYNAMIC_LIB "Use the dynamic library instead of the static" ON)

# Options to selectively include DLL dependencies
option(JSR_SDK_INCLUDE_CYUSB "Include CyUSB DLL" ON)
option(JSR_SDK_INCLUDE_DOTNET_DIAG "Include JSRDotNET_Diag DLL" ON)
option(JSR_SDK_INCLUDE_USB_COMMON "Include JSRDotNET_USB_Common DLL" ON)
option(JSR_SDK_INCLUDE_USB_DPR650 "Include JSRDotNET_USB_DPR650 DLL" ON)
option(JSR_SDK_INCLUDE_USB_PUREVIEWHID "Include JSRDotNET_USB_PureViewHid DLL" ON)
option(JSR_SDK_INCLUDE_DOTNETDPR "Include JSRDotNETDPR DLL" ON)
option(JSR_SDK_INCLUDE_NEWTONSOFT_JSON "Include Newtonsoft.json DLL" ON)
option(JSR_SDK_INCLUDE_SIMPLEHID "Include SimpleHID DLL" ON)

# MinGW-w64 GCC and MinGW clang can only use the C API (jsr_sdk.h) and the C++
# wrapper (jsr_sdk.hpp) through the DLL. The static library holds MSVC /clr
# objects, which they can't link.
if(MINGW AND NOT JSR_SDK_USE_DYNAMIC_LIB)
    message(FATAL_ERROR
        "JSR-SDK: MinGW builds need the dynamic library. "
        "Set JSR_SDK_USE_DYNAMIC_LIB to ON.")
endif()

# Detect if compiling for x64 or x86
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(_jsr_sdk_arch "x64")
else()
    set(_jsr_sdk_arch "Win32")
endif()

set(JSR_SDK_INCLUDE_DIR "${JSR_SDK_ROOT_DIR}/include")
set(JSR_SDK_PLUGINS_DIR "${JSR_SDK_ROOT_DIR}/lib/plugins")

# The .NET assemblies (JSRDotNETSDK.dll and the plugins) are only loaded at
# run time, and no program calls into them. They are still linked, through
# their import libraries in lib/plugins, for every compiler: that adds no
# import to the program (MSVC and GNU ld both drop them), but it makes them
# runtime dependencies of JSR-SDK, so $<TARGET_RUNTIME_DLLS:app> lists them
# and a program can copy every DLL it needs with one command.

# DLLs a program needs next to it at run time; see jsr_sdk_copy_runtime().
set_property(GLOBAL PROPERTY JSR_SDK_RUNTIME_TARGETS JSR-SDK JSRDotNETSDK)

add_library(JSR-SDK SHARED IMPORTED GLOBAL)
add_library(JSR-SDK::JSR-SDK ALIAS JSR-SDK)
add_library(JSRDotNETSDK SHARED IMPORTED GLOBAL)
set_target_properties(JSRDotNETSDK PROPERTIES IMPORTED_IMPLIB "${JSR_SDK_PLUGINS_DIR}/JSRDotNETSDK.lib")
target_link_libraries(JSR-SDK INTERFACE JSRDotNETSDK)

if(JSR_SDK_USE_DYNAMIC_LIB)
    message(STATUS "Adding JSR-SDK dynamic library ${_jsr_sdk_arch}")
    set(_jsr_sdk_lib_dir "${JSR_SDK_ROOT_DIR}/lib/${_jsr_sdk_arch}/dynamic")
    if(MINGW)
        set(_jsr_sdk_implib "${_jsr_sdk_lib_dir}/libJSR-SDK.dll.a")
    else()
        set(_jsr_sdk_implib "${_jsr_sdk_lib_dir}/JSR-SDK.lib")
    endif()
    set_target_properties(JSR-SDK PROPERTIES
        IMPORTED_LOCATION "${_jsr_sdk_lib_dir}/JSR-SDK.dll"
        IMPORTED_IMPLIB "${_jsr_sdk_implib}"
        INTERFACE_INCLUDE_DIRECTORIES "${JSR_SDK_INCLUDE_DIR}")
    set_target_properties(JSRDotNETSDK PROPERTIES
        IMPORTED_LOCATION "${_jsr_sdk_lib_dir}/JSRDotNETSDK.dll")
else()
    message(STATUS "Adding JSR-SDK static library ${_jsr_sdk_arch}")
    set(_jsr_sdk_lib_dir "${JSR_SDK_ROOT_DIR}/lib/${_jsr_sdk_arch}/static")
    set_target_properties(JSR-SDK PROPERTIES
        IMPORTED_LOCATION "${_jsr_sdk_lib_dir}/JSR-SDK.lib"
        IMPORTED_IMPLIB "${_jsr_sdk_lib_dir}/JSR-SDK.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${JSR_SDK_INCLUDE_DIR}"
        # jsr_sdk.h declares its functions without __declspec(dllimport).
        INTERFACE_COMPILE_DEFINITIONS "JSR_SDK_STATIC")
    set_target_properties(JSRDotNETSDK PROPERTIES
        IMPORTED_LOCATION "${_jsr_sdk_lib_dir}/JSRDotNETSDK.dll")
endif()

target_include_directories(JSR-SDK INTERFACE "${JSR_SDK_INCLUDE_DIR}")

function(register_jsr_dll dll_name)
    add_library(${dll_name} SHARED IMPORTED GLOBAL)
    set_target_properties(${dll_name} PROPERTIES
        IMPORTED_LOCATION "${JSR_SDK_PLUGINS_DIR}/${dll_name}.dll"
        IMPORTED_IMPLIB   "${JSR_SDK_PLUGINS_DIR}/${dll_name}.lib")
    # Linked only so $<TARGET_RUNTIME_DLLS> includes it; see above.
    target_link_libraries(JSR-SDK INTERFACE ${dll_name})
    set_property(GLOBAL APPEND PROPERTY JSR_SDK_RUNTIME_TARGETS ${dll_name})
endfunction()

if(JSR_SDK_INCLUDE_CYUSB)
    register_jsr_dll("CyUSB")
endif()
if(JSR_SDK_INCLUDE_DOTNET_DIAG)
    register_jsr_dll("JSRDotNET_Diag")
endif()
if(JSR_SDK_INCLUDE_USB_COMMON)
    register_jsr_dll("JSRDotNET_USB_Common")
endif()
if(JSR_SDK_INCLUDE_USB_DPR650)
    register_jsr_dll("JSRDotNET_USB_DPR650")
endif()
if(JSR_SDK_INCLUDE_USB_PUREVIEWHID)
    register_jsr_dll("JSRDotNET_USB_PureViewHid")
endif()
if(JSR_SDK_INCLUDE_DOTNETDPR)
    register_jsr_dll("JSRDotNETDPR")
endif()
if(JSR_SDK_INCLUDE_NEWTONSOFT_JSON)
    register_jsr_dll("Newtonsoft.json")
endif()
if(JSR_SDK_INCLUDE_SIMPLEHID)
    register_jsr_dll("SimpleHID")
endif()

# Copies JSR-SDK.dll, JSRDotNETSDK.dll and the enabled plugin DLLs next to
# <target> after every build, so the program runs from its build directory.
# Works with every compiler, including MinGW, where the plugins aren't linked.
function(jsr_sdk_copy_runtime target)
    get_property(runtime_targets GLOBAL PROPERTY JSR_SDK_RUNTIME_TARGETS)
    set(runtime_files "")
    foreach(runtime_target IN LISTS runtime_targets)
        list(APPEND runtime_files "$<TARGET_FILE:${runtime_target}>")
    endforeach()
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${runtime_files} "$<TARGET_FILE_DIR:${target}>"
        VERBATIM)
endfunction()
