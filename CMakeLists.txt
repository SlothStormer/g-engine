cmake_minimum_required(VERSION 3.16)
project(g-engine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# Archivos fuente del proyecto
add_executable(main
    src/main.cpp
    src/Shader.cpp
    src/stb_image.cpp
    src/gl.c
)

target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(main PRIVATE
    OpenGL::GL
    glfw
    glm::glm
)
