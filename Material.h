#pragma once

#include <DirectXMath.h>
#include <memory>
#include "SimpleShader.h"

class Material
{
public:
	Material(
		DirectX::XMFLOAT3					_tint,
		float								_roughness,
		std::shared_ptr<SimpleVertexShader>	_vertexShader,
		std::shared_ptr<SimplePixelShader>	_pixelShader);
	~Material();

	DirectX::XMFLOAT3						GetTint();
	float									GetRoughness();
	std::shared_ptr<SimpleVertexShader>		GetVertexShader();
	std::shared_ptr<SimplePixelShader>		GetPixelShader();

	void									SetTint(DirectX::XMFLOAT3 _tint);
	void									SetRoughness(float _roughness);
	void									SetVertexShader(std::shared_ptr<SimpleVertexShader> _vertexShader);
	void									SetPixelShader(std::shared_ptr<SimplePixelShader> _pixelShader);

private:
	DirectX::XMFLOAT3						tint;
	float									roughness;
	std::shared_ptr<SimpleVertexShader>		vertexShader;
	std::shared_ptr<SimplePixelShader>		pixelShader;
};
