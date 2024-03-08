#version 330 core

uniform sampler2D text;
uniform vec3 textColor;

in vec2 frag_uv;

out vec4 fragColor;

void main()
{    
    fragColor = vec4(1.0f, 1.0f, 1.0f, texture(text, TexCoords).r);
    //fragColor = vec4(textColor, texture(text, TexCoords).r);
}
