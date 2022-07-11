cmake_minimum_required(VERSION 3.10)

# Project initialisation
project(demonia VERSION 1.0)
set(demonia_VERSION_YEAR 2022)
set(demonia_OUTPUT_NAME demonia)

# Build configuration
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(GL_VERSION_MAJOR 3)
set(GL_VERSION_MINOR 3)
set(OpenGL_GL_PREFERENCE GLVND)

find_package(OpenGL REQUIRED)
find_package(glfw3 ${GL_VERSION_MAJOR}.${GL_VERSION_MINOR} REQUIRED)
find_package(GLEW REQUIRED)

set(CMAKE_CXX_FLAGS_DEBUG "-g")

set(demonia_CODE_SOURCE_DIR ${demonia_SOURCE_DIR}/src)

set(demonia_SOURCES
    demonia.cc
    gl_exception.cc
    gl_handler.cc
    shader.cc)

list(TRANSFORM demonia_SOURCES
    PREPEND "${demonia_CODE_SOURCE_DIR}/")

add_executable(${demonia_OUTPUT_NAME} ${demonia_SOURCES})

set(demonia_CXX_LIBRARIES
    GLEW
    OpenGL
    glfw)

target_link_libraries(${demonia_OUTPUT_NAME} ${demonia_CXX_LIBRARIES})
