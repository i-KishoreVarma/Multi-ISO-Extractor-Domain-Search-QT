/**======================================================================
 * ?                                ABOUT
 * @author         :  Kishore Varma
 * @email          :  kishorea@iisc.ac.in
 * @repo           :  
 * @createdOn      :  16/8/2021, 3:02:11 pm 
 * @description    :  Class file for Index buffer object (Element array buffer) in OpenGL, this 
 *                    helper class along with others, helps in writing OpenGL
 *                    programs.  
 *========================================================================**/

#include <GL/glew.h>
#include <vector>
#include "MyLibrary.h"

using namespace std;

IBO::IBO()
{
    ID = 0;
}

/*
 *   Creates Buffer Object
 */
void IBO::create()
{
    glGenBuffers(1, &ID);
    bind();
}

/*
 *   Bind this vertex buffer object to OpenGL context
 */
void IBO::bind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
    ErrorHandler();
}

/*
 *  @param indices - indices information
 *  @param usage {GLenum} - default value is GL_STATIC_DRAW
 *  @see {@link http://docs.gl/gl4/glBufferData}
 */
void IBO::bufferData(vector<GLuint> &indices, GLenum usage)
{
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), &indices[0], usage);
    ErrorHandler();
}

/*
 *  @param indices - indices information
 *  @param offset - offset at which data should be written
 *  @see {@link http://docs.gl/gl4/glBufferData}
 */
void IBO::bufferSubData(vector<GLuint> &indices, GLintptr offset)
{
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, indices.size()*sizeof(GLuint), &indices[0]);
    ErrorHandler();
}

/*
 *   Just sets the current buffer used by OpenGL to 0, there by unbinding 
 *   itself. 
 */
void IBO::unbind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    ErrorHandler();
}

IBO::~IBO()
{
    // unbind();
    // glDeleteBuffers(1, &ID);
    // ErrorHandler();
}
