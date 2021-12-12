#version 330

layout(location=0) in vec3 Position;
layout(location=1) in vec3 VNormal;

out vec3 FNormal;
out vec3 FragPos;

uniform mat4 mvp;

void main()
{
    gl_Position = mvp*vec4(Position,1.0f);
    FNormal = VNormal;
    FragPos = gl_Position.xyz;
}
