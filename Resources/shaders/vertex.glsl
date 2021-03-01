#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

layout(std140) uniform CamMatrices
{
	mat4 projection;
	mat4 view;
	mat4 viewProj;
};

out VSOUT
{
	vec3 worldPos;
	vec3 normal;
	vec2 texCoords;
} vs_out;

uniform mat4 model;

void main()
{
	vs_out.worldPos = (model * vec4(position, 1.0)).xyz;
	vs_out.normal = normal;
	vs_out.texCoords = texCoords;

	gl_Position = viewProj * vec4(vs_out.worldPos, 1.0);
}