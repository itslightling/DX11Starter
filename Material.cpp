#include "Material.h"

Material::Material(
	int _mode,
	DirectX::XMFLOAT3 _tint,
	float _roughness,
	std::shared_ptr<SimpleVertexShader> _vertexShader,
	std::shared_ptr<SimplePixelShader> _pixelShader)
{
	mode = _mode;
	tint = _tint;
	roughness = _roughness;
	normalIntensity = 1.f;
	alpha = 1;
	cutoff = 0;
	vertexShader = _vertexShader;
	pixelShader = _pixelShader;
	uvOffset = DirectX::XMFLOAT2(0, 0);
	uvScale = DirectX::XMFLOAT2(1, 1);
	emitAmount = DirectX::XMFLOAT3(0, 0, 0);
	hasAlbedoMap = false;
	hasEmissiveMap = false;
	hasSpecularMap = false;
	hasNormalMap = false;
	hasReflectionMap = false;
	hasRampDiffuse = false;
	hasRampSpecular = false;
	outlineThickness = 1;
	rimCutoff = 0.075f;
	rimTint = DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f);
}

Material::~Material()
{
}

void Material::Activate(Transform* _transform, std::shared_ptr<Camera> _camera, DirectX::XMFLOAT3 _ambient, std::vector<Light> _lights)
{
	switch (mode)
	{
	case MATTYPE_PBR:
		ActivatePBR(_transform, _camera, _ambient, _lights);
		break;
	case MATTYPE_TOON:
		ActivateToon(_transform, _camera, _ambient, _lights);
		break;
	case MATTYPE_STANDARD:
	default:
		ActivateStandard(_transform, _camera, _ambient, _lights);
		break;
	}
}

#pragma region Getters
DirectX::XMFLOAT3 Material::GetTint()
{
	return tint;
}

DirectX::XMFLOAT2 Material::GetUVScale()
{
	return uvScale;
}

DirectX::XMFLOAT2 Material::GetUVOffset()
{
	return uvOffset;
}

float Material::GetRoughness()
{
	return roughness;
}

float Material::GetAlpha()
{
	return alpha;
}

float Material::GetCutoff()
{
	return cutoff;
}

float Material::GetNormalIntensity()
{
	return normalIntensity;
}

float Material::GetRimCutoff()
{
	return rimCutoff;
}

float Material::GetOutlineThickness()
{
	return outlineThickness;
}

DirectX::XMFLOAT3 Material::GetEmitAmount()
{
	return emitAmount;
}

DirectX::XMFLOAT3 Material::GetOutlineTint()
{
	return outlineTint;
}

DirectX::XMFLOAT3 Material::GetRimTint()
{
	return rimTint;
}

std::shared_ptr<SimpleVertexShader> Material::GetVertexShader()
{
	return vertexShader;
}

std::shared_ptr<SimplePixelShader> Material::GetPixelShader()
{
	return pixelShader;
}
#pragma endregion

#pragma region Setters
void Material::SetTint(DirectX::XMFLOAT3 _tint)
{
	tint = _tint;
}

void Material::SetUVScale(DirectX::XMFLOAT2 _scale)
{
	uvScale = _scale;
}

