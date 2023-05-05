if(TARGET igl::core)
    return()
endif()

include(FetchContent)
FetchContent_Declare(
    libigl
    GIT_REPOSITORY https://github.com/libigl/libigl.git
    GIT_TAG v2.4.0
)

FetchContent_MakeAvailable(libigl)

igl_include(glfw)
igl_include(imgui)

set_target_properties(igl_core PROPERTIES FOLDER "3rdparty/libigl")
set_target_properties(igl_glfw PROPERTIES FOLDER "3rdparty/libigl")
set_target_properties(igl_imgui PROPERTIES FOLDER "3rdparty/libigl")
set_target_properties(igl_opengl PROPERTIES FOLDER "3rdparty/libigl")
set_target_properties(glfw PROPERTIES FOLDER "3rdparty")
set_target_properties(glad PROPERTIES FOLDER "3rdparty")
set_target_properties(imgui PROPERTIES FOLDER "3rdparty")
set_target_properties(imguizmo PROPERTIES FOLDER "3rdparty")
