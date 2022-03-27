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

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

private:

	// Should we use vsync to limit the frame rate?
	bool vsync;

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders();
	void LoadTextures();
	void LoadLighting();
	void CreateBasicGeometry();

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//    Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr
	
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

