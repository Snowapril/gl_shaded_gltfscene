#version 450 core
#extension GL_ARB_shading_language_include : require

layout(location = 1) in VSOUT
{
	vec3 worldPos;
	vec3 normal;
	vec2 texCoords;
} fs_in;
layout(location = 0) out vec4 fragColor;

#include "gltf.glsl"

void main()
{
	fragColor = vec4(vec3(0.0f, 0.0f, 0.9f), 1.0f);
}