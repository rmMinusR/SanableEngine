#version 330 core

uniform mat4 ViewProjection;
uniform mat4 GeometryTransform;
uniform vec3 CameraPosition;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUv;

out vec3 normal;
out vec2 uv;
out vec3 camRelCoord;

void main()
{
    vec4 worldPos = GeometryTransform * vec4(aPos, 1);
	camRelCoord = worldPos.xyz - CameraPosition;
    gl_Position = ViewProjection * worldPos;
    normal = (GeometryTransform * vec4(aNormal, 0)).xyz;
    uv = aUv;
}
