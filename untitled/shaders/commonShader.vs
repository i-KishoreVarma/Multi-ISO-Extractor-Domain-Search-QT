#version 330

layout(location=0) in vec3 Position;

uniform mat4 mvp;

void main()
{
    gl_Position = mvp*vec4(Position,1.0f);
}
