#include "Includes.hlsli"

cbuffer ExternalData : register(b0)
{
	float3 cameraPosition;
	float roughness;
	float3 ambient;
	Light directionalLight1;
}

float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	return float4(directionalLight1.Color, 1);
}
