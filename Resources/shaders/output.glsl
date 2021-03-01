#version 450 core

in VSOUT
{
	vec3 worldPos;
	vec3 normal;
	vec2 texCoords;
} fs_in;

out vec4 fragColor;

void main()
{
	fragColor = vec4(vec3(0.0f, 0.0f, 0.9f), 1.0f);
}