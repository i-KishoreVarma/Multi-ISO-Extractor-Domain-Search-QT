/**======================================================================
 * ?                                ABOUT
 * @author         :  Kishore Varma
 * @email          :  kishorea@iisc.ac.in
 * @repo           :  
 * @createdOn      :  16/8/2021, 3:02:11 pm 
 * @description    :  Class file for vertex Array object in OpenGL, this 
 *                    helper class along with others, helps in writing OpenGL
 *                    programs.  
 *========================================================================**/

#include <GL/glew.h>
#include <vector>
#include <iostream>
#include "MyLibrary.h"

using namespace std;

VAO::VAO()
{
    ID = 0;
}

/*
 *   Creates Buffer Object
 */
void VAO::create()
{
    glGenVertexArrays(1, &ID);
    bind();
}


/*
 *   Bind this vertex buffer object to OpenGL context
 */
void VAO::bind()
{
    ErrorHandler();
    glBindVertexArray(ID);
    ErrorHandler();
}

GLuint VAO::getID() const {
    return ID;
}

/*
 *  @param l - Buffer Layout
 */
void VAO::addLayout(BufferLayout &l)
{
    layout = l;
    int offset = 0;
    for(uint i=0;i<layout.arr.size();i++)
    {
        glVertexAttribPointer(i, layout.arr[i].noOfProperties, layout.arr[i].type, layout.arr[i].shouldNormalize,layout.total_size,BUFFER_OFFSET(offset));
        glEnableVertexAttribArray(i);
        offset+=layout.arr[i].noOfProperties*layout.arr[i].sizeOfProperty;
    }
    ErrorHandler();
}

/*
 *   Just sets the current buffer used by OpenGL to 0, there by unbinding 
 *   itself. 
 */
void VAO::unbind()
{
    glBindVertexArray(0);
    ErrorHandler();
}

VAO::~VAO()
{
    // unbind();
    // glDeleteVertexArrays(1,&ID);
    // ErrorHandler();
}
