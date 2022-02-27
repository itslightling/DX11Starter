#include "Material.h"

Material::Material(DirectX::XMFLOAT4 _tint, std::shared_ptr<SimpleVertexShader> _vertexShader, std::shared_ptr<SimplePixelShader> _pixelShader)
{
	tint = _tint;
	vertexShader = _vertexShader;
	pixelShader = _pixelShader;
}

Material::~Material()
{
}

DirectX::XMFLOAT4 Material::GetTint()
{
	return tint;
}

std::shared_ptr<SimpleVertexShader> Material::GetVertexShader()
{
	return vertexShader;
}

std::shared_ptr<SimplePixelShader> Material::GetPixelShader()
{
	return pixelShader;
}

void Material::SetTint(DirectX::XMFLOAT4 _tint)
{
	tint = _tint;
}

void Material::SetVertexShader(std::shared_ptr<SimpleVertexShader> _vertexShader)
{
	vertexShader = _vertexShader;
}

void Material::SetPixelShader(std::shared_ptr<SimplePixelShader> _pixelShader)
{
	pixelShader = _pixelShader;
}
