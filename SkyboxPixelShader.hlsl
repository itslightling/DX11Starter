#include "SkyboxDefines.hlsli"

TextureCube SkyTexture : register(t0);
SamplerState Sampler : register(s0);

float4 main(SkyboxVertexToPixel input) : SV_TARGET
{
	return SkyTexture.Sample(Sampler, input.sampleDir);
}
