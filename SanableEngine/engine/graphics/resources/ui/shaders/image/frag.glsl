#version 330 core

uniform sampler2D img;
uniform vec4 tintColor;

in vec2 frag_uv;

out vec4 fragColor;

void main()
{    
    fragColor = texture(img, frag_uv) * tintColor;
}
