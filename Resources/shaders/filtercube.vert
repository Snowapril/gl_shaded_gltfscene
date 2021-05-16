#version 450

layout (location = 0) in vec3 inPos;
layout (location = 0) out vec3 outUVW;

uniform mat4 mvp;

void main() 
{
	outUVW = inPos;
	gl_Position = mvp * vec4(inPos.xyz, 1.0);
}
