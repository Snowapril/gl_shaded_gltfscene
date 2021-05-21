#version 450
#extension GL_ARB_shading_language_include : require

#include tonemapping.glsl

layout(location = 0) in vec3 inWorldPosition;
layout(location = 0) out vec4 outColor;

layout (binding = 0) uniform sampler2D samplerEnv;

layout(std140, binding = 1) uniform UBOScene
{
	vec4  lightDir;		 // 16
	float lightRadiance; // 20
	float exposure;		 // 24
	float gamma;		 // 28
	int   materialMode;	 // 32
	float envIntensity;	 // 40
} uboScene;

const float ONE_OVER_PI = 0.3183099;

vec2 get_spherical_uv(vec3 v)
{
    float gamma = asin(-v.y);
    float theta = atan(v.z, v.x);

    vec2 uv = vec2(theta * ONE_OVER_PI * 0.5, gamma * ONE_OVER_PI) + 0.5;
    return uv;
}

void main()
{
  vec2 uv    = get_spherical_uv(normalize(inWorldPosition));
  vec4 color = texture(samplerEnv, uv);

  color    = tonemap(color, uboScene.gamma, uboScene.exposure);
  outColor = color;
}