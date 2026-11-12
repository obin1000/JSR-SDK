# Tells find_package(JSR-SDK <version>) whether this release is compatible.
# The version comes from the VERSION file that release.yml puts in the
# release zip ("v0.14.0"). Without it, for example in a checkout, the version
# is 0.0.0 and only find_package() without a version accepts it.
#
# Compatible means the same major version and at least the requested one.
# Before 1.0, a minor release may break the API, so 0.x also needs the same
# minor version.
set(PACKAGE_VERSION "0.0.0")
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../VERSION")
    file(STRINGS "${CMAKE_CURRENT_LIST_DIR}/../VERSION" _jsr_sdk_version LIMIT_COUNT 1)
    string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" _jsr_sdk_version "${_jsr_sdk_version}")
    if(_jsr_sdk_version)
        set(PACKAGE_VERSION "${_jsr_sdk_version}")
    endif()
endif()

set(PACKAGE_VERSION_COMPATIBLE FALSE)
set(PACKAGE_VERSION_EXACT FALSE)
# The count, not the version itself: "0" is a valid request but false in if().
if(NOT PACKAGE_FIND_VERSION_COUNT)
    set(PACKAGE_VERSION_COMPATIBLE TRUE)
elseif(NOT PACKAGE_VERSION VERSION_LESS PACKAGE_FIND_VERSION)
    string(REPLACE "." ";" _jsr_sdk_parts "${PACKAGE_VERSION}")
    list(GET _jsr_sdk_parts 0 _jsr_sdk_major)
    list(GET _jsr_sdk_parts 1 _jsr_sdk_minor)
    if(_jsr_sdk_major EQUAL PACKAGE_FIND_VERSION_MAJOR AND
       (NOT _jsr_sdk_major EQUAL 0 OR PACKAGE_FIND_VERSION_COUNT LESS 2 OR
        _jsr_sdk_minor EQUAL PACKAGE_FIND_VERSION_MINOR))
        set(PACKAGE_VERSION_COMPATIBLE TRUE)
    endif()
    if(PACKAGE_VERSION VERSION_EQUAL PACKAGE_FIND_VERSION)
        set(PACKAGE_VERSION_EXACT TRUE)
    endif()
endif()
