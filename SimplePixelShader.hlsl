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
	// normalize inputs and set uv scaling
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);
	input.uv = input.uv * scale + offset;

	// get surface from tint, multiply it by albedo if there is one
	// get alpha from exposed alpha value, multiply it by albedo alpha if there is one
	float3 surface = tint;
	float alphaValue = alpha;
	if (hasAlbedoMap)
	{
		float4 sampledAlbedo = Albedo.Sample(BasicSampler, input.uv);
		// discard if the alpha of the texture is less than the cutoff point
		if (sampledAlbedo.a < cutoff) discard;
		// gamma-correct the RGB of the albedo
		float3 albedo = pow(sampledAlbedo.rgb, 2.2f);
		// multiply surface and alpha by the sampled texture
		surface *= albedo.rgb;
		alphaValue *= sampledAlbedo.a;
	}

	// gets normal map if there is one
	float3 normal = input.normal;
	if (hasNormalMap > 0)
		normal = getNormal(BasicSampler, Normal, input.uv, input.normal, input.tangent, normalIntensity);

	// gets specular value; if there is a specular map, use that instead
	float specular = 1;
	if (hasSpecularMap > 0)
		specular = Specular.Sample(BasicSampler, input.uv).r;

	// pre-calculate view
	float3 view = getView(cameraPosition, input.worldPosition);

	// calculate lighting
	float3 light = ambient * surface;
	for (int i = 0; i < lightCount; i++)
	{
		switch (lights[i].Type)
		{
		case LIGHT_TYPE_DIRECTIONAL:
			light += calculateDirectionalLight(lights[i], -normal, view, roughness, surface, specular);
			break;
		case LIGHT_TYPE_POINT:
			light += calculatePointLight(lights[i], normal, view, input.worldPosition, roughness, surface, specular);
			break;
		}
	}

	// get emission; use emissive map if there is one
	float3 emit = float3(1, 1, 1);
	if (hasEmissiveMap > 0)
		emit = Emissive.Sample(BasicSampler, input.uv).rgb;

	// calculate the final color value with lighting and emission
	float3 final = float3(light + (emit * emitAmount));

	// utilize reflection map if there is one
	if (hasReflectionMap > 0)
	{
		float3 reflVec = getReflection(view, normal);
		float3 reflCol = Reflection.Sample(BasicSampler, reflVec).rgba;
		final = lerp(final, reflCol, getFresnel(normal, view, F0_NON_METAL));
	}

	// gamma-correct the final value
	return float4(pow(final, 1.0f/2.2f), alphaValue);
}
