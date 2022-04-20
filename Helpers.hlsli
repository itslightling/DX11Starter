#ifndef __SHADER_HELPERS__
#define __SHADER_HELPERS__

// from environment map demo
static const float F0_NON_METAL = 0.04f;

// Minimum roughness for when spec distribution function denominator goes to zero
static const float MIN_ROUGHNESS = 0.0000001f; // 6 zeros after decimal

// Handy to have this as a constant
static const float PI = 3.14159265359f;

// gets view vector, needed once per shader
float3 getView(float3 cameraPosition, float3 pixelWorldPosition)
{
	return normalize(cameraPosition - pixelWorldPosition);
}

// gets reflection vector, needed per light
float3 getReflection(float3 direction, float3 normal)
{
	return reflect(-direction, normal);
}

// gets specular exponent: (1-roughness) * max
float getSpecularExponent(float roughness, float max)
{
	return (1.0f - roughness) * max;
}

// gets specular value: clamp(dot(r,v))^e
float getSpecular(float3 view, float3 reflection, float exponent)
{
	float specular = 0;
	if (exponent > 0.05f)
	{
		specular = pow(saturate(dot(reflection, view)), exponent);
	}
	return specular;
}

// gets diffuse value: clamp(dot(n,d))
float getDiffuse(float3 normal, float3 direction)
{
	return saturate(dot(normal, direction));
}

// gets attenuation: clamp(1 - (distance^2 / range^2))^2
float getAttenuation(float3 pointPosition, float3 worldPosition, float3 range)
{
	float dist = distance(pointPosition, worldPosition);
	float attn = saturate(1.0f - (dist * dist / (range * range)));
	return attn * attn;
}

// from environment map demo
// gets fresnel (Schlick approx.): f0 + (1-f0)(1 - (n dot v))^5
float getFresnel(float3 normal, float3 view, float specularValue)
{
	return specularValue + (1 - specularValue) * pow(1 - saturate(dot(normal, view)), 5);
}

// gets normal: n*TBN, where n = sampled normal map, N = normal vector, T = processed tangent vector (t*N-dot(t,N), B = processed bitangent vector (cross(T,N))
float3 getNormal(SamplerState normalSampler, Texture2D map, float2 uv, float3 normal, float3 tangent, float intensity)
{
	float3 n = map.Sample(normalSampler, uv).rgb * 2 - 1;
	float3 T = normalize(tangent - normal * dot(tangent, normal)) * intensity;
	float3 B = cross(T, normal);
	float3x3 TBN = float3x3(T, B, normal);
	return mul(n, TBN);
}

#endif
