# googletest for the JSR-SDK tests, pinned by version and SHA-256. Included
# by test/CMakeLists.txt and ci/consumer/CMakeLists.txt.
include_guard(GLOBAL)
include(FetchContent)

# Link the same C runtime (/MD) as the programs under test, with MSVC.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

FetchContent_Declare(googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.18.0.tar.gz
  URL_HASH SHA256=6e3191c1455468b3fc35a417fb565c1c5071aee1b7e7f85e30cf48a98d37d8b5
  DOWNLOAD_EXTRACT_TIMESTAMP ON)
FetchContent_MakeAvailable(googletest)
