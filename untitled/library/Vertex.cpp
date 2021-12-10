#include "MyLibrary.h"

istream & operator >> (istream &in,Vertex &c)
{
    string s;
    auto readFloat = [&s,&in](){
        in>>s;
        return atof(s.c_str());
    };
    auto readVec3 = [&readFloat](){
        glm::vec3 a;
        a.x = readFloat();
        a.y = readFloat();
        a.z = readFloat();
        return a;
    };
    auto readVec2 = [&readFloat](){
        glm::vec2 a;
        a.x = readFloat();
        a.y = readFloat();
        return a;
    };
    if(c.vertexType&1) c.position = readVec3();
    // if(c.vertexType&2) c.color = readVec3();
    // if(c.vertexType&4) c.texture = readVec2();
    return in;
}

int Vertex::vertexType = MyLibrary::VertexPosition | MyLibrary::VertexColor | MyLibrary::VertexTexture;
