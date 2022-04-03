#pragma once

#include "DXCore.h"
#include "Camera.h"
#include "Mesh.h"
#include "Entity.h"
#include "SimpleShader.h"
#include "Material.h"
#include "Lights.h"
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

	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

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

	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBufferVS;
};

