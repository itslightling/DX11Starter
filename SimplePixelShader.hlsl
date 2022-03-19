#include "Includes.hlsli"

cbuffer ExternalData : register(b0)
{
	float3 cameraPosition;
	float roughness;
	float3 ambient;
	float3 tint;
	Light directionalLight1;
}

float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	float3 ambientTint = ambient * tint;
	float3 directionalLight1Dir = normalize(-directionalLight1.Direction);
	float diffuse = saturate(dot(input.normal, directionalLight1Dir));
	float3 final = (diffuse * directionalLight1.Color * tint) + ambientTint;

	return float4(final, 1);
}
