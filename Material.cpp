#include "Material.h"

Material::Material(
	DirectX::XMFLOAT3 _tint,
	float _roughness,
	std::shared_ptr<SimpleVertexShader> _vertexShader,
	std::shared_ptr<SimplePixelShader> _pixelShader)
{
	tint = _tint;
	roughness = _roughness;
	vertexShader = _vertexShader;
	pixelShader = _pixelShader;
}

Material::~Material()
{
}

DirectX::XMFLOAT3 Material::GetTint()
{
	return tint;
}

float Material::GetRoughness()
{
	return roughness;
}

std::shared_ptr<SimpleVertexShader> Material::GetVertexShader()
{
	return vertexShader;
}

std::shared_ptr<SimplePixelShader> Material::GetPixelShader()
{
	return pixelShader;
}

void Material::SetTint(DirectX::XMFLOAT3 _tint)
{
	tint = _tint;
}

void Material::SetRoughness(float _roughness)
{
	if (_roughness > 1)
	{
		roughness = 1;
	}
	else if (_roughness < 0)
	{
		roughness = 0;
	}
	else
	{
		roughness = _roughness;
	}
}

void Material::SetVertexShader(std::shared_ptr<SimpleVertexShader> _vertexShader)
{
	vertexShader = _vertexShader;
}

void Material::SetPixelShader(std::shared_ptr<SimplePixelShader> _pixelShader)
{
	pixelShader = _pixelShader;
}
