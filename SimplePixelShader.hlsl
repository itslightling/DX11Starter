#include "Includes.hlsli"

cbuffer ExternalData : register(b0)
{
	float3 cameraPosition;
	float roughness;
	float3 ambient;
}

float4 main() : SV_TARGET
{
	return float4(ambient.rgb, 1);
}
