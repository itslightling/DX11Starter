#include "Sky.h"

Sky::Sky(std::shared_ptr<Mesh> _mesh, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _cubemap, Microsoft::WRL::ComPtr<ID3D11SamplerState> _sampler, Microsoft::WRL::ComPtr<ID3D11Device> _device)
{
	mesh = _mesh;
	cubemap = _cubemap;
	sampler = _sampler;

	D3D11_RASTERIZER_DESC rDesc = {};
	rDesc.FillMode = D3D11_FILL_SOLID;
	rDesc.CullMode = D3D11_CULL_FRONT;
	_device->CreateRasterizerState(&rDesc, rasterizerState.GetAddressOf());

	D3D11_DEPTH_STENCIL_DESC dDesc = {};
	dDesc.DepthEnable = true;
	dDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	_device->CreateDepthStencilState(&dDesc, depthState.GetAddressOf());
}

Sky::~Sky()
{
}

void Sky::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> _context, Camera* _camera)
{
	_context->RSSetState(rasterizerState.Get());
	_context->OMSetDepthStencilState(depthState.Get(), 0);

	vertexShader->SetMatrix4x4("view", _camera->GetViewMatrix());
	vertexShader->SetMatrix4x4("projection", _camera->GetProjectionMatrix());
	vertexShader->CopyAllBufferData();
	vertexShader->SetShader();

	pixelShader->SetShaderResourceView("SkyTexture", cubemap.Get());
	pixelShader->SetSamplerState("Sampler", sampler.Get());
	pixelShader->CopyAllBufferData();
	pixelShader->SetShader();

	mesh->Draw();

	_context->RSSetState(0);
	_context->OMSetDepthStencilState(0, 0);
}
