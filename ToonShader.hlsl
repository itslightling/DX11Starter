#include "Defines.hlsli"
#include "Helpers.hlsli"
#include "Lights.hlsli"

#define MAX_LIGHTS 128

cbuffer ExternalData : register(b0)
{
	float3 cameraPosition;
	float roughness;

	float2 offset;
	float2 scale;

	float3 ambient;
	float emitAmount;

	float3 tint;
	float lightCount;

	int hasEmissiveMap;
	int hasSpecularMap;
	int hasNormalMap;

	Light lights[MAX_LIGHTS];
}

Texture2D Albedo : register(t0);
Texture2D Specular : register(t1);
Texture2D Emissive : register(t2);
Texture2D Normal : register(t3);
SamplerState BasicSampler : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}