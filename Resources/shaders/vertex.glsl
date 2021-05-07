#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

layout(std140, binding = 0) uniform UBOCamera
{
	mat4 projection;
	mat4 view;
	mat4 viewProj;
};

struct InstanceMat 
{
	mat4 model;
	mat4 modelIT;
};

layout(std430, binding = 1) readonly buffer UBOinstance
{
	InstanceMat matrices[];
};

layout(location = 0) out VSOUT
{
	vec3 worldPos;
	vec3 normal;
	vec2 texCoords;
} vs_out;

uniform int instanceIdx = 0;

void main()
{
	vs_out.worldPos = position;
	vs_out.normal = normal;
	vs_out.texCoords = texCoords;

	gl_Position = viewProj * vec4(vs_out.worldPos, 1.0);
}