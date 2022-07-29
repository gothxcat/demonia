// Vertex attributes and objects for creating usable sets of vertices.
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

#include "tuple.hh"

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include <GL/glew.h>

namespace demonia
{

typedef struct VertexAttribute
{
    typedef struct Metadata
    {
        GLint size;
        GLenum type;
        GLboolean normalized;
    } Metadata;

    static const Metadata metadata;
} VertexAttribute;

typedef struct Position Position;
struct Position : public VertexAttribute
{
    typedef struct Data
    {
        GLfloat x, y, z;
    } Data;

    Position(Data data);

    static const Metadata metadata;
    Data m_data;
};

typedef struct Color Color;
struct Color : public VertexAttribute
{
    typedef struct Data
    {
        GLfloat r, g, b;
    } Data;

    Color(Data data);

    static const Metadata metadata;
    Data m_data;
};

template<typename... Ts>
struct is_vertex_attribute;

template<typename T, typename... Ts>
struct is_vertex_attribute<T, Ts...>
{
    static const bool value = std::is_base_of<VertexAttribute, T>::value
        && is_vertex_attribute<Ts...>::value;
};

template<typename T>
struct is_vertex_attribute<T>
{
    static const bool value = std::is_base_of<VertexAttribute, T>::value;
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

// Expected usage patterns of data stores. Castable to GLenum.
enum class GlUsage
{
    STREAM_DRAW = GL_STREAM_DRAW,
    STREAM_READ = GL_STREAM_READ,
    STREAM_COPY = GL_STREAM_COPY,
    STATIC_DRAW = GL_STATIC_DRAW,
    STATIC_READ = GL_STATIC_READ,
    STATIC_COPY = GL_STATIC_COPY,
    DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
    DYNAMIC_READ = GL_DYNAMIC_READ,
    DYNAMIC_COPY = GL_DYNAMIC_COPY
};

// Creates a usable set of vertices.
template<typename AttributeFirst, typename... AttributeRest>
class Vertices
{
public:
    typedef Tuple<AttributeFirst, AttributeRest...> Vertex;

    // Creates a set of vertices from a list of attribute data tuples and
    // an optional list of indices describing the order in which the vertices
    // should be rendered.
    Vertices(std::initializer_list<Vertex> data,
             std::initializer_list<GLuint> indices = {})
            : m_data{data}, m_indices{indices}
    {
        static_assert(is_vertex_attribute<AttributeFirst, AttributeRest...
                                         >::value,
                      "attributes must be derived from type 'VertexAttribute'");

        metadata = {AttributeFirst::metadata, AttributeRest::metadata...};
        for (auto& attrib : metadata)
        {
            try
            {
                stride += attrib.size * gl_type_sizes.at(attrib.type);
            }
            catch (std::out_of_range)
            {
                std::stringstream msg;
                msg << "Failed to initialize vertices: no size associated with \
                        GL data type '" << attrib.type << "'.";
                std::runtime_error(msg.str());
            }
        }

        buffer_size = sizeof(Vertex) * data.size();
    }

    // Copies vertex data into a Vertex Buffer Object, and links and enables
    // the corresponding attributes in a Vertex Array Object for the set of
    // vertices to be used in rendering. The expected usage pattern of the
    // data store may optionally be specified.
    void use(GLuint vao, GLuint vbo, GlUsage usage = USAGE_DEFAULT) const
        noexcept
    {
        // Copy data into VBO
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, buffer_size, m_data.data(),
                     static_cast<GLenum>(usage));

        // Link and enable attributes
        glBindVertexArray(vao);
        for (int i = 0, ptr = 0; i < metadata.size(); ++i)
        {
            auto& attrib = metadata[i];
            glVertexAttribPointer(i, attrib.size, attrib.type,
                                  attrib.normalized, stride,
                                  (void*) static_cast<intptr_t>(ptr));
            glEnableVertexAttribArray(i);
            ptr += attrib.size * gl_type_sizes.at(attrib.type);
        }

        // Unbind VAO for use
        glBindVertexArray(0);
    }

    // Copies vertex data into a Vertex Buffer Object, links and enables
    // the corresponding attributes in a Vertex Array Object for the set of
    // vertices to be used in rendering, and stores the indices describing
    // the rendering order in an Element Buffer Object. The expected usage
    // pattern of the data store may optionally be specified.
    void use(GLuint vao, GLuint vbo, GLuint ebo, GlUsage usage = USAGE_DEFAULT)
        const noexcept
    {
        use(vao, vbo, usage);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indices),
                     m_indices.data(), static_cast<GLenum>(usage));
    }

    static constexpr GlUsage USAGE_DEFAULT = GlUsage::STATIC_DRAW;

private:
    GLsizei stride = 0;
    size_t buffer_size;
    std::vector<Vertex> m_data;
    std::vector<GLuint> m_indices;
    std::vector<VertexAttribute::Metadata> metadata;
};

// Vertices for a 2D triangle with only a position attribute.
const Vertices<Position> vertices_2d_triangle({
        //  x, y, z
        {{{  0.5f, -0.5f, 0.0f }}}, // bottom right
        {{{ -0.5f, -0.5f, 0.0f }}}, // bottom left
        {{{  0.0f,  0.5f, 0.0f }}}  // top
    });

// Vertices for a 2D triangle with position and color attributes.
const Vertices<Position, Color> vertices_color_2d_triangle({
        //  x, y, z                    r, g, b
        {{{  0.5f, -0.5f, 0.0f }},  {{ 1.0f, 0.0f, 0.0f }}}, // bottom right
        {{{ -0.5f, -0.5f, 0.0f }},  {{ 0.0f, 1.0f, 0.0f }}}, // bottom left
        {{{  0.0f,  0.5f, 0.0f }},  {{ 0.0f, 0.0f, 1.0f }}}  // top
    });

}; // namespace demonia

#endif // DEMONIA_SRC_VERTICES_HH_