void Material::SetUVOffset(DirectX::XMFLOAT2 _offset)
{
	uvOffset = _offset;
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

void Material::SetAlpha(float _alpha)
{
	alpha = _alpha;
}

void Material::SetCutoff(float _cutoff)
{
	cutoff = _cutoff;
}

void Material::SetNormalIntensity(float _intensity)
{
	normalIntensity = _intensity;
}

void Material::SetRimCutoff(float _cutoff)
{
	rimCutoff = _cutoff;
}

void Material::SetOutlineThickness(float _thickness)
{
	outlineThickness = _thickness;
}

void Material::SetEmitAmount(DirectX::XMFLOAT3 _emit)
{
	emitAmount = _emit;
}

void Material::SetOutlineTint(DirectX::XMFLOAT3 _tint)
{
	outlineTint = _tint;
}

void Material::SetRimTint(DirectX::XMFLOAT3 _tint)
{
	rimTint = _tint;
}

void Material::SetVertexShader(std::shared_ptr<SimpleVertexShader> _vertexShader)
{
	vertexShader = _vertexShader;
}

void Material::SetPixelShader(std::shared_ptr<SimplePixelShader> _pixelShader)
{
	pixelShader = _pixelShader;
}
#pragma endregion

#pragma region Utility
void Material::LoadTexture(const wchar_t* _path, const char* _type, ID3D11Device* _device, ID3D11DeviceContext* _context)
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	DirectX::CreateWICTextureFromFile(_device, _context, DXCore::GetFullPathTo_Wide(_path).c_str(), 0, shaderResourceView.GetAddressOf());
	PushTexture(_type, shaderResourceView);

	if (_type == TEXTYPE_ALBEDO) hasAlbedoMap = true;
	else if (_type == TEXTYPE_EMISSIVE) hasEmissiveMap = true;
	else if (_type == TEXTYPE_SPECULAR) hasSpecularMap = true;
	else if (_type == TEXTYPE_NORMAL) hasNormalMap = true;
	else if (_type == TEXTYPE_REFLECTION) hasReflectionMap = true;
	else if (_type == TEXTYPE_RAMPDIFFUSE) hasRampDiffuse = true;
	else if (_type == TEXTYPE_RAMPSPECULAR) hasRampSpecular = true;
}

void Material::PushSampler(std::string _name, Microsoft::WRL::ComPtr<ID3D11SamplerState> _sampler)
{
	samplers.insert({ _name, _sampler });
}

void Material::PushTexture(std::string _name, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _texture)
{
	textures.insert({ _name, _texture });
}
#pragma endregion

#pragma region Internal Material Activation
void Material::ActivateStandard(Transform* _transform, std::shared_ptr<Camera> _camera, DirectX::XMFLOAT3 _ambient, std::vector<Light> _lights)
{
	vertexShader->SetMatrix4x4("world", _transform->GetWorldMatrix());
	vertexShader->SetMatrix4x4("worldInvTranspose", _transform->GetWorldMatrixInverseTranspose());
	vertexShader->SetMatrix4x4("view", _camera->GetViewMatrix());
	vertexShader->SetMatrix4x4("projection", _camera->GetProjectionMatrix());
	vertexShader->CopyAllBufferData();
	vertexShader->SetShader();

	pixelShader->SetFloat3("cameraPosition", _camera->GetTransform()->GetPosition());
	pixelShader->SetFloat("roughness", GetRoughness());
	pixelShader->SetFloat("normalIntensity", GetNormalIntensity());
	pixelShader->SetFloat("alpha", GetAlpha());
	pixelShader->SetFloat("cutoff", GetCutoff());
	pixelShader->SetFloat2("scale", GetUVScale());
	pixelShader->SetFloat2("offset", GetUVOffset());
	pixelShader->SetFloat3("ambient", _ambient);
	pixelShader->SetFloat3("emitAmount", GetEmitAmount());
	pixelShader->SetFloat3("tint", GetTint());
	pixelShader->SetFloat("lightCount", (int)_lights.size());
	pixelShader->SetInt("hasAlbedoMap", (int)hasAlbedoMap);
	pixelShader->SetInt("hasEmissiveMap", (int)hasEmissiveMap);
	pixelShader->SetInt("hasSpecularMap", (int)hasSpecularMap);
	pixelShader->SetInt("hasNormalMap", (int)hasNormalMap);
	pixelShader->SetInt("hasReflectionMap", (int)hasReflectionMap);
	pixelShader->SetData("lights", &_lights[0], sizeof(Light) * (int)_lights.size());
	pixelShader->CopyAllBufferData();
	pixelShader->SetShader();

	for (auto& t : textures)
	{
		pixelShader->SetShaderResourceView(t.first.c_str(), t.second.Get());
	}
	for (auto& s : samplers)
	{
		pixelShader->SetSamplerState(s.first.c_str(), s.second.Get());
	}
}

