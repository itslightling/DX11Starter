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

	float3 outlineTint;
	float outlineThickness;
	
	float3 rimTint;
	float rimCutoff;

	int hasSpecularMap;
	int hasRampDiffuse;
	int hasRampSpecular;

	Light lights[MAX_LIGHTS];
}

Texture2D Albedo : register(t0);
Texture2D Specular : register(t1);
Texture2D Normal : register(t2);
Texture2D Emissive : register(t3);
Texture2D RampDiffuse : register(t4);
Texture2D RampSpecular : register(t5);
SamplerState BasicSampler : register(s0);
SamplerState ClampSampler : register(s1);

float GetRampDiffuse(float original)
{
	if (original < 0.25f) return 0.0f;
	if (original < 0.33f) return 0.33f;
	if (original < 0.8f) return 0.8f;

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
		normal = getNormal(BasicSampler, Normal, input.uv, input.normal, input.tangent, normalIntensity);
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

		float diffuse = 0;
		float specular = 0;
		if (hasRampDiffuse > 0)
		{
			diffuse = RampDiffuse.Sample(ClampSampler, float2(getDiffuse(normal, toLight), 0)).r;
		}
		else
		{
			diffuse = GetRampDiffuse(getDiffuse(normal, toLight));
		}
		if (hasRampSpecular > 0)
		{
			specular = RampSpecular.Sample(ClampSampler, float2(calculateSpecular(normal, toLight, view, specularValue, diffuse) * roughness, 0));
		}
		else
		{
			specular = GetRampSpecular(calculateSpecular(normal, toLight, view, specularValue, diffuse) * roughness);
		}

		light += (diffuse * surface.rgb + specular) * attenuate * lights[i].Intensity * lights[i].Color;
	}

	float3 emit = float3(1, 1, 1);
	if (hasEmissiveMap > 0) emit = Emissive.Sample(BasicSampler, input.uv).rgb;

	float vDotN = (1 - dot(view, input.normal));
	float rimValue = GetRampSpecular(vDotN * pow(light, rimCutoff));
	float outlineValue = GetRampSpecular(vDotN * outlineThickness);

	if (rimValue > 0)
	{
		return float4(light + (emit * emitAmount) + rimTint, alphaValue);
	}

	if (outlineValue > 0)
	{
		return float4(outlineValue * outlineTint, alphaValue);
	}

	float3 final = float3(light + (emit * emitAmount));

	return float4(pow(final, 1.0f / 2.2f), alphaValue);
}
