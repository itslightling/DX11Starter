#include "Includes.hlsli"

cbuffer ExternalData : register(b0)
{
	float3 cameraPosition;
	float roughness;
	float3 ambient;
	float3 tint;
	Light directionalLight1;
}

float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	float3 ambientTint = ambient * tint;
	float3 directionalLight1Dir = normalize(-directionalLight1.Direction);
	float diffuse = saturate(dot(input.normal, directionalLight1Dir));

	float3 view = normalize(cameraPosition - input.worldPosition);
	float3 reflection = reflect(directionalLight1Dir, input.normal);

	float specularExponent = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;
	float specular = 0;
	if (specularExponent > 0.05f)
	{
		specular = pow(saturate(dot(reflection, view)), specularExponent);
	}

	float3 final = (diffuse * directionalLight1.Color * tint) + ambientTint + specular;

	return float4(final, 1);
}
