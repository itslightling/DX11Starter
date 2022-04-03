#include "SkyboxDefines.hlsli"

cbuffer ExternalData : register(b0)
{
	matrix view;
	matrix projection;
}

matrix RemoveTranslation(matrix m)
{
	m._14 = 0;
	m._24 = 0;
	m._34 = 0;
	return m;
}

SkyboxVertexToPixel main(SkyboxVertexShaderInput input)
{
	SkyboxVertexToPixel output;
	matrix worldViewProjection = mul(projection, RemoveTranslation(view));
	output.screenPosition = mul(worldViewProjection, float4(input.localPosition, 1.0f));
	output.screenPosition.z = output.screenPosition.w;
	output.sampleDir = input.localPosition;
	return output;
}
