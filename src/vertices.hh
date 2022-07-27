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
#include <tuple>

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


// Creates a usable set of vertices.
template<typename Attribute0, typename... AttributeRest>
class Vertices
{
public:
    typedef Tuple<Attribute0, AttributeRest...> Vertex; 

    // Creates a set of vertices from a list of attribute data tuples.
    Vertices(std::initializer_list<Vertex> data)
            : m_data{data}
    {
        static_assert(is_vertex_attribute<Attribute0,
                AttributeRest...>::value,
                "attributes must be derived from type 'VertexAttribute'");

        metadata = {Attribute0::metadata, AttributeRest::metadata...};
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
    // vertices to be used in rendering.
    void use(GLuint vao, GLuint vbo) const noexcept
    {
        // Copy data into VBO
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, buffer_size, m_data.data(),
                GL_STATIC_DRAW); 

        // Link and enable attributes
        for (int i = 0, ptr = 0; i < metadata.size(); ++i)
        {
            auto& attrib = metadata[i];
            glVertexAttribPointer(i, attrib.size, attrib.type,
                    attrib.normalized, stride, (void*) (intptr_t) ptr);
            glEnableVertexAttribArray(i);
            ptr += attrib.size * gl_type_sizes.at(attrib.type);
        }

        // Unbind VAO for use
        glBindVertexArray(0);
    }
    
private:
    GLsizei stride = 0;
    size_t buffer_size;
    std::vector<Vertex> m_data;
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
