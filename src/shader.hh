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

#ifndef DEMONIA_SRC_SHADER_HH_
#define DEMONIA_SRC_SHADER_HH_

#include <GL/glew.h>

namespace demonia
{

// Creates a portable, compiled and linked shader program.
class ShaderProgram
{
public:
    // Compiles a vertex and fragment shader from their GLSL source code and
    // links them into a program. If a shader fails to compile, throws a
    // ShaderCompileException. If linking of shaders into a program fails,
    // throws a ProgramLinkException.
    ShaderProgram(const char* vertex_src, const char* fragment_src);

    ~ShaderProgram();

    // Use/activate the shader program for GL operations.
    void use() const noexcept;

    // Modifies a uniform value shared between shaders.
    template<typename T>
    void set_uniform(const char* name, T value) const noexcept;

private:
    // Compiles a shader of a given type from its GLSL source code and returns
    // its identifier.
    static GLuint compile_shader(const char *src, GLenum type);

    unsigned int id;
};

inline void ShaderProgram::use() const noexcept
{
    glUseProgram(id);
}

template<typename T>
inline void ShaderProgram::set_uniform(const char *name, T value) const
        noexcept
{
    glUniform1i(glGetUniformLocation(id, name), value);
}

}; // namespace demonia

#endif // DEMONIA_SRC_SHADER_HH_
