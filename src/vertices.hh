// Defines vertex attributes and creates a set of usable vertices.
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

#ifndef DEMONIA_SRC_VERTICES_HH_
#define DEMONIA_SRC_VERTICES_HH_

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <map>
#include <sstream>
#include <stdexcept>
#include <vector>

#include <GL/glew.h>

namespace demonia
{

typedef struct Position
{
    GLfloat x, y, z;
} Position;

typedef struct Color
{
    GLfloat r, g, b;
} Color;

typedef struct PositionColor
{
    Position position;
    Color color;
} PositionColor;

// Arguments to be passed to glVertexAttribPointer for an attribute in a
// vertex.
typedef struct VertexAttributeMetadata
{
    GLint size;
    GLenum type;
    GLboolean normalized;
} VertexAttributeMetadata;

constexpr VertexAttributeMetadata position_metadata
    {3, GL_FLOAT, false};

constexpr VertexAttributeMetadata color_metadata
    {3, GL_FLOAT, false};

// Creates a set of usable vertices.
template<typename VertexData>
class Vertices
{
public:
    // Creates a set of vertices from a list of GL metadata for each attribute
    // per vertex and a list of data for each vertex.
    Vertices(std::initializer_list<VertexAttributeMetadata> metadata,
            std::initializer_list<VertexData> data);

    // Copies vertex data into a Vertex Buffer Object, and links and enables
    // the corresponding attributes in a Vertex Array Object for the set of
    // vertices to be used in rendering.
    void use(GLuint vao, GLuint vbo) const noexcept;

private:
    GLsizei stride = 0;
    size_t buffer_size;
    std::vector<VertexData> m_data;
    std::vector<VertexAttributeMetadata> m_metadata;
};

const std::map<GLenum, size_t> gl_type_sizes
    {
        {GL_BYTE, sizeof(GLbyte)},
        {GL_UNSIGNED_BYTE, sizeof(GLubyte)},
        {GL_SHORT, sizeof(GLshort)},
        {GL_INT, sizeof(GLint)},
        {GL_UNSIGNED_INT, sizeof(GLuint)},
        {GL_HALF_FLOAT, sizeof(GLhalf)},
        {GL_FLOAT, sizeof(GLfloat)},
        {GL_DOUBLE, sizeof(GLdouble)},
        {GL_FIXED, sizeof(GLfixed)},
        {GL_INT_2_10_10_10_REV, sizeof(GLint)},
        {GL_UNSIGNED_INT_2_10_10_10_REV, sizeof(GLuint)},
        {GL_UNSIGNED_INT_10F_11F_11F_REV, sizeof(GLuint)}
    };
 
template<typename VertexData>
Vertices<VertexData>::Vertices(
        std::initializer_list<VertexAttributeMetadata> metadata,
        std::initializer_list<VertexData> data)
        : m_data{data}, m_metadata{metadata}
{
    for (auto &attrib : metadata)
    {
        try
        {
            stride += attrib.size * gl_type_sizes.at(attrib.type);
        }
        catch (std::out_of_range)
        {
            std::stringstream msg;
            msg << "Failed to initialize vertices: no size associated with GL \
                    data type '" << attrib.type << "'."; 
            std::runtime_error(msg.str());
        }
    }

    buffer_size = sizeof(VertexData) * data.size();
}

template<typename VertexData>
void Vertices<VertexData>::use(GLuint vao, GLuint vbo) const noexcept
{
    VertexData buffer[buffer_size];
    std::copy(m_data.begin(), m_data.end(), buffer);

    // Copy data into VBO
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, buffer_size, buffer, GL_STATIC_DRAW); 

    // Link and enable attributes
    for (int i = 0, ptr = 0; i < m_metadata.size(); ++i)
    {
        auto& attrib = m_metadata[i];
        glVertexAttribPointer(i, attrib.size, attrib.type, attrib.normalized,
                stride, (void*) (intptr_t) ptr);
        glEnableVertexAttribArray(i);
        ptr += attrib.size * gl_type_sizes.at(attrib.type);
    }

    glBindVertexArray(0);
}

// Vertices for a 2D triangle with a position attribute only.
const Vertices<Position> vertices_2d_triangle(
    {position_metadata},
    {
        // x, y, z
        {   0.5f, -0.5f, 0.0f }, // bottom right
        {  -0.5f, -0.5f, 0.0f }, // bottom left
        {   0.0f,  0.5f, 0.0f }  // top
    });

// Vertices for a 2D triangle with position and color attributes.
const Vertices<PositionColor> vertices_color_2d_triangle(
    {position_metadata, color_metadata},
    {
        // x, y, z                    r, g, b
        {{  0.5f, -0.5f, 0.0f },    { 1.0f, 0.0f, 0.0f }}, // bottom right
        {{ -0.5f, -0.5f, 0.0f },    { 0.0f, 1.0f, 0.0f }}, // bottom left
        {{  0.0f,  0.5f, 0.0f },    { 0.0f, 0.0f, 1.0f }}  // top
    });

}; // namespace demonia

#endif // DEMONIA_SRC_VERTICES_HH_
