#pragma once

#include <DirectXMath.h>
#include <memory>
#include "SimpleShader.h"
#include "Transform.h"
#include "Camera.h"
#include "Lights.h"

class Material
{
public:
	Material(
		DirectX::XMFLOAT3					_tint,
		float								_roughness,
		std::shared_ptr<SimpleVertexShader>	_vertexShader,
		std::shared_ptr<SimplePixelShader>	_pixelShader);
	~Material();

	void									Activate(
												Transform* _transform,
												std::shared_ptr<Camera> _camera,
												DirectX::XMFLOAT3 _ambient,
												std::vector<Light> _lights);

	DirectX::XMFLOAT3						GetTint();
	DirectX::XMFLOAT2						GetUVScale();
	DirectX::XMFLOAT2						GetUVOffset();
	float									GetRoughness();
	float									GetEmitAmount();
	std::shared_ptr<SimpleVertexShader>		GetVertexShader();
	std::shared_ptr<SimplePixelShader>		GetPixelShader();

	void									SetTint(DirectX::XMFLOAT3 _tint);
	void									SetUVScale(DirectX::XMFLOAT2 _scale);
	void									SetUVOffset(DirectX::XMFLOAT2 _offset);
	void									SetRoughness(float _roughness);
	void									SetEmitAmount(float _emit);
	void									SetVertexShader(std::shared_ptr<SimpleVertexShader> _vertexShader);
	void									SetPixelShader(std::shared_ptr<SimplePixelShader> _pixelShader);

	void									PushSampler(std::string _name, Microsoft::WRL::ComPtr<ID3D11SamplerState> _sampler);
	void									PushTexture(std::string _name, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _texture);

private:
	DirectX::XMFLOAT3						tint;
	float									roughness;
	float									emitAmount;
	DirectX::XMFLOAT2						uvScale;
	DirectX::XMFLOAT2						uvOffset;
	std::shared_ptr<SimpleVertexShader>		vertexShader;
	std::shared_ptr<SimplePixelShader>		pixelShader;

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>>			samplers;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>	textures;
};
