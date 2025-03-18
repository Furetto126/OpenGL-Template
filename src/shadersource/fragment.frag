#version 460 core

in vec2 FragCoord;
out vec4 FragColor;

uniform float frames;

void main()
{
    vec3 col = vec3(FragCoord, 0.0);
    col += vec3(sin(frames*0.01), cos(frames*0.01), cosh(frames*0.01));

    FragColor = vec4(col, 1.0);
}