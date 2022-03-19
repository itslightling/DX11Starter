#ifndef __SHADER_LIGHTS__
#define __SHADER_LIGHTS__

#define LIGHT_TYPE_DIRECTIONAL	0
#define LIGHT_TYPE_POINT		1
#define LIGHT_TYPE_SPOT			2

// Struct representing light data
// - This should match Lights.h
struct Light
{
	int		Type;
	float3	Direction;

	float	Range;
	float3	Position;

	float	Intensity;
	float3	Color;

	float	SpotFalloff;
	float3	Padding;
};

#endif
