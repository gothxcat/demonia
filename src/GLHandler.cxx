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


#include "GLHandler.hxx"
#include "config.hxx"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <iostream>
#include <vector>


namespace demonia
{


using namespace std;


GLFWwindow          *GLHandler::window;
unsigned int         GLHandler::vbo;
unsigned int         GLHandler::ebo;
unsigned int         GLHandler::vao;
unsigned int         GLHandler::vertex_shader;
unsigned int         GLHandler::fragment_shader;
unsigned int         GLHandler::shader_program;

const unsigned int GLHandler::k_window_initial_width  = 640;
const unsigned int GLHandler::k_window_initial_height = 480;

const string GLHandler::vertex_shader_source =
#include "shaders/vertex.glsl"
;

const string GLHandler::fragment_shader_source =
#include "shaders/fragment.glsl"
;

GLHandler::Vertex GLHandler::vertices[] = {
    {  0.5f,  0.5f, 0.0f }, // top right
    {  0.5f, -0.5f, 0.0f }, // bottom right
    { -0.5f, -0.5f, 0.0f }, // bottom left
    { -0.5f,  0.5f, 0.0f }  // top left
};

unsigned int GLHandler::indices[] = {
    0, 1, 3,    // first triangle
    1, 2, 3     // second triangle
};


int GLHandler::start()
{
    if (window)
    {
        cerr << "Failed to start handler: "
                "base window has already been created." << endl;
        return EXIT_FAILURE; 
    }


    if (!glfwInit())
    {
        cerr << "Failed to initialise GLFW." << endl;
        return EXIT_FAILURE;
    }


    glfwSetErrorCallback(glfw_error_callback);
    

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, config::k_gl_version_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, config::k_gl_version_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        // required for macOS
    glfwWindowHint(GLFW_FOCUSED, GL_FALSE);
        // required to comply with window manager standards
    
    const char *title = config::k_project_name.c_str();
    window = glfwCreateWindow(  k_window_initial_width,
                                k_window_initial_height,
                                title,
                                NULL,
                                NULL);

    if (!window)
    {
        cerr << "Failed to create GL window." << endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);


    if (!glewInit())
    {
        cerr << "Failed to initialise GLEW." << endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE; 
    }


    try
    {
        vertex_shader = create_shader(vertex_shader_source, GL_VERTEX_SHADER);
    }
    catch (GLException& e)
    {
        cerr << "GL error: Failed to compile vertex shader." << endl
             << e.what() << endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }


    try
    {
        fragment_shader = create_shader(fragment_shader_source,
                                        GL_FRAGMENT_SHADER);
    }
    catch (GLException& e)
    {
        cerr << "GL error: Failed to compile fragment shader." << endl
             << e.what() << endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }


    try
    {
        shader_program = create_shader_program({vertex_shader,
                                                fragment_shader});
    }
    catch (GLException& e)
    {
        cerr << "GL error: Failed to link shader program." << endl
             << e.what() << endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    delete_shaders({vertex_shader, fragment_shader});


    // Create objets
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenVertexArrays(1, &vao);

    // Bind VAO
    glBindVertexArray(vao);

    // Copy vertex data into VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Copy index data into EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Link vertex attributes
    //
    // @param[in] index         Attribute to be modified (position = 0 in
    //                           vertex.glsl).
    // @param[in] size          Components per attribute (vec3).
    // @param[in] type          Component type (float).
    // @param[in] normalized    Whether fixed-point values should be normalized
    //                           (false, as we are using floats).
    // @param[in] stride        Byte offset between attributes.
    // @param[in] pointer       Offset of first component.
    //
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                            (void *) 0);
    glEnableVertexAttribArray(0);


    int vp_initial_width, vp_initial_height;
    glfwGetFramebufferSize(window, &vp_initial_width, &vp_initial_height);
    glViewport(0, 0, vp_initial_width, vp_initial_height);


    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    

    glfwSwapInterval(1);


    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(shader_program);

        float timeValue = glfwGetTime();
        float redValue = sin(timeValue) / 2.0f + 0.5f;
        float greenValue = sin(timeValue + 90.0f) / 2.0f + 0.5f;
        float blueValue = sin(timeValue + 180.0f) / 2.0f + 0.5f;
        int vertexColourLocation = glGetUniformLocation(shader_program,
                                                        "uniformFragColour");
        glUniform4f(vertexColourLocation, redValue, greenValue, blueValue,
                    1.0f);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }


    glfwDestroyWindow(window);
    glfwTerminate();

    
    return EXIT_SUCCESS;
}


GLHandler::GLException::GLException(const char info_log[GL_INFO_LOG_LENGTH])
{
    strcpy(this->info_log, info_log);
}


const char *GLHandler::GLException::what() const throw()
{
    return this->info_log;
}


void GLHandler::glfw_error_callback(int error, const char *description)
        noexcept
{
    cerr << "GLFW error " << error << ": " << description << endl;
}


void GLHandler::framebuffer_size_callback(GLFWwindow* window, int width,
                                            int height)
{
    glViewport(0, 0, width, height);
}


unsigned int GLHandler::create_shader(string source, GLenum type)
{
    int success;
    char info_log[GL_INFO_LOG_LENGTH];

    unsigned int shader = glCreateShader(type);

    const char *source_c_str = source.c_str();
    glShaderSource(shader, 1, &source_c_str, NULL);

    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, GL_INFO_LOG_LENGTH, NULL, info_log);
        throw GLException(info_log);
    }

    return shader;
}



unsigned int GLHandler::create_shader_program(
        initializer_list<unsigned int> shaders)
{
    int success;
    char info_log[GL_INFO_LOG_LENGTH];

    unsigned int program = glCreateProgram();

    for (auto shader : shaders)
        glAttachShader(program, shader);

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, GL_INFO_LOG_LENGTH, NULL, info_log);
        throw GLException(info_log);
    }

    return program;
}


void GLHandler::delete_shaders(initializer_list<unsigned int> shaders)
{
    for (auto shader : shaders)
        glDeleteShader(shader);
}


} // namespace demonia
