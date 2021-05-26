struct GltfShadeMaterial
{
	// PbrMetallicRoughness
	vec4  pbrBaseColorFactor; //16

	int   pbrBaseColorTexture; //20
	float pbrMetallicFactor; //24
	float pbrRoughnessFactor; //28 
	int   pbrMetallicRoughnessTexture; //32

	// KHR_materials_pbrSpecularGlossiness
	vec4  khrDiffuseFactor; //48

	vec3  khrSpecularFactor; //60
	int   khrDiffuseTexture; //64
	float khrGlossinessFactor; //68
	int   khrSpecularGlossinessTexture; //72

	int   emissiveTexture; //76
	int   alphaMode; //80
	vec3  emissiveFactor; //92
	float alphaCutoff; // 96
	int   doubleSided; //100

	int   normalTexture; //104
	float normalTextureScale; //108
	int   occlusionTexture; //112
	float occlusionTextureStrength; //116
	int shadingModel;  // 120, 0: metallic-roughness, 1: specular-glossiness 
	int padding[2];
};
