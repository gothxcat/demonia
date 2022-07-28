// Handles GL initialisation and rendering.
// Copyright (C) 2022 Natalie Wiggins
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#define GLFW_INCLUDE_NONE

#include "gl_exception.hh"
#include "gl_handler.hh"
#include "shader.hh"
#include "vertices.hh"

#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace demonia
{

const unsigned int GlHandler::k_gl_version_major = 3;
const unsigned int GlHandler::k_gl_version_minor = 3;
const unsigned int GlHandler::k_initial_window_width = 640;
const unsigned int GlHandler::k_initial_window_height = 480;
const char* GlHandler::k_window_title = "Demonia";

const char* GlHandler::vertex_shader_src =
#include "shaders/color.vert"
;

const char* GlHandler::fragment_shader_src =
#include "shaders/color.frag"
;

GLFWwindow* GlHandler::window;
ShaderProgram* GlHandler::shader_program;
GLuint GlHandler::vbo;
GLuint GlHandler::vao;

// Outputs the string identifying an exception to stderr if the string is not
// empty.
inline void log_exception(std::exception& e) noexcept
{
    if (strlen(e.what()) > 0)
        std::cerr << e.what();
}

int GlHandler::start()
{
    // Pre-initialisation check
    if (window)
    {
        std::cerr << "Failed to start GL handler: a window has already been \
                      created" << std::endl;
        return EXIT_FAILURE;
    }

    // Initialise GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialise GLFW." << std::endl;
        return EXIT_FAILURE;
    }

    glfwSetErrorCallback(glfw_error_callback);

    // Create window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, k_gl_version_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, k_gl_version_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    /// Required for macOS
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    /// Comply with window manager standards
    glfwWindowHint(GLFW_FOCUSED, GL_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GL_FALSE);

    window = glfwCreateWindow(k_initial_window_width, k_initial_window_height,
                              k_window_title, NULL, NULL);

    if (!window)
    {
        std::cerr << "Failed to create GLFW window." << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);

    // Initialise GLEW to load all OpenGL function pointers
    if (!glewInit())
    {
        std::cerr << "Failed to initialise GLEW." << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // Load shaders
    try
    {
        shader_program = new ShaderProgram(vertex_shader_src,
                                           fragment_shader_src);
    }
    catch (ShaderCompileException& e)
    {
        if (e.get_shader_type() == GL_VERTEX_SHADER)
            std::cerr << "Failed to compile vertex shader." << std::endl;
        else if (e.get_shader_type() == GL_FRAGMENT_SHADER)
            std::cerr << "Failed to compile fragment shader." << std::endl;
        else
            std::cerr << "Failed to compile shader." << std::endl;

        log_exception(e);
        return EXIT_FAILURE;
    }
    catch (ProgramLinkException& e)
    {
        std::cerr << "Failed to link shader program." << std::endl;
        log_exception(e);
        return EXIT_FAILURE;
    }

    // Create buffer and array objects
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);
    vertices_color_2d_triangle.use(vao, vbo);
    
    // Pre-render setup
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(1);
    shader_program->use();

    // Render
    while (!glfwWindowShouldClose(window))
    {
        // Clear framebuffer
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // End
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Deinitialise GL
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    delete shader_program;
    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

void GlHandler::glfw_error_callback(int error, const char* description)
    noexcept
{
    std::cerr << "GLFW error " << error << ": " << description << std::endl;
}

void GlHandler::framebuffer_size_callback(GLFWwindow* window, int width,
                                          int height)
{
    glViewport(0, 0, width, height);
}

}; // namespace demonia
