# Every subdirectory holding a CMakeLists.txt is an example.
file(GLOB example_dirs RELATIVE "${CMAKE_CURRENT_LIST_DIR}" "${CMAKE_CURRENT_LIST_DIR}/*")
list(SORT example_dirs)

foreach(example_dir IN LISTS example_dirs)
    if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${example_dir}/CMakeLists.txt")
        add_subdirectory("${example_dir}")
    endif()
endforeach()
