cmake_minimum_required(VERSION 3.16)

project(lvgl_safe_examples LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# 1. Locate the unpacked pre-built LVGL Safe package
# ---------------------------------------------------------------------------
# The package is an extracted tarball: <prefix>/include, <prefix>/lib and
# <prefix>/lib/cmake/lvgl_safe. By default it is expected to be unpacked right
# here (this directory then holds include/ and lib/); an unpacked
# lvgl_safe-x.y.z-<platform>/ subdirectory is picked up as well.
#
# Override with -DLS_PACKAGE_PREFIX=/path/to/unpacked/lvgl_safe-x.y.z-<platform>
if(NOT LS_PACKAGE_PREFIX)
    set(LS_PACKAGE_PREFIX_CANDIDATES "${CMAKE_CURRENT_SOURCE_DIR}")

    file(GLOB LS_PACKAGE_PREFIX_SUBDIRS
         "${CMAKE_CURRENT_SOURCE_DIR}/lvgl_safe-*")
    list(SORT LS_PACKAGE_PREFIX_SUBDIRS)
    # Newest version last after the sort, so search it first.
    list(REVERSE LS_PACKAGE_PREFIX_SUBDIRS)
    list(APPEND LS_PACKAGE_PREFIX_CANDIDATES ${LS_PACKAGE_PREFIX_SUBDIRS})

    foreach(candidate IN LISTS LS_PACKAGE_PREFIX_CANDIDATES)
        if(EXISTS "${candidate}/lib/cmake/lvgl_safe/lvgl_safeConfig.cmake")
            set(LS_PACKAGE_PREFIX "${candidate}")
            break()
        endif()
    endforeach()

    if(NOT LS_PACKAGE_PREFIX)
        message(FATAL_ERROR
                "No unpacked LVGL Safe package found in ${CMAKE_CURRENT_SOURCE_DIR}. "
                "Extract lvgl_safe-x.y.z-<platform>.tar.gz here, or pass "
                "-DLS_PACKAGE_PREFIX=<path> to point at the unpacked package.")
    endif()
endif()

get_filename_component(LS_PACKAGE_PREFIX "${LS_PACKAGE_PREFIX}" ABSOLUTE)
set(LS_PACKAGE_PREFIX "${LS_PACKAGE_PREFIX}" CACHE PATH
    "Unpacked LVGL Safe package (headers, static library, CMake config)")

if(NOT EXISTS "${LS_PACKAGE_PREFIX}/lib/cmake/lvgl_safe/lvgl_safeConfig.cmake")
    message(FATAL_ERROR
            "${LS_PACKAGE_PREFIX} does not look like an unpacked LVGL Safe package: "
            "lib/cmake/lvgl_safe/lvgl_safeConfig.cmake is missing.")
endif()

# ---------------------------------------------------------------------------
# 2. Import the library through the CMake config package it ships
# ---------------------------------------------------------------------------

find_package(lvgl_safe REQUIRED CONFIG
             PATHS "${LS_PACKAGE_PREFIX}"
             NO_DEFAULT_PATH)

message(STATUS "Found LVGL Safe ${lvgl_safe_VERSION} in ${LS_PACKAGE_PREFIX}")

# IMPORTANT NOTE: the shipped include/ls_conf.h is authoritative — its values are baked
# into the struct layouts inside liblvgl_safe.a, ls_conf.h must not be changed as all
# functionality of the lib as already made available.

# ---------------------------------------------------------------------------
# 3. SDL2 backend used by the examples (${LS_SDL2_LIBRARY})
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
# 4. Examples
# ---------------------------------------------------------------------------
add_subdirectory(examples)
