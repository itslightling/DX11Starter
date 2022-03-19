#include "Defines.hlsli"
#include "Helpers.hlsli"
#include "Lights.hlsli"

// temporary
#define LIGHT_COUNT 3

cbuffer ExternalData : register(b0)
{
	float3 cameraPosition;
	float roughness;
	float3 ambient;
	float3 tint;
	Light lights[LIGHT_COUNT];
}

float calculateSpecular(float3 normal, float3 direction, float3 worldPosition, float3 cameraPosition, float roughness)
{
	return getSpecular(
		getView(cameraPosition, worldPosition),
		getReflection(direction, normal),
		getSpecularExponent(roughness, MAX_SPECULAR_EXPONENT)
	);
}

float3 calculateDirectionalLight(Light light, float3 normal, float3 worldPosition, float3 cameraPosition, float roughness, float3 surfaceColor)
{
	float3 lightDirection = normalize(light.Direction);
	float diffuse = getDiffuse(normal, -lightDirection);
	float specular = calculateSpecular(normal, lightDirection, worldPosition, cameraPosition, roughness);

	return (diffuse * surfaceColor + specular) * light.Intensity * light.Color;
}

float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);

	float3 light = ambient * tint;
	for (int i = 0; i < LIGHT_COUNT; i++)
	{
		switch (lights[i].Type)
		{
		case LIGHT_TYPE_DIRECTIONAL:
			light += calculateDirectionalLight(lights[i], input.normal, input.worldPosition, cameraPosition, roughness, tint);
			break;
		}
	}

	return float4(light, 1);
}
