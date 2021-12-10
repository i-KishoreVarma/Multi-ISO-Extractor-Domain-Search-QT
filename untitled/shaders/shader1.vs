#version 420

layout(location=0) in vec3 Position;

uniform mat4 gWorld; 
out vec4 theColor; 

vec4 arr[7] = {
    vec4(0,0.741,0.9116833333333333,0.95),
    vec4(0.125,0.5192000000000001,0.7236533333333334,0.88),
    vec4(0.42,0.246,0.3639999999999999,0.6),
    vec4(0.5,0.06660000000000002,0.11210999999999985,0.37),
    vec4(0.5,0.17359166666666662,0.37,0.03329999999999999),
    vec4(0.65,0.2943791557480731,0.59,0.21239999999999998),
    vec4(1,0.8486949532130028,1,0.71),
};

vec4 getColor(float z){
    for(int i=0;i<7;i++)
    {
        if(z<=arr[i][0]) return arr[i];
    }
    return vec4(1.0,1.0,1.0,1.0);
}

void main()
{
    gl_Position = gWorld*vec4(Position,1.0f);
    vec4 tmp = getColor(Position.z);
    theColor = vec4(tmp[1],tmp[2],tmp[3],1.0);
}