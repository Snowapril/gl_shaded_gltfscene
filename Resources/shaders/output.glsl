#version 450 core
#extension GL_ARB_shading_language_include : require

//! This PBR shader largely referenced on two sources.
//! 1. https://github.com/SaschaWillems/Vulkan-glTF-PBR/blob/master/data/shaders/pbr_khr.frag
//! 2. https://github.com/nvpro-samples/vk_shaded_gltfscene/blob/master/shaders/metallic-roughness.frag

layout(location = 1) in VSOUT
{
	vec3 worldPos;
	vec3 normal;
	vec4 color;
	vec2 texCoord;
} fs_in;

layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform UBOCamera
{
	mat4 projection; //  64
	mat4 view;		 // 128
	mat4 viewProj;	 // 192
	vec3 camPos;	 // 208
} uboCamera;

layout(std140, binding = 1) uniform UBOScene
{
	vec4  lightDir;		 // 16
	float lightRadiance; // 20
	float exposure;		 // 24
	float gamma;		 // 28
	int   materialMode;	 // 32
	int   tonemap;		 // 36
	float envIntensity;	 // 40
} uboScene;

#define MAX_TEXTURES 10
uniform sampler2D textures[MAX_TEXTURES];

#include gltf.glsl
uniform GltfShadeMaterial material;
#include utils.glsl

#define PBR_METALLIC_ROUGHNESS_MODEL  0
#define PBR_SPECULAR_GLOSSINESS_MODEL 1

void main()
{
	vec3 diffuseColor			= vec3(0.0);
	vec3 specularColor			= vec3(0.0);
	vec4 baseColor				= vec4(0.0, 0.0, 0.0, 1.0);
	vec3 f0						= vec3(0.04);
	float perceptualRoughness;
	float metallic;

	//! The albedo maybe defined from a base texture or a flat color
	if (material.alphaMode > 0)
	{
		baseColor = material.pbrBaseColorFactor;
		if (material.pbrBaseColorTexture > -1)
			baseColor *= SRGBtoLinear(texture(textures[material.pbrBaseColorTexture], fs_in.texCoord), 2.2);
	}

	if (material.shadingModel == PBR_METALLIC_ROUGHNESS_MODEL)
	{
		perceptualRoughness = material.pbrRoughnessFactor;
		metallic = material.pbrMetallicFactor;
		//! Roughness is stored in the 'g' channel, metallic is stored in the 'b' channel
		//! This layout intentionally reserves the 'r' channel for (optional) occlusion map data
		if (material.pbrMetallicRoughnessTexture > -1)
		{
			vec4 mrSample = texture(textures[material.pbrMetallicRoughnessTexture], fs_in.texCoord);
			perceptualRoughness *= mrSample.g;
			metallic *= mrSample.b;
		}
		else
		{
			perceptualRoughness = clamp(perceptualRoughness, MIN_ROUGHNESS, 1.0);
			metallic = clamp(metallic, 0.0, 1.0);
		}

		baseColor = material.pbrBaseColorFactor;
		if (material.pbrBaseColorTexture > -1)
			baseColor *= SRGBtoLinear(texture(textures[material.pbrBaseColorTexture], fs_in.texCoord), 2.2);
	}

	if (material.shadingModel == PBR_SPECULAR_GLOSSINESS_MODEL)
	{
		if (material.pbrMetallicRoughnessTexture > -1)
		{
			perceptualRoughness = 1.0 - texture(textures[material.pbrMetallicRoughnessTexture], fs_in.texCoord).a;
		}
		else
		{
			perceptualRoughness = 0.0;
		}

		vec4 diffuse = SRGBtoLinear(texture(textures[material.khrDiffuseTexture], fs_in.texCoord), 2.2);
		vec3 specular = SRGBtoLinear(texture(textures[material.pbrMetallicRoughnessTexture], fs_in.texCoord), 2.2).rgb;

		float maxSpecular = max(max(specular.r, specular.g), specular.b);

		//! Convert metallic value from sepcular glossiness inputs
		metallic = convertMetallic(diffuse.rgb, specular, maxSpecular);

		vec3 baseColorDiffusePart = diffuse.rgb * ((1.0 - maxSpecular) / (1.0 - MIN_ROUGHNESS) / max(1 - metallic, EPSILON)) * material.khrDiffuseFactor.rgb;
		vec3 baseColorSpecularPart = specular - (vec3(MIN_ROUGHNESS) * (1.0 - metallic) * (1.0 / max(metallic, EPSILON))) * material.khrSpecularFactor.rgb;
		baseColor = vec4(mix(baseColorDiffusePart, baseColorSpecularPart, metallic * metallic), diffuse.a);
	}

	baseColor *= fs_in.color;
	diffuseColor = baseColor.rgb * (vec3(1.0) - f0) * (1.0 - metallic);
	specularColor = mix(f0, baseColor.rgb, metallic);

	//! Roughness is authored as perceptual roughness; as is convention
	//! convert to material roughness by squaring the perceptual roughness [2].
	float alphaRoughness = perceptualRoughness * perceptualRoughness;
	
	//! For typical incident reflectance range (between 4% to 100%) set the grazing reflectance to 100% for typical fresnel effect.
	//! For very low reflectance range on highly diffuse objects (below 4%), incrementally reduce grazing reflectance to 0%;
	float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);
	vec3 specularEnvironmentR0 = specularColor.rgb;
	vec3 specularEnvironmentR90 = vec3(clamp(reflectance * 50.0, 0.0, 1.0));

	//! Lighting start
	vec3 color = vec3(0.0);
	vec3 normal = material.normalTexture > -1 ? getNormal(material.normalTexture) :
												normalize(fs_in.normal);
	vec3 view = normalize(uboCamera.camPos - fs_in.worldPos);
	vec3 light = normalize(uboScene.lightDir.xyz);
	vec3 h = normalize(light + view);
	vec3 reflection = -normalize(reflect(view, normal));
	reflection.y *= -1.0f;

	float NdotL = clamp(dot(normal, light),		0.001, 1.0);
	float NdotV = clamp(abs(dot(normal, view)), 0.001, 1.0);
	float NdotH = clamp(dot(normal, h),			  0.0, 1.0);
	float LdotH = clamp(dot(light, h),			  0.0, 1.0);
	float VdotH = clamp(dot(view, h),			  0.0, 1.0);

	//! TODO : start from here.
	//! https://github.com/SaschaWillems/Vulkan-glTF-PBR/blob/master/data/shaders/pbr_khr.frag

	fragColor = vec4(gammaCorrection(diffuseColor, 2.2), 1.0);
}