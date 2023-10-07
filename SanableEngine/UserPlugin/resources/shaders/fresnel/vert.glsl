#version 330 core

uniform mat4 matVP; //@bind ViewProjection
uniform mat4 matGeo; //@bind Transform
uniform vec3 camPos; //@bind CameraPosition

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUv;

out vec3 normal;
out vec2 uv;
out vec3 camRelCoord;

void main()
{
	camRelCoord = (matGeo * vec4(aPos, 1)).xyz - camPos;
    gl_Position = matVP * matGeo * vec4(aPos, 1);
    normal = aNormal;
    uv = aUv;
}
