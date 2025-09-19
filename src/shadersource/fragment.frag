#version 460 core

in vec2 FragCoord;
out vec4 FragColor;

uniform sampler2D tex;

void main()
{
    FragColor = vec4(texture(tex, FragCoord));
}