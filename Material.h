#pragma once

#include <DirectXMath.h>
#include <memory>
#include "DXCore.h"
#include "SimpleShader.h"
#include "Transform.h"
#include "Camera.h"
#include "Lights.h"
#include "WICTextureLoader.h"

constexpr auto TEXTYPE_ALBEDO = "Albedo";
constexpr auto TEXTYPE_NORMAL = "Normal";
constexpr auto TEXTYPE_EMISSIVE = "Emissive";
constexpr auto TEXTYPE_SPECULAR = "Specular";
constexpr auto TEXTYPE_REFLECTION = "Reflection";

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

	void									LoadTexture(const wchar_t* _path, const char* _type, ID3D11Device* _device, ID3D11DeviceContext* _context);
	void									PushSampler(std::string _name, Microsoft::WRL::ComPtr<ID3D11SamplerState> _sampler);
	void									PushTexture(std::string _name, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _texture);

	bool									hasEmissiveMap;
	bool									hasSpecularMap;
	bool									hasNormalMap;
	bool									hasReflectionMap;
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
