#ifndef __SHADER_LIGHTS__
#define __SHADER_LIGHTS__

#define LIGHT_TYPE_DIRECTIONAL	0
#define LIGHT_TYPE_POINT		1
#define LIGHT_TYPE_SPOT			2

// Struct representing light data
// - This should match Lights.h
struct Light
{
	int		Type;
	float3	Direction;

	float	Range;
	float3	Position;

	float	Intensity;
	float3	Color;

	float	SpotFalloff;
	float3	Padding;
};

// Gets the specular value for any light
float calculateSpecular(float3 normal, float3 direction, float3 view, float roughness, float diffuse)
{
	return getSpecular(
		view,
		getReflection(direction, normal),
		getSpecularExponent(roughness, MAX_SPECULAR_EXPONENT)
	) * any(diffuse);
}

// Gets the RGB value of a pixel with a directional light
float3 calculateDirectionalLight(Light light, float3 normal, float3 view, float roughness, float3 surfaceColor, float specularValue)
{
	float3 lightDirection = normalize(light.Direction);
	float diffuse = getDiffuse(normal, -lightDirection);
	float specular = calculateSpecular(normal, lightDirection, view, roughness, diffuse) * specularValue;

	return (diffuse * surfaceColor + specular) * light.Intensity * light.Color;
}

// Gets the RGB value of a pixel with a point light
float3 calculatePointLight(Light light, float3 normal, float3 view, float3 worldPosition, float roughness, float3 surfaceColor, float specularValue)
{
	float3 lightDirection = normalize(light.Position - worldPosition);
	float attenuation = getAttenuation(light.Position, worldPosition, light.Range);
	float diffuse = getDiffuse(normal, lightDirection);
	float specular = calculateSpecular(normal, lightDirection, view, roughness, diffuse) * specularValue;

	return (diffuse * surfaceColor + specular) * attenuation * light.Intensity * light.Color;
}

#endif
