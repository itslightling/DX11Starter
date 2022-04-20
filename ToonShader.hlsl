#include "Defines.hlsli"
#include "Helpers.hlsli"
#include "Lights.hlsli"

#define MAX_LIGHTS 128

cbuffer ExternalData : register(b0)
{
	float3 cameraPosition;
	int hasNormalMap;

	float2 offset;
	float2 scale;

	float3 ambient;
	float lightCount;

	float3 tint;
	int hasSpecularMap;

	Light lights[MAX_LIGHTS];
}

Texture2D Albedo : register(t0);
Texture2D Specular : register(t1);
Texture2D Normal : register(t2);
SamplerState BasicSampler : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);
	input.uv = input.uv * scale + offset;
	float3 view = getView(cameraPosition, input.worldPosition);

	if (hasNormalMap > 0)
	{
		float3 unpackedNormal = Normal.Sample(BasicSampler, input.uv).rgb * 2 - 1;
		float3 T = normalize(input.tangent - input.normal * dot(input.tangent, input.normal));
		float3 B = cross(T, input.normal);
		float3x3 TBN = float3x3(T, B, input.normal);
		input.normal = mul(unpackedNormal, TBN);
	}

	float specular = 1;
	if (hasSpecularMap > 0)
	{
		specular = Specular.Sample(BasicSampler, input.uv).r;
	}

	float4 albedo = pow(Albedo.Sample(BasicSampler, input.uv).rgba, 2.2f);
	float3 surface = albedo.rgb * tint;
	float3 light = ambient * surface;
	for (int i = 0; i < lightCount; i++)
	{
		switch (lights[i].Type)
		{
		case LIGHT_TYPE_DIRECTIONAL:
			light += calculateDirectionalLight(lights[i], -input.normal, view, 1 /*roughness*/, surface, specular);
			break;
		case LIGHT_TYPE_POINT:
			light += calculatePointLight(lights[i], input.normal, view, input.worldPosition, 1 /*roughness*/, surface, specular);
			break;
		}
	}

	float3 final = float3(light/* + (emit * emitAmount)*/);

	return float4(pow(final, 1.0f / 2.2f), albedo.a);
}