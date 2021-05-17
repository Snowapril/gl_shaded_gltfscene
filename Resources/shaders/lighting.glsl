// KHR_lights_punctual extension.
// see https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_lights_punctual
struct Light
{
    vec3  direction;
    float range;
    vec3  color;
    float intensity;
    vec3  position;
    float innerConeCos;
    float outerConeCos;
    int   type;
    vec2  padding;
};

// Smith Joint GGX
// Note: Vis = G / (4 * NdotL * NdotV)
// see Eric Heitz. 2014. Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs. Journal of Computer Graphics Techniques, 3
// see Real-Time Rendering. Page 331 to 336.
// see https://google.github.io/filament/Filament.md.html#materialsystem/specularbrdf/geometricshadowing(specularg)
float visibilityOcclusion(PBRInfo pbr)
{
    float NdotL = pbr.NdotL;
    float NdotV = pbr.NdotV;
    float alphaRoughnessSq = pbr.alphaRoughness * pbr.alphaRoughness;

    float GGXV = NdotL * sqrt(NdotV * NdotV * (1.0 - alphaRoughnessSq) + alphaRoughnessSq);
    float GGXL = NdotV * sqrt(NdotL * NdotL * (1.0 - alphaRoughnessSq) + alphaRoughnessSq);

    float GGX = GGXV + GGXL;
    if (GGX > 0.0)
    {
        return 0.5 / GGX;
    }
    return 0.0;
}

vec3 getPointShade(vec3 pointToLight, PBRInfo pbr)
{
    // If one of the dot products is larger than zero, no division by zero can happen. Avoids black borders.
    if (pbr.NdotL > 0.0 || pbr.NdotV > 0.0)
    {
        // Calculate the shading terms for the microfacet specular shading model
        vec3  F = specularReflection(pbr);
        float Vis = visibilityOcclusion(pbr);
        float D = microfacetDistribution(pbr);

        // Calculation of analytical lighting contribution
        vec3 diffuseContrib = (1.0 - F) * diffuse(pbr);
        vec3 specContrib = F * Vis * D;

        // Obtain final intensity as reflectance (BRDF) scaled by the energy of the light (cosine law)
        return pbr.NdotL * (diffuseContrib + specContrib);
    }

    return vec3(0.0, 0.0, 0.0);
}

// https://github.com/KhronosGroup/glTF/blob/master/extensions/2.0/Khronos/KHR_lights_punctual/README.md#range-property
float getRangeAttenuation(float range, float distance)
{
    if (range < 0.0)
    {
        // negative range means unlimited
        return 1.0;
    }
    return max(min(1.0 - pow(distance / range, 4.0), 1.0), 0.0) / pow(distance, 2.0);
}

// https://github.com/KhronosGroup/glTF/blob/master/extensions/2.0/Khronos/KHR_lights_punctual/README.md#inner-and-outer-cone-angles
float getSpotAttenuation(vec3 pointToLight, vec3 spotDirection, float outerConeCos, float innerConeCos)
{
    float actualCos = dot(normalize(spotDirection), normalize(-pointToLight));
    if (actualCos > outerConeCos)
    {
        if (actualCos < innerConeCos)
        {
            return smoothstep(outerConeCos, innerConeCos, actualCos);
        }
        return 1.0;
    }
    return 0.0;
}

vec3 applyDirectionalLight(Light light, PBRInfo pbr)
{
    vec3 pointToLight = -light.direction;
    vec3 shade = getPointShade(pointToLight, pbr);
    return light.intensity * light.color * shade;
}

vec3 applyPointLight(Light light, PBRInfo pbr)
{
    vec3  pointToLight = light.position - fs_in.worldPos;
    float distance = length(pointToLight);
    float attenuation = getRangeAttenuation(light.range, distance);
    vec3  shade = getPointShade(pointToLight, pbr);
    return attenuation * light.intensity * light.color * shade;
}

vec3 applySpotLight(Light light, PBRInfo pbr)
{
    vec3  pointToLight = light.position - fs_in.worldPos;
    float distance = length(pointToLight);
    float rangeAttenuation = getRangeAttenuation(light.range, distance);
    float spotAttenuation = getSpotAttenuation(pointToLight, light.direction, light.outerConeCos, light.innerConeCos);
    vec3  shade = getPointShade(pointToLight, pbr);
    return rangeAttenuation * spotAttenuation * light.intensity * light.color * shade;
}