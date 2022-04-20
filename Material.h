#pragma once

#include <DirectXMath.h>
#include <memory>
#include <functional>
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
constexpr auto TEXTYPE_ROUGHNESS = "Roughness";
constexpr auto TEXTYPE_METALNESS = "Metalness";
constexpr auto TEXTYPE_RAMPDIFFUSE = "RampDiffuse";
constexpr auto TEXTYPE_RAMPSPECULAR = "RampSpecular";

class Material
{
public:
	Material(
		bool								_pbr,
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
	float									GetAlpha();
	float									GetCutoff();
	float									GetNormalIntensity();
	float									GetRimCutoff();
	float									GetOutlineThickness();
	DirectX::XMFLOAT3						GetEmitAmount();
	DirectX::XMFLOAT3						GetOutlineTint();
	DirectX::XMFLOAT3						GetRimTint();
	std::shared_ptr<SimpleVertexShader>		GetVertexShader();
	std::shared_ptr<SimplePixelShader>		GetPixelShader();

	void									SetTint(DirectX::XMFLOAT3 _tint);
	void									SetUVScale(DirectX::XMFLOAT2 _scale);
	void									SetUVOffset(DirectX::XMFLOAT2 _offset);
	void									SetRoughness(float _roughness);
	void									SetAlpha(float _alpha);
	void									SetCutoff(float _cutoff);
	void									SetNormalIntensity(float _intensity);
	void									SetRimCutoff(float _cutoff);
	void									SetOutlineThickness(float _thickness);
	void									SetEmitAmount(DirectX::XMFLOAT3 _emit);
	void									SetOutlineTint(DirectX::XMFLOAT3 _tint);
	void									SetRimTint(DirectX::XMFLOAT3 _tint);
	void									SetVertexShader(std::shared_ptr<SimpleVertexShader> _vertexShader);
	void									SetPixelShader(std::shared_ptr<SimplePixelShader> _pixelShader);

	void									LoadTexture(const wchar_t* _path, const char* _type, ID3D11Device* _device, ID3D11DeviceContext* _context);
	void									PushSampler(std::string _name, Microsoft::WRL::ComPtr<ID3D11SamplerState> _sampler);
	void									PushTexture(std::string _name, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _texture);

	bool									hasAlbedoMap;
	bool									hasEmissiveMap;
	bool									hasSpecularMap;
	bool									hasNormalMap;
	bool									hasReflectionMap;
	bool									hasRampDiffuse;
	bool									hasRampSpecular;
private:
	void									ActivateStandard(
												Transform* _transform,
												std::shared_ptr<Camera> _camera,
												DirectX::XMFLOAT3 _ambient,
												std::vector<Light> _lights);
	void									ActivatePBR(
												Transform* _transform,
												std::shared_ptr<Camera> _camera,
												DirectX::XMFLOAT3 _ambient,
												std::vector<Light> _lights);

	bool									pbr;
	DirectX::XMFLOAT3						tint;
	float									roughness;
	float									alpha;
	float									cutoff;
	float									normalIntensity;
	float									outlineThickness;
	float									rimCutoff;
	DirectX::XMFLOAT3						emitAmount;
	DirectX::XMFLOAT3						outlineTint;
	DirectX::XMFLOAT3						rimTint;
	DirectX::XMFLOAT2						uvScale;
	DirectX::XMFLOAT2						uvOffset;
	std::shared_ptr<SimpleVertexShader>		vertexShader;
	std::shared_ptr<SimplePixelShader>		pixelShader;

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>>			samplers;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>	textures;
};
