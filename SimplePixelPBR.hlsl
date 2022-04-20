#include "Defines.hlsli"
#include "Helpers.hlsli"
#include "HelpersPBR.hlsli"
#include "Lights.hlsli"
#include "LightsPBR.hlsli"

#define MAX_LIGHTS 128

cbuffer ExternalData : register(b0)
{
	float2 offset;
	float2 scale;

	float3 cameraPosition;
	float lightCount;

	float normalIntensity;

	Light lights[MAX_LIGHTS];
}

Texture2D Albedo : register(t0);
Texture2D Normal : register(t1);
Texture2D Roughness : register(t2);
Texture2D Metalness : register(t3);
TextureCube Reflection : register(t4);
SamplerState Sampler : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);

	float3 view = normalize(cameraPosition - input.worldPosition);

	float4 albedo = pow(Albedo.Sample(Sampler, input.uv), 2.2f);

	float3 normal = getNormal(Sampler, Normal, input.uv, input.normal, input.tangent, normalIntensity);

	float roughness = Roughness.Sample(Sampler, input.uv).r;
	float metalness = Metalness.Sample(Sampler, input.uv).r;
	float3 specular = lerp(F0_NON_METAL.rrr, albedo.rgb, metalness);

	float3 light = float3(0, 0, 0);
	for (int i = 0; i < lightCount; i++)
	{
		switch (lights[i].Type)
		{
		case LIGHT_TYPE_DIRECTIONAL:
			light += directionalLightPBR(lights[i], normal, view, roughness, metalness, albedo, specular);
			break;
		case LIGHT_TYPE_POINT:
			light += pointLightPBR(lights[i], normal, view, roughness, metalness, albedo, specular, input.worldPosition);
			break;
		}
	}

	float3 final = light;

	return float4(pow(final, 1.0f / 2.2f), albedo.a);
}