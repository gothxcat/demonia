// GL vertex shader with color I/O.
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

R""(
#version 330 core
layout (location = 0) in vec3 viPos;
layout (location = 1) in vec3 viColor;
out vec3 voColor;

void main()
{
    gl_Position = vec4(viPos, 1.0);
    voColor = viColor;
}
)""
