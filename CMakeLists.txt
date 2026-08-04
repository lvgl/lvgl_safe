# 3.24 for FetchContent's DOWNLOAD_EXTRACT_TIMESTAMP option.
cmake_minimum_required(VERSION 3.24)

project(lvgl_safe_examples LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# 1. Locate the pre-built LVGL Safe package archive
# ---------------------------------------------------------------------------
# Override with -DLS_PACKAGE_ARCHIVE=/path/to/lvgl_safe-x.y.z-<platform>.tar.gz
if(NOT LS_PACKAGE_ARCHIVE)
    file(GLOB LS_PACKAGE_ARCHIVE_CANDIDATES
         "${CMAKE_CURRENT_SOURCE_DIR}/lvgl_safe-*.tar.gz")
    list(SORT LS_PACKAGE_ARCHIVE_CANDIDATES)
    list(LENGTH LS_PACKAGE_ARCHIVE_CANDIDATES LS_PACKAGE_ARCHIVE_COUNT)

    if(LS_PACKAGE_ARCHIVE_COUNT EQUAL 0)
        message(FATAL_ERROR
                "No lvgl_safe-*.tar.gz found in ${CMAKE_CURRENT_SOURCE_DIR}. "
                "Pass -DLS_PACKAGE_ARCHIVE=<path> to point at the package archive.")
    endif()

    # Newest version last after the sort.
    list(GET LS_PACKAGE_ARCHIVE_CANDIDATES -1 LS_PACKAGE_ARCHIVE)
    if(LS_PACKAGE_ARCHIVE_COUNT GREATER 1)
        message(WARNING
                "Multiple package archives found; using ${LS_PACKAGE_ARCHIVE}. "
                "Pass -DLS_PACKAGE_ARCHIVE=<path> to choose a different one.")
    endif()
endif()

set(LS_PACKAGE_ARCHIVE "${LS_PACKAGE_ARCHIVE}" CACHE FILEPATH
    "LVGL Safe binary package archive (headers, static library, CMake config)")

if(NOT EXISTS "${LS_PACKAGE_ARCHIVE}")
    message(FATAL_ERROR "Package archive does not exist: ${LS_PACKAGE_ARCHIVE}")
endif()

# ---------------------------------------------------------------------------
# 2. Unpack it with FetchContent
# ---------------------------------------------------------------------------

include(FetchContent)
file(SHA256 "${LS_PACKAGE_ARCHIVE}" LS_PACKAGE_ARCHIVE_SHA256)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${LS_PACKAGE_ARCHIVE}")

FetchContent_Declare(lvgl_safe_pkg
                     URL "${LS_PACKAGE_ARCHIVE}"
                     URL_HASH "SHA256=${LS_PACKAGE_ARCHIVE_SHA256}"
                     DOWNLOAD_EXTRACT_TIMESTAMP FALSE)
FetchContent_MakeAvailable(lvgl_safe_pkg)
set(LS_PACKAGE_PREFIX "${lvgl_safe_pkg_SOURCE_DIR}")

# ---------------------------------------------------------------------------
# 3. Import the library through the CMake config package it ships
# ---------------------------------------------------------------------------

get_filename_component(LS_PACKAGE_ARCHIVE_NAME "${LS_PACKAGE_ARCHIVE}" NAME)
if(LS_PACKAGE_ARCHIVE_NAME MATCHES "^lvgl_safe-([0-9]+\\.[0-9]+\\.[0-9]+)")
    set(LS_PACKAGE_VERSION "${CMAKE_MATCH_1}")
endif()

find_package(lvgl_safe ${LS_PACKAGE_VERSION} REQUIRED CONFIG
             PATHS "${LS_PACKAGE_PREFIX}"
             NO_DEFAULT_PATH)

message(STATUS "Found LVGL Safe ${lvgl_safe_VERSION} in ${LS_PACKAGE_PREFIX}")

# IMPORTANT NOTE: the shipped include/ls_conf.h is authoritative — its values are baked
# into the struct layouts inside liblvgl_safe.a, ls_conf.h must not be changed as all
# functionality of the lib as already made available.

# ---------------------------------------------------------------------------
# 4. SDL2 backend used by the examples (${LS_SDL2_LIBRARY})
# ---------------------------------------------------------------------------

find_package(SDL2 QUIET)
if(TARGET SDL2::SDL2)
    set(LS_SDL2_LIBRARY SDL2::SDL2)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LS_SDL2 REQUIRED IMPORTED_TARGET sdl2)
    set(LS_SDL2_LIBRARY PkgConfig::LS_SDL2)
endif()
message(STATUS "SDL2 target for examples: ${LS_SDL2_LIBRARY}")

# ---------------------------------------------------------------------------
# 5. Examples
# ---------------------------------------------------------------------------
add_subdirectory(examples)
