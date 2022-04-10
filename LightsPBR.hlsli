#ifndef __SHADER_LIGHTS_PBR__
#define __SHADER_LIGHTS_PBR__

// Gets the RGB value of a pixel with a directional light
float3 directionalLightPBR(Light light, float3 normal, float3 view, float roughness, float metalness, float3 surfaceColor, float3 specularColor)
{
	float3 lightDirection = normalize(light.Direction);
	float diffuse = DiffusePBR(normal, lightDirection);
	float3 specular = MicrofacetBRDF(normal, lightDirection, view, roughness, specularColor);

	float3 balancedDiff = DiffuseEnergyConserve(diffuse, specular, metalness);

	return (balancedDiff * surfaceColor + specular) * light.Intensity * light.Color;
}

// Gets the RGB value of a pixel with a point light
float3 pointLightPBR(Light light, float3 normal, float3 view, float roughness, float metalness, float3 surfaceColor, float3 specularColor, float3 worldPosition)
{
	float3 lightDirection = normalize(light.Position - worldPosition);
	float attenuation = getAttenuation(light.Position, worldPosition, light.Range);
	float diffuse = DiffusePBR(normal, lightDirection);
	float3 specular = MicrofacetBRDF(normal, lightDirection, view, roughness, specularColor);

	float3 balancedDiff = DiffuseEnergyConserve(diffuse, specular, metalness);

	return (balancedDiff * surfaceColor + specular) * attenuation * light.Intensity * light.Color;
}

#endif
