#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "lvgl_safe::lvgl_safe" for configuration "Release"
set_property(TARGET lvgl_safe::lvgl_safe APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(lvgl_safe::lvgl_safe PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/liblvgl_safe.a"
  )

list(APPEND _cmake_import_check_targets lvgl_safe::lvgl_safe )
list(APPEND _cmake_import_check_files_for_lvgl_safe::lvgl_safe "${_IMPORT_PREFIX}/lib/liblvgl_safe.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
