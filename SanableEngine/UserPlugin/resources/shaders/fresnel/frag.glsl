#version 330 core

in vec3 normal;
in vec2 uv;
in vec3 camRelCoord;

out vec4 fragColor;

#define PI 3.14159

void main()
{
	float cosAngToCamera = abs(dot(normalize(camRelCoord), normalize(normal)));
	//float angToCamera = acos(cosAngToCamera);

    fragColor = vec4(
        vec3(cosAngToCamera),
        //vec3(1-cosAngToCamera),
        //vec3(angToCamera/PI*2),
        1
    );
}
