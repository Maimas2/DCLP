cmake_minimum_required(VERSION 3.16)

project(DCLP)

set(THREADS_PREFER_PTHREAD_FLAG ON)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "enable X functionality")
add_subdirectory(glfw)
add_subdirectory(clip)

find_package(Freetype REQUIRED)

include_directories("include" "imgui" "imgui/backends")
link_directories("lib")
#include_directories("glfw/include")

#find_package(FindOpenGL)

file(GLOB Source
     "src/*.h"
     "src/*.cpp"
     "include/*.cpp"
     "include/glad/*.c"
     "freetype/src/*.cpp"
)

add_library(imgui
    # Main Imgui files
    imgui/imgui.cpp imgui/imgui_draw.cpp
    imgui/imgui.cpp imgui/imgui_demo.cpp
    imgui/imgui_tables.cpp imgui/imgui_widgets.cpp
    # SDL2+OpenGL-specific files
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp)

add_executable(DCLP WIN32 ${Source})

target_compile_features(DCLP PUBLIC cxx_std_20)

target_link_libraries(DCLP ${FREETYPE_LIBRARIES} imgui)
target_include_directories(DCLP PRIVATE ${FREETYPE_INCLUDE_DIRS})

target_link_libraries(DCLP glfw ${GLFW_LIBRARIES})
target_link_libraries(DCLP freetype)
target_link_libraries(DCLP clip)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")