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
	int hasReflectionMap;

	Light lights[MAX_LIGHTS];
}

Texture2D Albedo : register(t0);
Texture2D Specular : register(t1);
Texture2D Emissive : register(t2);
Texture2D Normal : register(t3);
TextureCube Reflection : register(t4);
SamplerState BasicSampler : register(s0);

// shader entry point
float4 main(VertexToPixel input) : SV_TARGET
{
	// ensure input normals are normalized
	input.normal = normalize(input.normal);
	input.uv = input.uv * scale + offset;

	// view only needs calculated once, so pre-calculate here and pass it to lights
	float3 view = getView(cameraPosition, input.worldPosition);

	float4 albedo = Albedo.Sample(BasicSampler, input.uv).rgba;
	float specular = 1;
	if (hasSpecularMap > 0) specular = Specular.Sample(BasicSampler, input.uv).r;
	float3 emit = float3(1, 1, 1);
	if (hasEmissiveMap > 0) emit = Emissive.Sample(BasicSampler, input.uv).rgb;
	float3 surface = albedo.rgb * tint;
	float3 light = ambient * surface;

	// loop through lights
	for (int i = 0; i < lightCount; i++)
	{
		switch (lights[i].Type)
		{
		case LIGHT_TYPE_DIRECTIONAL:
			light += calculateDirectionalLight(lights[i], input.normal, view, roughness, surface, specular);
			break;
		case LIGHT_TYPE_POINT:
			light += calculatePointLight(lights[i], input.normal, view, input.worldPosition, roughness, surface, specular);
			break;
		}
	}

	return float4(light + (emit * emitAmount), albedo.a);
}
