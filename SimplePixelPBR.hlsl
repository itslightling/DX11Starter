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
SamplerState BasicSampler : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
	// normalize inputs and set uv scaling
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);
	input.uv = input.uv * scale + offset;

	// gets albedo with gamma correction
	float4 albedo = pow(Albedo.Sample(BasicSampler, input.uv), 2.2f);

	// gets normal map
	float3 normal = getNormal(BasicSampler, Normal, input.uv, input.normal, input.tangent, normalIntensity);

	// get pbr values
	float roughness = Roughness.Sample(BasicSampler, input.uv).r;
	float metalness = Metalness.Sample(BasicSampler, input.uv).r;
	float3 specular = lerp(F0_NON_METAL.rrr, albedo.rgb, metalness);

	// pre-calculate view
	float3 view = normalize(cameraPosition - input.worldPosition);

	// calculate lighting
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

	// calculate the final color value with lighting
	float3 final = light;

	// gamma-correct the final value
	return float4(pow(final, 1.0f / 2.2f), albedo.a);
}