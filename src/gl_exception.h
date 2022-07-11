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

#ifndef DEMONIA_SRC_GL_EXCEPTION_H_
#define DEMONIA_SRC_GL_EXCEPTION_H_

#define GLFW_INCLUDE_NONE

#include <exception>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace demonia
{

// Exception raised when a GL operation has failed.
class GlException : public std::exception
{
public:
    // Stores the information log for the failed operation.
    GlException(const char info_log[GL_INFO_LOG_LENGTH]);
    
    // Returns the information log for the failed operation.
    virtual const char *what() const throw();

protected:
    char m_info_log[GL_INFO_LOG_LENGTH];
};

// Exception raised when a shader has failed to compile.
class ShaderCompileException : public GlException
{
public:
    // Stores the information log and type of shader.
    ShaderCompileException(const char info_log[GL_INFO_LOG_LENGTH],
                            GLuint shader_type);

    // Returns the type of shader that failed to compile.
    GLuint get_shader_type() const;

protected:
    GLuint m_shader_type;
};

inline GLuint ShaderCompileException::get_shader_type() const
{
    return m_shader_type;
}

// Exception raised when an operation to link a program has failed.
class ProgramLinkException : public GlException
{
public:
    ProgramLinkException(const char info_log[GL_INFO_LOG_LENGTH]);
};

}; // namespace demonia

#endif // DEMONIA_SRC_GL_EXCEPTION_H_
