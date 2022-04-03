#ifndef __SKYBOX_DEFINES__
#define __SKYBOX_DEFINES__

struct SkyboxVertexToPixel
{
	float4 screenPosition	: SV_POSITION;
	float3 sampleDir		: DIRECTION;
};

// Struct representing a single vertex worth of data
// - This should match Vertex.h
struct SkyboxVertexShaderInput
{
	float3 localPosition	: POSITION;
	float3 normal			: NORMAL;
	float3 tangent			: TANGENT;
	float2 uv				: TEXCOORD;
};

#endif
