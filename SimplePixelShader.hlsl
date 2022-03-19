#include "Includes.hlsli"

cbuffer ExternalData : register(b0)
{
	float3 cameraPosition;
	float roughness;
}

float4 main() : SV_TARGET
{
	return float4(roughness.rrr, 1);
}
