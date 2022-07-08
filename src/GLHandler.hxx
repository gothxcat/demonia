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

#ifndef DEMONIA_SRC_GLHANDLER_HXX_
#define DEMONIA_SRC_GLHANDLER_HXX_


#define GLFW_INCLUDE_NONE


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <exception>
#include <initializer_list>
#include <string>
#include <vector>


namespace demonia
{


using namespace std;


// Handles GL initialisation, rendering and user interaction.
class GLHandler
{

public:

    // Starts the GL handler.
    //
    // Initialises GL, starts graphics rendering, and handles user interaction.
    //
    // @return `EXIT_SUCCESS` or `EXIT_FAILURE`.
    //
    static int start();


private:

    // Exception class raised when a GL operation has failed.
    class GLException: public exception
    {

    public:

        // Exception constructor.
        //
        // @param[in] info_log      GL information log for the object which
        //                           failed operation.
        //
        GLException(const char info_log[GL_INFO_LOG_LENGTH]);
        
        virtual const char *what() const throw();


    private:

        char info_log[GL_INFO_LOG_LENGTH];

    };

    typedef struct Vertex
    {
        float x, y, z;
    } Vertex;

    static void glfw_error_callback(int error, const char *description)
            noexcept;

    static void framebuffer_size_callback(GLFWwindow* window, int width,
                                            int height);
    
    // Creates a GL shader.
    //
    // Compiles a GL shader of a given type from its GLSL source code `source`.
    // If the operation fails, throws an exception of type `GLException`.
    //
    // @param[in] source        GLSL source code.
    // @param[in] type          Shader type.
    // @return                  Shader identifier.
    //
    static unsigned int create_shader(string source, GLenum type);

    // Creates a GL shader program.
    //
    // Links a GL shader program from the given compiled shaders. If linking
    // fails, throws an exception of type `GLException`.
    //
    // @param[in] shaders       List of shaders to link.
    // @return                  Identifier of the linked shader program.
    //
    static unsigned int create_shader_program(
            initializer_list<unsigned int> shaders);

    // Deletes the specified GL shaders.
    //
    // @param[in] shaders       List of shaders to delete.
    //
    static void delete_shaders(initializer_list<unsigned int> shaders); 

    static const unsigned int    k_window_initial_width;
    static const unsigned int    k_window_initial_height;
    static GLFWwindow           *window;
    static Vertex                vertices[];
    static unsigned int          indices[];
    static unsigned int          vbo;               // Vertex Buffer Object
    static unsigned int          ebo;               // Element Buffer Object
    static unsigned int          vao;               // Vertex Array Object
    static unsigned int          vertex_shader;
    static unsigned int          fragment_shader;
    static unsigned int          shader_program;
    static const string          vertex_shader_source;
    static const string          fragment_shader_source;

};


}; // namespace demonia


#endif // DEMONIA_SRC_GLHANDLER_HXX_
