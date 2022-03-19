#ifndef __SHADER_HELPERS__
#define __SHADER_HELPERS__

float3 getView(float3 cameraPosition, float3 pixelWorldPosition)
{
	return normalize(cameraPosition - pixelWorldPosition);
}

float3 getReflection(float3 direction, float3 normal)
{
	return reflect(direction, normal);
}

float getSpecularExponent(float roughness, float max)
{
	return (1.0f - roughness) * max;
}

float getSpecular(float3 view, float3 reflection, float exponent)
{
	float specular = 0;
	if (exponent > 0.05f)
	{
		specular = pow(saturate(dot(reflection, view)), exponent);
	}
	return specular;
}

float getDiffuse(float3 normal, float3 direction)
{
	return saturate(dot(normal, direction));
}

float getAttenuation(float3 pointPosition, float3 worldPosition, float3 range)
{
	float dist = distance(pointPosition, worldPosition);
	float attn = saturate(1.0f - (dist * dist / (range * range)));
	return attn * attn;
}

#endif
