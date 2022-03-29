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
	uvOffset = DirectX::XMFLOAT2(0, 0);
	uvScale = DirectX::XMFLOAT2(1, 1);
}

Material::~Material()
{
}

void Material::Activate(Transform* _transform, std::shared_ptr<Camera> _camera, DirectX::XMFLOAT3 _ambient, std::vector<Light> _lights)
{
	vertexShader->SetMatrix4x4("world", _transform->GetWorldMatrix());
	vertexShader->SetMatrix4x4("worldInvTranspose", _transform->GetWorldMatrixInverseTranspose());
	vertexShader->SetMatrix4x4("view", _camera->GetViewMatrix());
	vertexShader->SetMatrix4x4("projection", _camera->GetProjectionMatrix());
	vertexShader->CopyAllBufferData();
	vertexShader->SetShader();

	pixelShader->SetFloat3("cameraPosition", _camera->GetTransform()->GetPosition());
	pixelShader->SetFloat("roughness", GetRoughness());
	pixelShader->SetFloat2("scale", GetUVScale());
	pixelShader->SetFloat2("offset", GetUVOffset());
	pixelShader->SetFloat3("ambient", _ambient);
	pixelShader->SetFloat("emitAmount", GetEmitAmount());
	pixelShader->SetFloat3("tint", GetTint());
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

float Material::GetEmitAmount()
{
	return emitAmount;
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

void Material::SetEmitAmount(float _emit)
{
	emitAmount = _emit;
}

void Material::SetVertexShader(std::shared_ptr<SimpleVertexShader> _vertexShader)
{
	vertexShader = _vertexShader;
}

void Material::SetPixelShader(std::shared_ptr<SimplePixelShader> _pixelShader)
{
	pixelShader = _pixelShader;
}

void Material::LoadTexture(const wchar_t* _path, const char* _type, ID3D11Device* _device, ID3D11DeviceContext* _context)
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	DirectX::CreateWICTextureFromFile(_device, _context, DXCore::GetFullPathTo_Wide(_path).c_str(), 0, shaderResourceView.GetAddressOf());
	PushTexture(_type, shaderResourceView);
}

void Material::PushSampler(std::string _name, Microsoft::WRL::ComPtr<ID3D11SamplerState> _sampler)
{
	samplers.insert({ _name, _sampler });
}

void Material::PushTexture(std::string _name, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _texture)
{
	textures.insert({ _name, _texture });
}
