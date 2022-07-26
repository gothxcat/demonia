// Creates a portable, compiled and linked shader program.
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

#include "gl_exception.hh"
#include "shader.hh"

#include <cstddef>

#include <GL/glew.h>

namespace demonia
{

ShaderProgram::ShaderProgram(const char* vertex_src, const char* fragment_src)
{
    GLuint vertex_shader = compile_shader(vertex_src, GL_VERTEX_SHADER);
    GLuint fragment_shader = compile_shader(fragment_src, GL_FRAGMENT_SHADER);
    id = glCreateProgram();
    glAttachShader(id, vertex_shader);
    glAttachShader(id, fragment_shader);

    glLinkProgram(id);

    glDeleteShader(fragment_shader);
    glDeleteShader(vertex_shader);

    int success;
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success)
    {
        char info_log[GL_INFO_LOG_LENGTH];
        glGetProgramInfoLog(id, GL_INFO_LOG_LENGTH, NULL, info_log);
        throw ProgramLinkException(info_log);
    }
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(id);
}

GLuint ShaderProgram::compile_shader(const char* src, GLenum type)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char info_log[GL_INFO_LOG_LENGTH];
        glGetShaderInfoLog(shader, GL_INFO_LOG_LENGTH, NULL, info_log);
        throw ShaderCompileException(info_log, type);
    }

    return shader;
}

}; // namespace demonia
