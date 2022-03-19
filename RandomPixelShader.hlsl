#include "Includes.hlsli"

cbuffer ExternalData : register(b0)
{
	float4 tint;
    float noise;
}

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	// Just return the tint
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
	return float4(
        sin(snoise(input.uv * noise) + tint.r - random(input.uv)) + (tint.r * 0.5),
        sin(snoise(input.uv * noise) + tint.g - random(input.uv)) + (tint.g * 0.5),
        sin(snoise(input.uv * noise) + tint.b - random(input.uv)) + (tint.b * 0.5),
        cos(random(input.screenPosition)));
}
