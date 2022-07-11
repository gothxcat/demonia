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

#ifndef DEMONIA_SRC_SHADER_H_
#define DEMONIA_SRC_SHADER_H_

#define GLFW_INCLUDE_NONE

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace demonia
{

class ShaderProgram
{
public:
    // Compiles a vertex and fragment shader from their GLSL source code and
    // links them into a program. If a shader fails to compile, throws an
    // exception of type `demonia::ShaderCompileException`. If linking of
    // shaders into a program fails, throws an exception of type
    // `demonia::ProgramLinkException (see `gl_exception.h`).
    ShaderProgram(const char *vertex_src, const char *fragment_src);

    ~ShaderProgram();

    // Use/activate the shader program for GL operations.
    void use() const;

    // Modify uniforms.
    void set_uniform_bool(const char *name, bool value) const;
    void set_uniform_int(const char *name, int value) const;
    void set_uniform_float(const char *name, float value) const;

private:
    unsigned int id;
};

inline void ShaderProgram::use() const
{
    glUseProgram(id);
}

inline void ShaderProgram::set_uniform_bool(const char *name, bool value) const
{
    glUniform1i(glGetUniformLocation(id, name), (int) value);
}

inline void ShaderProgram::set_uniform_int(const char *name, int value) const
{
    glUniform1i(glGetUniformLocation(id, name), value);
}

inline void ShaderProgram::set_uniform_float(const char *name, float value) const
{
    glUniform1i(glGetUniformLocation(id, name), value);
}

// Compiles a shader of a specified type from its GLSL source code. Returns the
// identifier of the shader. If compilation fails, throws an exception of type
// `demonia::ShaderCompileException` (see `gl_exception.h`).
unsigned int compile_shader(const char *src, GLenum type);

}; // namespace demonia

#endif // DEMONIA_SRC_SHADER_H_
