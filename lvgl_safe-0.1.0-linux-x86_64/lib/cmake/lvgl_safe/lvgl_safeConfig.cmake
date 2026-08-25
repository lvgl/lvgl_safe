
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was lvgl_safeConfig.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

# LVGL Safe 0.1.0 - redistributable static library.
#
# Provides the imported target lvgl_safe::lvgl_safe. Its INTERFACE include
# directory carries every public header plus the ls_conf.h the archive was
# compiled against - that header's values are baked into the struct layouts, so
# consumers must use the shipped copy rather than their own.
#
# The archive depends only on the C standard library (memset, strcmp, printf,
# snprintf); there is deliberately no libm, pthread or SDL2 in the link
# interface. SDL2 is used only by the example programs, never by the library.

include("${CMAKE_CURRENT_LIST_DIR}/lvgl_safeTargets.cmake")

check_required_components(lvgl_safe)
