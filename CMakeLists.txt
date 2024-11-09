include(FetchContent)
cmake_minimum_required(VERSION 3.17)

project(example)

set(CMAKE_CXX_STANDARD 17)

FetchContent_Declare(
        imgui-glfw-glad-glm
        GIT_REPOSITORY https://github.com/cmmw/imgui-glfw-glad-glm.git
)
FetchContent_MakeAvailable(imgui-glfw-glad-glm)

file(GLOB app_SRC
     "src/*.hpp" 
     "src/*.cpp"
)

add_executable(app ${app_SRC} "src/stb_image.h")

target_link_libraries(
        app
        imgui
        glm
)