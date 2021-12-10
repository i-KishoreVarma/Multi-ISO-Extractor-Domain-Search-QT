#version 410
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

struct Vertex {
    vec3 worldPosition;
    vec3 finalPosition;
    vec3 color;
    vec2 textureCoord;
    vec3 normalsInfo;
};

in Vertex vertices[];

out Vertex vertex;

vec3 calcNormal(vec3 a,vec3 b,vec3 c)
{
    /*
        a --> c
        |
        \/
        b
    */
    return cross(b-a,c-a); 
}

void main() {   
    for(int i=0;i<gl_in.length();i+=3)
    {
        for(int j=0;j<3;j++)
        {
            // 1st vertex
            gl_Position = gl_in[i+j].gl_Position;
            vertex = vertices[i+j];
            // vertex.normalsInfo = calcNormal(vertices[i+j%3].worldPosition,vertices[i+(j+1)%3].worldPosition,vertices[i+(j+2)%3].worldPosition);
            vertex.normalsInfo = normalize(vertex.normalsInfo);
            EmitVertex();
        }
        EndPrimitive();
    } 
}  