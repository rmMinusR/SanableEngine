#version 330 core

in vec3 normal;
in vec2 uv;

out vec4 fragColor;

void main()
{
    fragColor = vec4(
        vec3(1, 1, 1)*dot(normal, gl_FragCoord.xyz),
        1
    );
}
