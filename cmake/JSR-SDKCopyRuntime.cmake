# Copies the SDK's DLLs into a folder, for jsr_sdk_copy_runtime(). Run as
#   cmake -DFILES=<file;file;...> -DDESTINATION=<folder> -P JSR-SDKCopyRuntime.cmake
#
# Two programs that finish linking at the same time copy the same DLLs into
# the same folder at once, and Windows (or a virus scanner) may briefly hold a
# DLL that is being written: cmake -E copy_if_different then fails with
# "Permission denied". This skips a file that is already identical, which
# also covers the other copy having just finished, and otherwise retries a
# busy file for about 10 seconds before giving up.
cmake_minimum_required(VERSION 3.21)

if(NOT DEFINED FILES OR NOT DEFINED DESTINATION)
    message(FATAL_ERROR "Usage: cmake -DFILES=<files> -DDESTINATION=<folder> -P ${CMAKE_CURRENT_LIST_FILE}")
endif()

file(MAKE_DIRECTORY "${DESTINATION}")
foreach(source IN LISTS FILES)
    get_filename_component(name "${source}" NAME)
    set(target "${DESTINATION}/${name}")
    foreach(attempt RANGE 1 50)
        file(COPY_FILE "${source}" "${target}" ONLY_IF_DIFFERENT RESULT result)
        if(result EQUAL 0)
            break()
        endif()
        if(attempt EQUAL 50)
            message(FATAL_ERROR "Cannot copy ${source} to ${DESTINATION}: ${result}")
        endif()
        execute_process(COMMAND ${CMAKE_COMMAND} -E sleep 0.2)
    endforeach()
endforeach()
