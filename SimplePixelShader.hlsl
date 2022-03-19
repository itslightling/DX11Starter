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

float calculateSpecular(float3 normal, float3 worldPosition, float3 cameraPosition, float3 direction, float roughness)
{
	return getSpecular(
		getView(cameraPosition, worldPosition),
		getReflection(direction, normal),
		getSpecularExponent(roughness, MAX_SPECULAR_EXPONENT)
	);
}

float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	float3 directionalLight1Dir = normalize(-directionalLight1.Direction);

	float diffuse = getDiffuse(input.normal, directionalLight1Dir);
	float specular = calculateSpecular(input.normal, input.worldPosition, cameraPosition, directionalLight1Dir, roughness);

	float3 final = (diffuse * directionalLight1.Color * tint) + (ambient * tint) + specular;

	return float4(final, 1);
}
