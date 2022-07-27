// Defines vertex attribute members.
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

#include "vertices.hh"

namespace demonia
{

constexpr VertexAttribute::Metadata Position::metadata{3, GL_FLOAT, false};
constexpr VertexAttribute::Metadata Color::metadata{3, GL_FLOAT, false};

Position::Position(Data data)
        : m_data{data}
{
};

Color::Color(Data data)
        : m_data{data}
{
};

}; // namespace demonia
