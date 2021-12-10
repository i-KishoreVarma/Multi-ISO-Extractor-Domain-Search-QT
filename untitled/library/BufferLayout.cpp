/**======================================================================
 * ?                                ABOUT
 * @author         :  Kishore Varma
 * @email          :  kishorea@iisc.ac.in
 * @repo           :  
 * @createdOn      :  16/8/2021, 3:02:11 pm 
 * @description    :  Class file for Buffer Layout.Helps in specifying buffer
 *                    layout with ease.This along with other classes helps in 
 *                    writing OpenGL programs.  
 *========================================================================**/

#include <GL/glew.h>
#include <vector>
#include <iostream>
// #include <type_traits>
#include "MyLibrary.h"

using namespace std;

BufferLayout::BufferLayout()
{
    total_size = 0;
}

BufferLayout::~BufferLayout()
{
}
/**
 * @param size - Specifies the number of components per generic vertex attribute. 
 *               Must be 1, 2, 3, 4. Additionally, the symbolic constant GL_BGRA is 
 *               accepted by glVertexAttribPointer. The initial value is 4.
 * @param type - used internally , passed as function template parameter.Specifies the
 *               data type of each component in the array. The symbolic constants 
 *               GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, and 
 *               GL_UNSIGNED_INT are accepted by glVertexAttribPointer and 
 *               glVertexAttribIPointer. 
 * @param normalized - For glVertexAttribPointer, specifies whether fixed-point data 
 *               values should be normalized (GL_TRUE) or converted directly as fixed-point 
 *               values (GL_FALSE) when they are accessed.
 */
template<>
void BufferLayout::addAttrib<float>(GLint noOfProperties,GLboolean shouldNormalize)
{
    BufferLayoutData tmp;
    tmp.noOfProperties = noOfProperties;
    tmp.shouldNormalize = shouldNormalize;
    tmp.type = GL_FLOAT;
    tmp.sizeOfProperty = sizeof(float);
    arr.push_back(tmp);
    total_size+=noOfProperties*sizeof(float);
}

void BufferLayout::print()
{
    for(auto &elm:arr)
    {
        cout<<"\n-----\n";
        cout<<"\tnoOfProperties : "<<elm.noOfProperties<<'\n';
        cout<<"\ttype : "<<elm.type<<'\n';
        cout<<"\tsizeOfProperty : "<<elm.sizeOfProperty<<'\n';
        cout<<"\n-----\n";
    }
}