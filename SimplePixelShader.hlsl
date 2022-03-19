#include "Defines.hlsli"
#include "Helpers.hlsli"
#include "Lights.hlsli"

// temporary
#define LIGHT_COUNT 5

cbuffer ExternalData : register(b0)
{
	float3 cameraPosition;
	float roughness;
	float3 ambient;
	float3 tint;
	Light lights[LIGHT_COUNT];
}

// Gets the specular value for any light
float calculateSpecular(float3 normal, float3 direction, float3 view, float roughness)
{
	return getSpecular(
		view,
		getReflection(direction, normal),
		getSpecularExponent(roughness, MAX_SPECULAR_EXPONENT)
	);
}

// Gets the RGB value of a pixel with a directional light
float3 calculateDirectionalLight(Light light, float3 normal, float3 view, float roughness, float3 surfaceColor)
{
	float3 lightDirection = normalize(light.Direction);
	float diffuse = getDiffuse(normal, -lightDirection);
	float specular = calculateSpecular(normal, lightDirection, view, roughness);

	return (diffuse * surfaceColor + specular) * light.Intensity * light.Color;
}

// Gets the RGB value of a pixel with a point light
float3 calculatePointLight(Light light, float3 normal, float3 view, float3 worldPosition, float roughness, float3 surfaceColor)
{
	float3 lightDirection = normalize(worldPosition - light.Position);
	float attenuation = getAttenuation(light.Position, worldPosition, light.Range);
	float diffuse = getDiffuse(normal, -lightDirection);
	float specular = calculateSpecular(normal, lightDirection, view, roughness);

	return (diffuse * surfaceColor + specular) * attenuation * light.Intensity * light.Color;
}

// shader entry point
float4 main(VertexToPixel input) : SV_TARGET
{
	// ensure input normals are normalized
	input.normal = normalize(input.normal);

	// view only needs calculated once, so pre-calculate here and pass it to lights
	float3 view = getView(cameraPosition, input.worldPosition);

	// start with ambient light and material tint
	float3 light = ambient * tint;

	// loop through lights
	for (int i = 0; i < LIGHT_COUNT; i++)
	{
		switch (lights[i].Type)
		{
		case LIGHT_TYPE_DIRECTIONAL:
			light += calculateDirectionalLight(lights[i], input.normal, view, roughness, tint);
			break;
		case LIGHT_TYPE_POINT:
			light += calculatePointLight(lights[i], input.normal, view, input.worldPosition, roughness, tint);
			break;
		}
	}

	return float4(light, 1);
}
