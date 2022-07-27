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

#ifndef DEMONIA_SRC_GL_HANDLER_HH_
#define DEMONIA_SRC_GL_HANDLER_HH_

#define GLFW_INCLUDE_NONE

#include "shader.hh"

#include <GLFW/glfw3.h>

namespace demonia
{

// Handles GL initialisation and rendering.
typedef class GlHandler
{
public:
    static const unsigned int k_gl_version_major;
    static const unsigned int k_gl_version_minor;
    static const unsigned int k_initial_window_width;
    static const unsigned int k_initial_window_height;
    static const char* k_window_title;
    static const char* vertex_shader_source;
    static const char* fragment_shader_source;

    // Initialises GL and starts graphics rendering; deinitialises GL upon an
    // exit signal. Returns an exit code.
    static int start();

private:
    // Called when GLFW throws an error; logs the error code and description to
    // stderr.
    static void glfw_error_callback(int error, const char* description)
        noexcept;

    // Called when the GL framebuffer is resized; resizes the GL viewport to
    // the new dimensions of the framebuffer.
    static void framebuffer_size_callback(GLFWwindow* window, int width,
                                          int height);

    static GLFWwindow* window;
    static ShaderProgram* shader_program;
    static GLuint vbo; // Vertex Buffer Object
    static GLuint vao; // Vertex Array Object
} GlHandler;

}; // namespace demonia

#endif // DEMONIA_SRC_GL_HANDLER_HH_
