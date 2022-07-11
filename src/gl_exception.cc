// GL-related exceptions.
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

#include "gl_exception.h"

#include <cstring>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace demonia
{

GlException::GlException(const char info_log[GL_INFO_LOG_LENGTH])
{
    strcpy(m_info_log, info_log);
}

const char *GlException::what() const throw()
{
    return m_info_log;
}

ShaderCompileException::ShaderCompileException(
        const char info_log[GL_INFO_LOG_LENGTH],
        GLuint shader_type)
        : GlException(info_log), m_shader_type{shader_type}
{
}

ProgramLinkException::ProgramLinkException(
        const char info_log[GL_INFO_LOG_LENGTH])
        : GlException(info_log)
{
}

}; // namespace demonia