void Material::ActivatePBR(Transform* _transform, std::shared_ptr<Camera> _camera, DirectX::XMFLOAT3 _ambient, std::vector<Light> _lights)
{
	vertexShader->SetMatrix4x4("world", _transform->GetWorldMatrix());
	vertexShader->SetMatrix4x4("worldInvTranspose", _transform->GetWorldMatrixInverseTranspose());
	vertexShader->SetMatrix4x4("view", _camera->GetViewMatrix());
	vertexShader->SetMatrix4x4("projection", _camera->GetProjectionMatrix());
	vertexShader->CopyAllBufferData();
	vertexShader->SetShader();

	pixelShader->SetFloat2("scale", GetUVScale());
	pixelShader->SetFloat2("offset", GetUVOffset());
	pixelShader->SetFloat3("cameraPosition", _camera->GetTransform()->GetPosition());
	pixelShader->SetFloat("normalIntensity", GetNormalIntensity());
	pixelShader->SetFloat("lightCount", (int)_lights.size());
	pixelShader->SetData("lights", &_lights[0], sizeof(Light) * (int)_lights.size());
	pixelShader->CopyAllBufferData();
	pixelShader->SetShader();

	for (auto& t : textures)
	{
		pixelShader->SetShaderResourceView(t.first.c_str(), t.second.Get());
	}
	for (auto& s : samplers)
	{
		pixelShader->SetSamplerState(s.first.c_str(), s.second.Get());
	}
}

void Material::ActivateToon(Transform* _transform, std::shared_ptr<Camera> _camera, DirectX::XMFLOAT3 _ambient, std::vector<Light> _lights)
{
	vertexShader->SetMatrix4x4("world", _transform->GetWorldMatrix());
	vertexShader->SetMatrix4x4("worldInvTranspose", _transform->GetWorldMatrixInverseTranspose());
	vertexShader->SetMatrix4x4("view", _camera->GetViewMatrix());
	vertexShader->SetMatrix4x4("projection", _camera->GetProjectionMatrix());
	vertexShader->CopyAllBufferData();
	vertexShader->SetShader();

	pixelShader->SetFloat3("cameraPosition", _camera->GetTransform()->GetPosition());
	pixelShader->SetFloat("roughness", GetRoughness());
	pixelShader->SetFloat("normalIntensity", GetNormalIntensity());
	pixelShader->SetFloat("alpha", GetAlpha());
	pixelShader->SetFloat("cutoff", GetCutoff());
	pixelShader->SetFloat2("scale", GetUVScale());
	pixelShader->SetFloat2("offset", GetUVOffset());
	pixelShader->SetFloat3("ambient", _ambient);
	pixelShader->SetFloat3("emitAmount", GetEmitAmount());
	pixelShader->SetFloat3("tint", GetTint());
	pixelShader->SetFloat3("rimTint", GetRimTint());
	pixelShader->SetFloat3("outlineTint", GetOutlineTint());
	pixelShader->SetFloat("outlineThickness", GetOutlineThickness());
	pixelShader->SetFloat("rimCutoff", GetRimCutoff());
	pixelShader->SetFloat("lightCount", (int)_lights.size());
	pixelShader->SetInt("hasAlbedoMap", (int)hasAlbedoMap);
	pixelShader->SetInt("hasEmissiveMap", (int)hasEmissiveMap);
	pixelShader->SetInt("hasSpecularMap", (int)hasSpecularMap);
	pixelShader->SetInt("hasNormalMap", (int)hasNormalMap);
	pixelShader->SetInt("hasRampDiffuse", (int)hasRampDiffuse);
	pixelShader->SetInt("hasRampSpecular", (int)hasRampSpecular);
	pixelShader->SetData("lights", &_lights[0], sizeof(Light) * (int)_lights.size());
	pixelShader->CopyAllBufferData();
	pixelShader->SetShader();

	for (auto& t : textures)
	{
		pixelShader->SetShaderResourceView(t.first.c_str(), t.second.Get());
	}
	for (auto& s : samplers)
	{
		pixelShader->SetSamplerState(s.first.c_str(), s.second.Get());
	}
}
#pragma endregion
