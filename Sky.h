#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
#include "Mesh.h"
#include "SimpleShader.h"
#include "Camera.h"

class Sky
{
public:
	Sky(
		std::shared_ptr<Mesh>											_mesh,
		std::shared_ptr<SimpleVertexShader>								_vertexShader,
		std::shared_ptr<SimplePixelShader>								_pixelShader,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>				_cubemap,
		Microsoft::WRL::ComPtr<ID3D11SamplerState>						_sampler,
		Microsoft::WRL::ComPtr<ID3D11Device>							_device);
	~Sky();

	void			Draw(
						Microsoft::WRL::ComPtr<ID3D11DeviceContext>		_context,
						std::shared_ptr<Camera>							_camera);

private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState>				sampler;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>			depthState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>			rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		cubemap;
	std::shared_ptr<Mesh>									mesh;
	std::shared_ptr<SimpleVertexShader>						vertexShader;
	std::shared_ptr<SimplePixelShader>						pixelShader;
};
