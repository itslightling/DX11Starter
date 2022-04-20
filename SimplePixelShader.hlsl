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
	input.tangent = normalize(input.tangent);

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
		input.normal = mul(unpackedNormal, TBN);
	}
	input.uv = input.uv * scale + offset;

	// view only needs calculated once, so pre-calculate here and pass it to lights
	float3 view = getView(cameraPosition, input.worldPosition);

	float specular = 1;
	if (hasSpecularMap > 0) specular = Specular.Sample(BasicSampler, input.uv).r;
	float3 emit = float3(1, 1, 1);
	if (hasEmissiveMap > 0) emit = Emissive.Sample(BasicSampler, input.uv).rgb;
	float3 light = ambient * surface;

	// loop through lights
	for (int i = 0; i < lightCount; i++)
	{
		switch (lights[i].Type)
		{
		case LIGHT_TYPE_DIRECTIONAL:
			light += calculateDirectionalLight(lights[i], -input.normal, view, roughness, surface, specular);
			break;
		case LIGHT_TYPE_POINT:
			light += calculatePointLight(lights[i], input.normal, view, input.worldPosition, roughness, surface, specular);
			break;
		}
	}

	float3 final = float3(light + (emit * emitAmount));

	if (hasReflectionMap > 0)
	{
		float3 reflVec = getReflection(view, input.normal);
		float3 reflCol = Reflection.Sample(BasicSampler, reflVec).rgba;
		final = lerp(final, reflCol, getFresnel(input.normal, view, F0_NON_METAL));
	}

	return float4(pow(final, 1.0f/2.2f), alphaValue);
}
