#version 330 core

uniform mat4 ViewProjection;
uniform mat4 GeometryTransform;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUv;

out vec2 uv;

void main()
{
    vec4 worldPos = GeometryTransform * vec4(aPos, 1);
    gl_Position = ViewProjection * worldPos;
    uv = aUv;
}
