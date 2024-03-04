#version 330 core

uniform vec4 tint;
in vec2 uv;
//TODO image sampler

out vec4 fragColor;

void main()
{
    fragColor = tint;
}
