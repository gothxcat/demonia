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

#ifndef DEMONIA_SRC_VERTEX_H_
#define DEMONIA_SRC_VERTEX_H_

namespace demonia
{

// Stores position and colour attributes for a given vertex.
typedef struct Vertex
{
    float x, y, z;
    float r, g, b;
} Vertex;

}; // namespace demonia

#endif // DEMONIA_SRC_VERTEX_H_
