#pragma once

#include <DirectXMath.h>
#include <memory>
#include "SimpleShader.h"

class Material
{
public:
	Material(
		DirectX::XMFLOAT4					_tint,
		std::shared_ptr<SimpleVertexShader>	_vertexShader,
		std::shared_ptr<SimplePixelShader>	_pixelShader);
	~Material();

	DirectX::XMFLOAT4						GetTint();
	std::shared_ptr<SimpleVertexShader>		GetVertexShader();
	std::shared_ptr<SimplePixelShader>		GetPixelShader();

	void									SetTint(DirectX::XMFLOAT4 _tint);
	void									SetVertexShader(std::shared_ptr<SimpleVertexShader> _vertexShader);
	void									SetPixelShader(std::shared_ptr<SimplePixelShader> _pixelShader);

private:
	DirectX::XMFLOAT4						tint;
	std::shared_ptr<SimpleVertexShader>		vertexShader;
	std::shared_ptr<SimplePixelShader>		pixelShader;
};
