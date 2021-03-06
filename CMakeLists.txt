﻿cmake_minimum_required (VERSION 3.8)
project ("ImageFrequencyAnalysis")

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(LIBS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs")
set(SRCS "${SRC_DIR}/ImageFrequencyAnalysis.cpp")


add_executable(${PROJECT_NAME} ${SRCS} "src/ImageHandler.cpp" "src/imgui/imgui.cpp" "src/imgui/imgui_demo.cpp" "src/imgui/imgui_draw.cpp" "src/imgui/imgui_impl_glfw.cpp" "src/imgui/imgui_impl_opengl3.cpp" "src/imgui/imgui_tables.cpp" "src/imgui/imgui_widgets.cpp" "src/AppContext.hpp" "src/AppContext.cpp" "src/MathUtil.cpp" "src/MathUtil.hpp" "src/FastDCTLee.hpp" "src/FastDCTLee.cpp"  "src/MaskedImageHandler.cpp" "src/MaskedImageHandler.hpp")
target_include_directories(${PROJECT_NAME} PRIVATE "${SRC_DIR}")
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)


# GLFW
set(GLFW_DIR "${LIBS_DIR}/glfw")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
add_subdirectory("${GLFW_DIR}")
target_link_libraries(${PROJECT_NAME} "glfw" "${GLFW_LIBRARIES}")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLFW_DIR}/include")
target_compile_definitions(${PROJECT_NAME} PRIVATE "GLFW_INCLUDE_NONE")

# glad
set(GLAD_DIR "${LIBS_DIR}/glad")
add_library("glad" "${GLAD_DIR}/src/glad.c"  "src/AppContext.hpp" "src/AppContext.cpp"    "src/MaskedImageHandler.cpp" "src/MaskedImageHandler.hpp")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")
target_link_libraries(${PROJECT_NAME} "glad" "${CMAKE_DL_LIBS}")

# glm
find_package(glm REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE "${LIBS_DIR}")