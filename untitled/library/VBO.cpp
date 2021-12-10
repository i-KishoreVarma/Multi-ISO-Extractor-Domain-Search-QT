/**======================================================================
 * ?                                ABOUT
 * @author         :  Kishore Varma
 * @email          :  kishorea@iisc.ac.in
 * @repo           :  
 * @createdOn      :  16/8/2021, 3:02:11 pm 
 * @description    :  Class file for vertex buffer object in OpenGL, this 
 *                    helper class along with others, helps in writing OpenGL
 *                    programs.  
 *========================================================================**/

#include <GL/glew.h>
#include <vector>
#include "MyLibrary.h"

using namespace std;

VBO::VBO()
{
    ID = 0;
}

/*
 *   Creates Buffer Object
 */
void VBO::create()
{
    glGenBuffers(1, &ID);
    bind();
}

/*
 *   Bind this vertex buffer object to OpenGL context
 */
void VBO::bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, ID);
    ErrorHandler();
}

/*
 *  @param vertices - vertices information
 *  @param usage {GLenum} - default value is GL_STATIC_DRAW
 *  @see {@link http://docs.gl/gl4/glBufferData}
 */
void VBO::bufferData(vector<Vertex> &vertices, GLenum usage)
{
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), &vertices[0], usage);
    ErrorHandler();
}

/*
 *  @param bufferSize - no of vertices 
 *  @param usage {GLenum} - default value is GL_DYNAMIC_DRAW
 *  @see {@link http://docs.gl/gl4/glBufferData}
 */
void VBO::bufferData(int bufferSize,GLenum usage)
{
    glBufferData(GL_ARRAY_BUFFER, bufferSize*sizeof(Vertex), NULL, usage);
    ErrorHandler();
}

/*
 *  @param vertices - vertices information
 *  @param offset - offset at which data should be written
 *  @see {@link http://docs.gl/gl4/glBufferData}
 */
void VBO::bufferSubData(vector<Vertex> &vertices, GLintptr offset)
{
    glBufferSubData(GL_ARRAY_BUFFER, offset*sizeof(Vertex), vertices.size()*sizeof(Vertex), &vertices[0]);
    ErrorHandler();
}

/*
 *   Just sets the current buffer used by OpenGL to 0, there by unbinding 
 *   itself. 
 */
void VBO::unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    ErrorHandler();
}

VBO::~VBO()
{
    // unbind();
    // glDeleteBuffers(1, &ID);
    // ErrorHandler();
}
