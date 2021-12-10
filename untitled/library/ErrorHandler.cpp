#include "ErrorHandler.h"
#include <iostream>
GLenum errorCode;


void ErrorHandler(){
    errorCode = glGetError();

    if (errorCode != GL_NO_ERROR)
    {
        //error received
        std::cerr<<"OpenGL rendering error "<< errorCode<<"\n";
    }
}