#pragma once

#include "DXCore.h"
#include "Camera.h"
#include "Mesh.h"
#include "Entity.h"
#include "SimpleShader.h"
#include "Material.h"
#include "Lights.h"
#include "Sky.h"
#include <DirectXMath.h>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include <memory>
#include <vector>

class Game 
	: public DXCore
{
public:
	Game(HINSTANCE hInstance);
	~Game();

	void															Init();
	void															OnResize();
	void															Update(float deltaTime, float totalTime);
	void															Draw(float deltaTime, float totalTime);

	static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>			CreateCubemap(
																		Microsoft::WRL::ComPtr<ID3D11Device> _device,
																		Microsoft::WRL::ComPtr<ID3D11DeviceContext> _context,
																		const wchar_t* _right,
																		const wchar_t* _left,
																		const wchar_t* _up,
																		const wchar_t* _down,
																		const wchar_t* _front,
																		const wchar_t* _back);

private:
	// Should we use vsync to limit the frame rate?
	bool vsync;

	void LoadShadersAndMaterials();
	void LoadTextures();
	void LoadLighting();
	void CreateBasicGeometry();
	
	// Shaders and shader-related constructs
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimplePixelShader> pixelShaderPBR;
	std::shared_ptr<SimpleVertexShader> vertexShaderPBR;
	std::shared_ptr<SimplePixelShader> pixelShaderToon;

	// A2 shapes
	std::vector<std::shared_ptr<Mesh>> shapes;
	// A4 entities;
	std::vector<std::shared_ptr<Entity>> entities;
	// A5 Camera
	std::shared_ptr<Camera> camera;
	// A6 Materials
	std::vector<std::shared_ptr<Material>> materials;
	// A7 Lights
	std::vector<Light> lights;
	DirectX::XMFLOAT3 ambient;
	// A9 Normalmaps & Cubemaps
	std::shared_ptr<Sky> skybox;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> demoCubemap;

	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBufferVS;
	Microsoft::WRL::ComPtr<ID3D11BlendState> alphaBlendState;
};

