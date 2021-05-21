// This PBR shader largely referenced on two sources.
// 1. https://github.com/SaschaWillems/Vulkan-glTF-PBR/blob/master/data/shaders/pbr_khr.frag
// 2. https://github.com/nvpro-samples/vk_shaded_gltfscene/blob/master/shaders/metallic-roughness.frag

const float M_PI = 3.141592653589793;
const float EPSILON = 1e-6;
const float MIN_ROUGHNESS = 0.04;

vec3 gammaCorrection(vec3 color, float gamma)
{
	return pow(color, vec3(1.0 / gamma));
}

//! Gets metallic factor from specular glossiness workflow inputs
float convertMetallic(vec3 diffuse, vec3 specular, float maxSpecular)
{
	float perceivedDiffuse = sqrt(0.299 * diffuse.r * diffuse.r + 
								  0.587 * diffuse.g * diffuse.g +
								  0.114 * diffuse.b * diffuse.b);
	float perceivedSpecular = sqrt(0.299 * specular.r * specular.r + 
								   0.587 * specular.g * specular.g +
								   0.114 * specular.b * specular.b);
	if (perceivedSpecular < MIN_ROUGHNESS) 
	{
		return 0.0;
	}
	float a = MIN_ROUGHNESS;
	float b = perceivedDiffuse * (1.0 - maxSpecular) / (1.0 - MIN_ROUGHNESS) +
			  perceivedSpecular - 2.0 * MIN_ROUGHNESS;
	float c = MIN_ROUGHNESS - perceivedSpecular;
	float D = max(b * b - 4.0 * a * c, 0.0);
	return clamp((-b + sqrt(D)) / (2.0 * a), 0.0, 1.0);
}

//! Find the normal for this fragment, pulling either from a predefined normal map
//! or from the interpolated mesh normal and tangent attributes.
//! See http://www.thetenthplanet.de/archives/1180
vec3 getNormal(int normalTexture)
{
	if (normalTexture > -1)
	{
		vec3 tangentNormal = texture(textures[normalTexture], fs_in.texCoord).xyz;
		if (length(tangentNormal) <= 0.01)
			return fs_in.normal;
		tangentNormal = tangentNormal * 2.0 - 1.0;
		vec3 q1 = dFdx(fs_in.worldPos);
		vec3 q2 = dFdy(fs_in.worldPos);
		vec2 st1 = dFdx(fs_in.texCoord);
		vec2 st2 = dFdy(fs_in.texCoord);

		vec3 N = normalize(fs_in.normal);
		vec3 T = normalize(q1 * st2.t - q2 * st1.t);
		vec3 B = -normalize(cross(N, T));
		mat3 TBN = mat3(T, B, N);

		return normalize(TBN * tangentNormal);
	}
	else
		return normalize(fs_in.normal);
}