# Dependency 관련 변수 설정
set(DEP_THIRDPARTY_DIR ${CMAKE_SOURCE_DIR}/libs)
set(DEP_INSTALL_DIR ${PROJECT_BINARY_DIR}/install)
set(DEP_INCLUDE_DIR ${DEP_THIRDPARTY_DIR}/include)
set(DEP_LIB_DIR ${DEP_THIRDPARTY_DIR}/lib)

set(DEP_LIBS ${DEP_LIBS} spdlog$<$<CONFIG:Debug>:d>)
set(DEP_LIBS ${DEP_LIBS} glad)




# message("CMAKE" ${DEP_INCLUDE_DIR})
add_library(imgui
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui.cpp
    imgui/imgui_impl_glfw.cpp
    imgui/imgui_impl_opengl3.cpp
    )

target_include_directories(imgui PRIVATE ${DEP_INCLUDE_DIR})
#add_dependencies(imgui ${DEP_LIST})
set(DEP_INCLUDE_DIR ${DEP_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
set(DEP_LIST ${DEP_LIST} imgui)
set(DEP_LIBS ${DEP_LIBS} imgui)

set(DEP_LIBS ${DEP_LIBS}
    assimp-vc142-mt$<$<CONFIG:Debug>:d>
    zlibstatic$<$<CONFIG:Debug>:d>
    IrrXML$<$<CONFIG:Debug>:d>
    )