#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;

uniform mat4 GeometryTransform;
uniform mat4 ViewProjection;

out vec2 frag_uv;

void main()
{
    gl_Position = ViewProjection * GeometryTransform * vec4(Position, 1.0);
    frag_uv = TexCoord;
}
