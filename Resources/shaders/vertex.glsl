#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 color;
layout(location = 3) in vec2 texCoord;

layout(std140, binding = 0) uniform UBOCamera
{
	mat4 projection; //  64
	mat4 view;		 // 128
	mat4 viewProj;	 // 192
	vec3 camPos;	 // 208
} uboCamera;

struct InstanceMat 
{
	mat4 model;	  //  64
	mat4 modelIT; // 128
};

layout(std430, binding = 2) readonly buffer UBOinstance
{
	InstanceMat matrices[];
};

layout(location = 0) out VSOUT
{
	vec3 worldPos;
	vec3 normal;
	vec4 color;
	vec2 texCoord;
} vs_out;

uniform int instanceIdx = 0;

void main()
{
	vec4 worldPos = matrices[instanceIdx].model * vec4(position, 1.0);
	vs_out.worldPos = worldPos.xyz;
	vs_out.normal	= (matrices[instanceIdx].modelIT * vec4(normal, 1.0)).xyz;
	vs_out.color	= color;
	vs_out.texCoord = texCoord;

	gl_Position = uboCamera.viewProj * worldPos;
}