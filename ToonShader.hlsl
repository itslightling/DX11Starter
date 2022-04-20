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
	int hasAlbedoMap;

	float3 emitAmount;
	int hasEmissiveMap;

	float alpha;
	float cutoff;
	float roughness;
	float normalIntensity;

	int hasSpecularMap;

	Light lights[MAX_LIGHTS];
}

Texture2D Albedo : register(t0);
Texture2D Specular : register(t1);
Texture2D Normal : register(t2);
Texture2D Emissive : register(t3);
Texture2D RampDiffuse : register(t4);
Texture2D RampSpecular : register(t5);
SamplerState BasicSampler : register(s0);

float GetRampDiffuse(float original)
{
	if (original < 0.25f) return 0.0f;
	//if (original < 0.5f) return 0.5f;
	if (original < 0.75f) return 0.75f;

	return 1;
}

float GetRampSpecular(float original)
{
	if (original < 0.6f) return 0.0f;

	return 1.0f;
}

float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);
	input.uv = input.uv * scale + offset;
	float3 view = getView(cameraPosition, input.worldPosition);
	float3 normal = input.normal;

	float3 surface = tint;
	float alphaValue = alpha;
	if (hasAlbedoMap)
	{
		float4 sampledAlbedo = Albedo.Sample(BasicSampler, input.uv);
		if (sampledAlbedo.a < cutoff) discard;
		float3 albedo = pow(sampledAlbedo.rgb, 2.2f);
		surface *= albedo.rgb;
		alphaValue *= sampledAlbedo.a;
	}

	if (hasNormalMap > 0)
	{
		float3 unpackedNormal = Normal.Sample(BasicSampler, input.uv).rgb * 2 - 1;
		float3 T = normalize(input.tangent - input.normal * dot(input.tangent, input.normal)) * normalIntensity;
		float3 B = cross(T, input.normal);
		float3x3 TBN = float3x3(T, B, input.normal);
		normal = mul(unpackedNormal, TBN);
	}

	float specularValue = 1;
	if (hasSpecularMap > 0)
	{
		specularValue = Specular.Sample(BasicSampler, input.uv).r;
	}
	float3 light = ambient * surface;
	for (int i = 0; i < lightCount; i++)
	{
		float3 toLight = float3(0, 0, 0);
		float attenuate = 1;
		switch (lights[i].Type)
		{
		case LIGHT_TYPE_DIRECTIONAL:
			toLight = normalize(lights[i].Direction);
			break;
		case LIGHT_TYPE_POINT:
			toLight = normalize(lights[i].Position - input.worldPosition);
			attenuate = getAttenuation(lights[i].Position, input.worldPosition, lights[i].Range);
			break;
		}

		float diffuse = GetRampDiffuse(getDiffuse(normal, toLight));
		float specular = GetRampSpecular(calculateSpecular(normal, toLight, view, specularValue, diffuse) * roughness);

		light += (diffuse * surface.rgb + specular) * attenuate * lights[i].Intensity * lights[i].Color;
	}

	float3 emit = float3(1, 1, 1);
	if (hasEmissiveMap > 0) emit = Emissive.Sample(BasicSampler, input.uv).rgb;
	float4 rim = GetRampSpecular((1 - dot(view, input.normal)) * pow(light, 0.075f));
	float3 final = float3(light + rim + (emit * emitAmount));

	return float4(pow(final, 1.0f / 2.2f), alphaValue);
}