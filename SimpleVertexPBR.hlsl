#include "Defines.hlsli"

cbuffer ExternalData : register(b0)
{
	matrix world;
	matrix worldInvTranspose;
	matrix view;
	matrix projection;
}

VertexToPixel main(VertexShaderInput input)
{
	VertexToPixel output;

	matrix worldViewProjection = mul(projection, mul(view, world));
	output.screenPosition = mul(worldViewProjection, float4(input.localPosition, 1.0f));

	output.uv = input.uv;
	output.normal = normalize(mul((float3x3)worldInvTranspose, input.normal));
	output.tangent = normalize(mul((float3x3)worldInvTranspose, input.tangent));
	output.worldPosition = mul(world, float4(input.localPosition, 1)).xyz;

	return output;
}
