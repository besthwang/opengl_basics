cmake_minimum_required(VERSION 3.21)
find_library(
    GLFW3_LIBRARY
    NAMES glfw3
    HINTS ${PROJECT_SOURCE_DIR}/libs/
    PATH_SUFFIXES lib bin build/Release build/Debug
    )

find_path(
    GLFW3_INCLUDE_DIR
    NAMES GLFW/glfw3.h
    HINTS ${PROJECT_SOURCE_DIR}/libs/include/
    )
include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
    Glfw3
    DEFAULT_MSG
    GLFW3_LIBRARY
    GLFW3_INCLUDE_DIR
)
mark_as_advanced(GLFW3_LIBRARY GLFW3_INCLUDE_DIR)
if(NOT TARGET Glfw3::Glfw3)
    add_library(Glfw3::Glfw3 UNKNOWN IMPORTED )
    set_target_properties(Glfw3::Glfw3 PROPERTIES
        IMPORTED_LOCATION "${GLFW3_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES 
        "${GLFW3_INCLUDE_DIR}"
        IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
)
endif()