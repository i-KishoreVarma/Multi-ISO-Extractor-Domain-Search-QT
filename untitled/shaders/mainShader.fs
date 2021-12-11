#version 330

uniform int opacity;

void main()
{
    float cur_op = opacity;
///     gl_FragColor = vec4(0.15, 0.7, 0.13, cur_op/100);
    gl_FragColor = vec4(0.5,0.5,0.5,cur_op/100);
}
