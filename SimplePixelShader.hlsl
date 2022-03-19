#include "Defines.hlsli"
#include "Helpers.hlsli"
#include "Lights.hlsli"

cbuffer ExternalData : register(b0)
{
	float3 cameraPosition;
	float roughness;
	float3 ambient;
	float3 tint;
	Light directionalLight1;
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

	float3 light = calculateDirectionalLight(directionalLight1, input.normal, input.worldPosition, cameraPosition, roughness, tint);
	float3 ambientTint = ambient * tint;
	float3 final = light + ambientTint;

	return float4(final, 1);
}
