#include "Game.h"
#include "Vertex.h"
#include "Input.h"
#include "SimpleShader.h"
#include <algorithm>

// Needed for a helper function to read compiled shader files from the hard drive
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>
#include <iostream>
#include "WICTextureLoader.h"

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true),			   // Show extra stats (fps) in title bar?
	vsync(false)
{
#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif
	camera = std::make_shared<Camera>(0.0f, 5.0f, -15.0f, (float)width / height, 60, 0.01f, 1000.0f, 5.0f);
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	// Note: Since we're using smart pointers (ComPtr),
	// we don't need to explicitly clean up those DirectX objects
	// - If we weren't using smart pointers, we'd need
	//   to call Release() on each DirectX object created in Game
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	LoadShadersAndMaterials();
	LoadTextures();
	LoadMeshes();
	LoadScene(0);
	
	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files and pushes them to materials
// --------------------------------------------------------
void Game::LoadShadersAndMaterials()
{
	vertexShader = std::make_shared<SimpleVertexShader>(device, context, GetFullPathTo_Wide(L"VertexShader.cso").c_str());
	pixelShader = std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"SimplePixelShader.cso").c_str());
	vertexShaderPBR = std::make_shared<SimpleVertexShader>(device, context, GetFullPathTo_Wide(L"SimpleVertexPBR.cso").c_str());
	pixelShaderPBR = std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"SimplePixelPBR.cso").c_str());
	pixelShaderToon = std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"ToonShader.cso").c_str());

	XMFLOAT3 white = XMFLOAT3(1.0f, 1.0f, 1.0f);
	XMFLOAT3 deepPurple = XMFLOAT3(0.1f, 0.02f, 0.1f);

	materials = {
		std::make_shared<Material>(MATTYPE_STANDARD, white, 0, vertexShader, pixelShader), //0: blueish bronze material with reflection map
		std::make_shared<Material>(MATTYPE_PBR, white, 0, vertexShaderPBR, pixelShaderPBR), //1: bronze PBR
		std::make_shared<Material>(MATTYPE_PBR, white, 0, vertexShaderPBR, pixelShaderPBR), //2: cobblestone PBR
		std::make_shared<Material>(MATTYPE_PBR, white, 0, vertexShaderPBR, pixelShaderPBR), //3: metallic diamond-pattern floor PBR
		std::make_shared<Material>(MATTYPE_PBR, white, 0, vertexShaderPBR, pixelShaderPBR), //4: paint PBR
		std::make_shared<Material>(MATTYPE_PBR, white, 0, vertexShaderPBR, pixelShaderPBR), //5: rough metal PBR
		std::make_shared<Material>(MATTYPE_PBR, white, 0, vertexShaderPBR, pixelShaderPBR), //6: scratched metal PBR
		std::make_shared<Material>(MATTYPE_PBR, white, 0, vertexShaderPBR, pixelShaderPBR), //7: wood PBR
		std::make_shared<Material>(MATTYPE_STANDARD, white, 0, vertexShader, pixelShader), //8: transparent floor grate for scene 1
		std::make_shared<Material>(MATTYPE_TOON, white, 0, vertexShader, pixelShaderToon), //9: cushion toon
		std::make_shared<Material>(MATTYPE_TOON, deepPurple, 0, vertexShader, pixelShaderToon), //10: emissive lava toon
		std::make_shared<Material>(MATTYPE_STANDARD, white, 0, vertexShader, pixelShader), //11: lava toon
		std::make_shared<Material>(MATTYPE_PBR, white, 0, vertexShader, pixelShaderPBR), //12: fence PBR
		std::make_shared<Material>(MATTYPE_STANDARD, white, 0, vertexShader, pixelShader), //13: transparent floor grate for scene 2
	};
}

// --------------------------------------------------------
// Loads textures and pushes them to the loaded materials
// --------------------------------------------------------
void Game::LoadTextures()
{
	#pragma region Sampler Initialization
	// Sampler description for wrapped texture sampling
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.MaxAnisotropy = 16;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&sampDesc, sampler.GetAddressOf());

	// Blend description for alpha support
	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	device->CreateBlendState(&blendDesc, alphaBlendState.GetAddressOf());

	// Rasterizer description for alpha support/rendering backfaces only
	D3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.DepthClipEnable = true;
	rastDesc.CullMode = D3D11_CULL_FRONT;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	device->CreateRasterizerState(&rastDesc, backfaceRasterState.GetAddressOf());

	// Sampler description for clamping
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	device->CreateSamplerState(&sampDesc, clampSampler.GetAddressOf());
	#pragma endregion

	#pragma region Cubemap Setup
	demoCubemap1 = CreateCubemap(
		device,
		context,
		L"Assets/Textures/Skies/xenskybox/right.png",
		L"Assets/Textures/Skies/xenskybox/left.png",
		L"Assets/Textures/Skies/xenskybox/top.png",
		L"Assets/Textures/Skies/xenskybox/bottom.png",
		L"Assets/Textures/Skies/xenskybox/front.png",
		L"Assets/Textures/Skies/xenskybox/back.png"
	);

	demoCubemap2 = CreateCubemap(
		device,
		context,
		L"Assets/Textures/Skies/planets/right.png",
		L"Assets/Textures/Skies/planets/left.png",
		L"Assets/Textures/Skies/planets/up.png",
		L"Assets/Textures/Skies/planets/down.png",
		L"Assets/Textures/Skies/planets/front.png",
		L"Assets/Textures/Skies/planets/back.png"
	);
	#pragma endregion

	#pragma region Material Setup
	materials[0]->PushSampler("BasicSampler", sampler);
	materials[0]->PushTexture(TEXTYPE_REFLECTION, demoCubemap1);
	materials[0]->hasReflectionMap = true;
	materials[0]->LoadTexture(L"Assets/Textures/PBR/bronze_albedo.png", TEXTYPE_ALBEDO, device.Get(), context.Get());
	materials[0]->LoadTexture(L"Assets/Textures/PBR/bronze_normals.png", TEXTYPE_NORMAL, device.Get(), context.Get());
	materials[0]->LoadTexture(L"Assets/Textures/PBR/bronze_roughness.png", TEXTYPE_SPECULAR, device.Get(), context.Get());
	materials[0]->SetNormalIntensity(2.5f);
	materials[0]->SetTint(DirectX::XMFLOAT3(0.25f, 0.25f, 0.85f));

	materials[1]->PushSampler("BasicSampler", sampler);
	materials[1]->LoadTexture(L"Assets/Textures/PBR/bronze_albedo.png", TEXTYPE_ALBEDO, device.Get(), context.Get());
	materials[1]->LoadTexture(L"Assets/Textures/PBR/bronze_normals.png", TEXTYPE_NORMAL, device.Get(), context.Get());
	materials[1]->LoadTexture(L"Assets/Textures/PBR/bronze_roughness.png", TEXTYPE_ROUGHNESS, device.Get(), context.Get());
	materials[1]->LoadTexture(L"Assets/Textures/PBR/bronze_metal.png", TEXTYPE_METALNESS, device.Get(), context.Get());
	materials[1]->SetNormalIntensity(2.5f);

	materials[2]->PushSampler("BasicSampler", sampler);
	materials[2]->LoadTexture(L"Assets/Textures/PBR/cobblestone_albedo.png", TEXTYPE_ALBEDO, device.Get(), context.Get());
	materials[2]->LoadTexture(L"Assets/Textures/PBR/cobblestone_normals.png", TEXTYPE_NORMAL, device.Get(), context.Get());
	materials[2]->LoadTexture(L"Assets/Textures/PBR/cobblestone_roughness.png", TEXTYPE_ROUGHNESS, device.Get(), context.Get());
	materials[2]->LoadTexture(L"Assets/Textures/PBR/cobblestone_metal.png", TEXTYPE_METALNESS, device.Get(), context.Get());

	materials[3]->PushSampler("BasicSampler", sampler);
	materials[3]->LoadTexture(L"Assets/Textures/PBR/floor_albedo.png", TEXTYPE_ALBEDO, device.Get(), context.Get());
	materials[3]->LoadTexture(L"Assets/Textures/PBR/floor_normals.png", TEXTYPE_NORMAL, device.Get(), context.Get());
	materials[3]->LoadTexture(L"Assets/Textures/PBR/floor_roughness.png", TEXTYPE_ROUGHNESS, device.Get(), context.Get());
	materials[3]->LoadTexture(L"Assets/Textures/PBR/floor_metal.png", TEXTYPE_METALNESS, device.Get(), context.Get());

	materials[4]->PushSampler("BasicSampler", sampler);
	materials[4]->LoadTexture(L"Assets/Textures/PBR/paint_albedo.png", TEXTYPE_ALBEDO, device.Get(), context.Get());
	materials[4]->LoadTexture(L"Assets/Textures/PBR/paint_normals.png", TEXTYPE_NORMAL, device.Get(), context.Get());
	materials[4]->LoadTexture(L"Assets/Textures/PBR/paint_roughness.png", TEXTYPE_ROUGHNESS, device.Get(), context.Get());
	materials[4]->LoadTexture(L"Assets/Textures/PBR/paint_metal.png", TEXTYPE_METALNESS, device.Get(), context.Get());
	materials[4]->SetNormalIntensity(0.5f);

	materials[5]->PushSampler("BasicSampler", sampler);
	materials[5]->LoadTexture(L"Assets/Textures/PBR/rough_albedo.png", TEXTYPE_ALBEDO, device.Get(), context.Get());
	materials[5]->LoadTexture(L"Assets/Textures/PBR/rough_normals.png", TEXTYPE_NORMAL, device.Get(), context.Get());
	materials[5]->LoadTexture(L"Assets/Textures/PBR/rough_roughness.png", TEXTYPE_ROUGHNESS, device.Get(), context.Get());
	materials[5]->LoadTexture(L"Assets/Textures/PBR/rough_metal.png", TEXTYPE_METALNESS, device.Get(), context.Get());
	materials[5]->SetNormalIntensity(3.5f);

	materials[6]->PushSampler("BasicSampler", sampler);
	materials[6]->LoadTexture(L"Assets/Textures/PBR/scratched_albedo.png", TEXTYPE_ALBEDO, device.Get(), context.Get());
	materials[6]->LoadTexture(L"Assets/Textures/PBR/scratched_normals.png", TEXTYPE_NORMAL, device.Get(), context.Get());
	materials[6]->LoadTexture(L"Assets/Textures/PBR/scratched_roughness.png", TEXTYPE_ROUGHNESS, device.Get(), context.Get());
	materials[6]->LoadTexture(L"Assets/Textures/PBR/scratched_metal.png", TEXTYPE_METALNESS, device.Get(), context.Get());

	materials[7]->PushSampler("BasicSampler", sampler);
	materials[7]->LoadTexture(L"Assets/Textures/PBR/wood_albedo.png", TEXTYPE_ALBEDO, device.Get(), context.Get());
	materials[7]->LoadTexture(L"Assets/Textures/PBR/wood_normals.png", TEXTYPE_NORMAL, device.Get(), context.Get());
	materials[7]->LoadTexture(L"Assets/Textures/PBR/wood_roughness.png", TEXTYPE_ROUGHNESS, device.Get(), context.Get());
	materials[7]->LoadTexture(L"Assets/Textures/PBR/wood_metal.png", TEXTYPE_METALNESS, device.Get(), context.Get());
	materials[7]->SetNormalIntensity(3.5f);

	materials[8]->PushSampler("BasicSampler", sampler);
	materials[8]->LoadTexture(L"Assets/Textures/HQGame/structure-endgame-floor_albedo.png", TEXTYPE_ALBEDO, device.Get(), context.Get());
	materials[8]->LoadTexture(L"Assets/Textures/HQGame/structure-endgame-floor_specular.png", TEXTYPE_SPECULAR, device.Get(), context.Get());
	// this texture has some weird noise artifacts in the holes of the floor that I probably just never noticed
	// when I used it in the game I made it for because it was also had Cutoff in Unity. add high cutoff, but not too high for distant mipmaps
	materials[8]->SetCutoff(0.9f);
	// this is just to test alpha
	materials[8]->SetAlpha(0.8f);

	materials[9]->PushSampler("BasicSampler", sampler);
	materials[9]->PushSampler("ClampSampler", clampSampler);
	materials[9]->SetRoughness(1);
	materials[9]->LoadTexture(L"Assets/Textures/WithNormals/cushion.png", TEXTYPE_ALBEDO, device.Get(), context.Get());
	materials[9]->LoadTexture(L"Assets/Textures/WithNormals/cushion_normals.png", TEXTYPE_NORMAL, device.Get(), context.Get());
	materials[9]->LoadTexture(L"Assets/Textures/WithNormals/cushion_specular.png", TEXTYPE_SPECULAR, device.Get(), context.Get());
	materials[9]->SetOutlineThickness(0);

	materials[10]->PushSampler("BasicSampler", sampler);
	materials[10]->PushSampler("ClampSampler", clampSampler);
	materials[10]->LoadTexture(L"Assets/Textures/HQGame/structure-endgame-deepfloor_emissive.png", TEXTYPE_EMISSIVE, device.Get(), context.Get());
	materials[10]->LoadTexture(L"Assets/Textures/Ramps/toonRamp3.png", TEXTYPE_RAMPDIFFUSE, device.Get(), context.Get());
	materials[10]->LoadTexture(L"Assets/Textures/Ramps/toonRampSpecular.png", TEXTYPE_RAMPSPECULAR, device.Get(), context.Get());
	materials[10]->SetRimCutoff(0.15f);
	materials[10]->SetEmitAmount(XMFLOAT3(0.05f, 0.1f, 0.01f));

	materials[11]->PushSampler("BasicSampler", sampler);
	materials[11]->LoadTexture(L"Assets/Textures/HQGame/structure-endgame-deepfloor_albedo.png", TEXTYPE_ALBEDO, device.Get(), context.Get());
	materials[11]->LoadTexture(L"Assets/Textures/HQGame/structure-endgame-deepfloor_specular.png", TEXTYPE_SPECULAR, device.Get(), context.Get());
	materials[11]->LoadTexture(L"Assets/Textures/HQGame/structure-endgame-deepfloor_emissive.png", TEXTYPE_EMISSIVE, device.Get(), context.Get());
	materials[11]->SetEmitAmount(XMFLOAT3(0.05f, 0.1f, 0.01f));

	materials[12]->PushSampler("BasicSampler", sampler);
	materials[12]->LoadTexture(L"Assets/Textures/Transparent/fence_albedo.png", TEXTYPE_ALBEDO, device.Get(), context.Get());
	materials[12]->LoadTexture(L"Assets/Textures/Transparent/fence_normals.png", TEXTYPE_NORMAL, device.Get(), context.Get());
	materials[12]->LoadTexture(L"Assets/Textures/Transparent/fence_roughness.png", TEXTYPE_ROUGHNESS, device.Get(), context.Get());
	materials[12]->LoadTexture(L"Assets/Textures/Transparent/fence_metal.png", TEXTYPE_METALNESS, device.Get(), context.Get());
	materials[12]->SetCutoff(0.95f);

	materials[13]->PushSampler("BasicSampler", sampler);
	materials[13]->LoadTexture(L"Assets/Textures/HQGame/structure-endgame-floor_albedo.png", TEXTYPE_ALBEDO, device.Get(), context.Get());
	materials[13]->LoadTexture(L"Assets/Textures/HQGame/structure-endgame-floor_specular.png", TEXTYPE_SPECULAR, device.Get(), context.Get());
	materials[13]->SetAlpha(0.85f);
	materials[13]->SetCutoff(0.95f);
	#pragma endregion
}

// --------------------------------------------------------
// Loads the geometry we're going to draw
// --------------------------------------------------------
void Game::LoadMeshes()
{
	shapes = {
		std::make_shared<Mesh>(
			GetFullPathTo("Assets/Models/cube.obj").c_str(),
			device, context),
		std::make_shared<Mesh>(
			GetFullPathTo("Assets/Models/cylinder.obj").c_str(),
			device, context),
		std::make_shared<Mesh>(
			GetFullPathTo("Assets/Models/helix.obj").c_str(),
			device, context),
		std::make_shared<Mesh>(
			GetFullPathTo("Assets/Models/sphere.obj").c_str(),
			device, context),
		std::make_shared<Mesh>(
			GetFullPathTo("Assets/Models/torus.obj").c_str(),
			device, context),
		std::make_shared<Mesh>(
			GetFullPathTo("Assets/Models/quad.obj").c_str(),
			device, context),
		std::make_shared<Mesh>(
			GetFullPathTo("Assets/Models/quad_double_sided.obj").c_str(),
			device, context),

		std::make_shared<Mesh>(
			GetFullPathTo("Assets/Models/warped_plane.obj").c_str(),
			device, context),
		std::make_shared<Mesh>(
			GetFullPathTo("Assets/Models/warped_building.obj").c_str(),
			device, context),
		std::make_shared<Mesh>(
			GetFullPathTo("Assets/Models/warped_archway_outer.obj").c_str(),
			device, context),
		std::make_shared<Mesh>(
			GetFullPathTo("Assets/Models/warped_archway_inner.obj").c_str(),
			device, context),
		std::make_shared<Mesh>(
			GetFullPathTo("Assets/Models/warped_monke.obj").c_str(),
			device, context),
	};

	skybox1 = std::make_shared<Sky>(
		shapes[0],
		std::make_shared<SimpleVertexShader>(device, context, GetFullPathTo_Wide(L"SkyboxVertexShader.cso").c_str()),
		std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"SkyboxPixelShader.cso").c_str()),
		demoCubemap1,
		sampler,
		device
	);

	skybox2 = std::make_shared<Sky>(
		shapes[0],
		std::make_shared<SimpleVertexShader>(device, context, GetFullPathTo_Wide(L"SkyboxVertexShader.cso").c_str()),
		std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"SkyboxPixelShader.cso").c_str()),
		demoCubemap2,
		sampler,
		device
	);
}

// --------------------------------------------------------
// Loads the entities, lighting, etc. based on scene
// --------------------------------------------------------
void Game::LoadScene(int _currentScene)
{
	currentScene = _currentScene;
	switch (currentScene)
	{
	case 0:
		LoadScene1();
		break;
	case 1:
		LoadScene2();
		break;
	}
}

void Game::LoadScene1()
{
	camera->GetTransform()->SetPosition(0.0f, 9.0f, -15.0f);
	camera->GetTransform()->SetRotation(0.1f, 0, 0);

	ambient = XMFLOAT3(0.01f, 0.01f, 0.015f);

	lights = {
		Light::Directional(XMFLOAT3(1, 0.5f, -0.5f), XMFLOAT3(1, 1, 1), 1.0f),
	};

	#pragma region Entity Definition
	entities = {
		std::make_shared<Entity>(materials[2], shapes[7]), //0
		std::make_shared<Entity>(materials[0], shapes[8]), //1
		std::make_shared<Entity>(materials[0], shapes[8]), //2
		std::make_shared<Entity>(materials[0], shapes[8]), //3
		std::make_shared<Entity>(materials[0], shapes[8]), //4
		std::make_shared<Entity>(materials[7], shapes[9]), //5
		std::make_shared<Entity>(materials[11], shapes[10]), //6
		std::make_shared<Entity>(materials[10], shapes[11]), //7
	};

	transpEntities = {
		std::make_shared<Entity>(materials[12], shapes[5]), //0
		std::make_shared<Entity>(materials[12], shapes[5]), //1
		std::make_shared<Entity>(materials[12], shapes[5]), //2
		std::make_shared<Entity>(materials[13], shapes[3]), //3
		std::make_shared<Entity>(materials[13], shapes[3]), //4
		std::make_shared<Entity>(materials[13], shapes[3]), //5
	};
	#pragma endregion

	#pragma region Transform Setup
	entities[1]->GetTransform()->SetPosition(-5, 0, 5);
	entities[2]->GetTransform()->SetPosition(5, 0, 5);
	entities[3]->GetTransform()->SetPosition(5, 0, -5);
	entities[4]->GetTransform()->SetPosition(-5, 0, -5);
	entities[5]->GetTransform()->SetPosition(0, 3, 5);

	entities[6]->GetTransform()->SetPosition(0, 3, 5);
	entities[5]->GetTransform()->SetRotation(0, 1.57f, 0);
	entities[6]->GetTransform()->SetRotation(0, 1.57f, 0);
	entities[5]->GetTransform()->SetScale(0.75f, 0.75f, 0.75f);
	entities[6]->GetTransform()->SetScale(0.75f, 0.75f, 0.75f);

	entities[7]->GetTransform()->SetPosition(0, 20, 20);
	entities[7]->GetTransform()->SetScale(8, 8, 8);
	entities[7]->GetTransform()->SetRotation(-0.5f, 0, 0);

	transpEntities[0]->GetTransform()->SetPosition(0, 1, 5);
	transpEntities[0]->GetTransform()->SetRotation(1.57079f, 0, 0);
	transpEntities[0]->GetTransform()->SetScale(6, 6, 1);
	transpEntities[1]->GetTransform()->SetPosition(-5, 1, 0);
	transpEntities[1]->GetTransform()->SetRotation(1.57079f, 1.57079f, 0);
	transpEntities[1]->GetTransform()->SetScale(6, 6, 1);
	transpEntities[2]->GetTransform()->SetPosition(5, 1, 0);
	transpEntities[2]->GetTransform()->SetRotation(1.57079f, -1.57079f, 0);
	transpEntities[2]->GetTransform()->SetScale(6, 6, 1);

	transpEntities[3]->GetTransform()->SetScale(-30, -30, -30);
	transpEntities[4]->GetTransform()->SetScale(-60, -60, -60);
	transpEntities[5]->GetTransform()->SetScale(-90, -90, -90);
	#pragma endregion

	materials[0]->SwapTexture(TEXTYPE_REFLECTION, demoCubemap1);
	materials[0]->SetUVScale(DirectX::XMFLOAT2(10, 10));
	materials[2]->SetUVScale(DirectX::XMFLOAT2(5, 5));
}

void Game::LoadScene2()
{
	camera->GetTransform()->SetPosition(0.0f, 0.0f, -10.0f);
	camera->GetTransform()->SetRotation(0, 0, 0);

	ambient = XMFLOAT3(0.01f, 0.01f, 0.015f);

	lights = {
		Light::Directional(XMFLOAT3(1, 0.5f, -0.5f), XMFLOAT3(1, 1, 1), 1.0f),
	};

	#pragma region Entity Definition
	entities = {
		// PBR
		std::make_shared<Entity>(materials[1], shapes[3]),
		std::make_shared<Entity>(materials[2], shapes[3]),
		std::make_shared<Entity>(materials[3], shapes[3]),
		std::make_shared<Entity>(materials[4], shapes[3]),
		std::make_shared<Entity>(materials[5], shapes[3]),
		std::make_shared<Entity>(materials[6], shapes[3]),
		std::make_shared<Entity>(materials[7], shapes[3]),
		// std
		std::make_shared<Entity>(materials[0], shapes[3]),
		// toon
		std::make_shared<Entity>(materials[9], shapes[3]),
		std::make_shared<Entity>(materials[9], shapes[3]),
		std::make_shared<Entity>(materials[9], shapes[3]),
		std::make_shared<Entity>(materials[9], shapes[3]),
		std::make_shared<Entity>(materials[10], shapes[3]),
		std::make_shared<Entity>(materials[10], shapes[3]),
		std::make_shared<Entity>(materials[10], shapes[3]),
		std::make_shared<Entity>(materials[10], shapes[3]),
	};

	transpEntities = {
		std::make_shared<Entity>(materials[8], shapes[3]),
		std::make_shared<Entity>(materials[8], shapes[3]),
		std::make_shared<Entity>(materials[8], shapes[3]),
	};
	#pragma endregion

	#pragma region Transform Setup
	transpEntities[0]->GetTransform()->SetPosition(0, 3, -5);
	transpEntities[1]->GetTransform()->SetPosition(0, 3, 0);
	transpEntities[2]->GetTransform()->SetPosition(0, 3, 5);
	for (int i = 0; i < entities.size() / 2; ++i)
	{
		entities[i]->GetTransform()->SetPosition((-(int)(entities.size() / 4) + i + 0.5f) * 2.5f, -1.5f, 0);
	}
	
	for (int i = entities.size() / 2; i < entities.size(); ++i)
	{
		entities[i]->GetTransform()->SetPosition((-(int)(entities.size() / 4) + (i - (int)entities.size() / 2) + 0.5f) * 2.5f, 1.5f, 0);
	}
	
	for (int i = 0; i < transpEntities.size(); ++i)
	{
		transpEntities[i]->GetTransform()->SetPosition(0, -3.5f, (-(int)(transpEntities.size() / 2) + i) * 2.5f);
	}
	#pragma endregion

	materials[0]->SwapTexture(TEXTYPE_REFLECTION, demoCubemap2);
	materials[0]->SetUVScale(DirectX::XMFLOAT2(1, 1));
	materials[2]->SetUVScale(DirectX::XMFLOAT2(1, 1));
	materials[11]->SetEmitAmount(DirectX::XMFLOAT3(1, 1, 1));
}

void Game::UpdateScene1(float deltaTime, float totalTime)
{
	for (int i = 1; i < 5; ++i)
	{
		DirectX::XMFLOAT3 pos = entities[i]->GetTransform()->GetPosition();
		entities[i]->GetTransform()->SetPosition(pos.x, sin(totalTime / 2) * 0.5f + 1, pos.z);
		entities[i]->GetTransform()->SetRotation(0, cos(totalTime / 4) * 4, 0);
	}

	materials[11]->SetUVOffset(DirectX::XMFLOAT2(0, -tan(totalTime / 4) * 0.15f));
	materials[11]->SetEmitAmount(DirectX::XMFLOAT3(sin(totalTime / 1) * 0.25f + 0.25f, sin(totalTime / 1) * 0.25f + 0.25f, sin(totalTime / 1) * 0.25f + 0.25f));
}

void Game::UpdateScene2(float deltaTime, float totalTime)
{
	for (int i = 0; i < entities.size(); ++i)
	{
		entities[i]->GetTransform()->SetRotation(0, sin(totalTime / 720) * 360, 0);
	}
}

// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	// Ensure camera has its projection matrix updated when window size changes
	camera->SetAspect((float)width / height);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	if (Input::GetInstance().KeyDown(VK_ESCAPE))
		Quit();

	if (Input::GetInstance().KeyDown(0x31))
		LoadScene(0);
	else if (Input::GetInstance().KeyDown(0x32))
		LoadScene(1);

	switch (currentScene)
	{
	case 0:
		UpdateScene1(deltaTime, totalTime);
		break;
	case 1:
		UpdateScene2(deltaTime, totalTime);
		break;
	}

	camera->Update(deltaTime);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color for clearing
	static const float color[4] = { 0.1f, 0.1f, 0.1f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen) before doign anything else
	context->ClearRenderTargetView(backBufferRTV.Get(), color);
	context->ClearDepthStencilView(
		depthStencilView.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	// Render solid entities first
	for (auto entity : entities)
	{
		entity->Draw(camera, ambient, lights);
	}

	// Draw the skybox after solid entities to avoid overdraw
	switch (currentScene)
	{
	case 0:
		skybox1->Draw(context, camera);
		break;
	case 1:
		skybox2->Draw(context, camera);
		break;
	}

	// Sort transparent entities
	std::sort(transpEntities.begin(), transpEntities.end(), [&](std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) -> bool
	{
		XMFLOAT3 positionA = a->GetTransform()->GetPosition();
		XMFLOAT3 positionB = b->GetTransform()->GetPosition();
		XMFLOAT3 camPos = camera->GetTransform()->GetPosition();

		// compare distance
		float aDist = XMVectorGetX(XMVector3Length(XMLoadFloat3(&positionA) - XMLoadFloat3(&camPos)));
		float bDist = XMVectorGetX(XMVector3Length(XMLoadFloat3(&positionB) - XMLoadFloat3(&camPos)));
		return aDist > bDist;
	});

	// Draw transparent entities with proper blendstate
	context->OMSetBlendState(alphaBlendState.Get(), 0, 0xFFFFFFFF);
	for (auto entity : transpEntities)
	{
		context->RSSetState(backfaceRasterState.Get());
		entity->Draw(camera, ambient, lights);
		context->RSSetState(0);
		entity->Draw(camera, ambient, lights);
	}

	// Reset blendstate after drawing transparent entities
	context->OMSetBlendState(0, 0, 0xFFFFFFFF);

	// Present the back buffer (i.e. the final frame) to the user at the end of drawing
	swapChain->Present(vsync ? 1 : 0, 0);

	// Due to the usage of a more sophisticated swap chain,
	// the render target must be re-bound after every call to Present()
	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthStencilView.Get());
}

// --------------------------------------------------------
// Loads six individual textures (the six faces of a cube map), then
// creates a blank cube map and copies each of the six textures to
// another face.  Afterwards, creates a shader resource view for
// the cube map and cleans up all of the temporary resources.
// --------------------------------------------------------
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Game::CreateCubemap(
	Microsoft::WRL::ComPtr<ID3D11Device> device,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
	const wchar_t* right,
	const wchar_t* left,
	const wchar_t* up,
	const wchar_t* down,
	const wchar_t* front,
	const wchar_t* back)
{

// --------------------------------------------------------
// Author: Chris Cascioli
// Purpose: Creates a cube map on the GPU from 6 individual textures
//
// - You are allowed to directly copy/paste this into your code base
//   for assignments, given that you clearly cite that this is not
//   code of your own design.
//
// - Note: This code assumes you’re putting the function in Game.cpp,
//   you’ve included WICTextureLoader.h and you have an ID3D11Device
//   ComPtr called “device”. Make any adjustments necessary for
//   your own implementation.
// --------------------------------------------------------

	// Load the 6 textures into an array.
	// - We need references to the TEXTURES, not the SHADER RESOURCE VIEWS!
	// - Specifically NOT generating mipmaps, as we usually don't need them for the sky!
	// - Order matters here!  +X, -X, +Y, -Y, +Z, -Z
	ID3D11Texture2D* textures[6] = {};
	CreateWICTextureFromFile(device.Get(), right, (ID3D11Resource**)&textures[0], 0);
	CreateWICTextureFromFile(device.Get(), left, (ID3D11Resource**)&textures[1], 0);
	CreateWICTextureFromFile(device.Get(), up, (ID3D11Resource**)&textures[2], 0);
	CreateWICTextureFromFile(device.Get(), down, (ID3D11Resource**)&textures[3], 0);
	CreateWICTextureFromFile(device.Get(), front, (ID3D11Resource**)&textures[4], 0);
	CreateWICTextureFromFile(device.Get(), back, (ID3D11Resource**)&textures[5], 0);

	// We'll assume all of the textures are the same color format and resolution,
	// so get the description of the first shader resource view
	D3D11_TEXTURE2D_DESC faceDesc = {};
	textures[0]->GetDesc(&faceDesc);

	// Describe the resource for the cube map, which is simply
	// a "texture 2d array".  This is a special GPU resource format,
	// NOT just a C++ array of textures!!!
	D3D11_TEXTURE2D_DESC cubeDesc = {};
	cubeDesc.ArraySize = 6; // Cube map!
	cubeDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // We'll be using as a texture in a shader
	cubeDesc.CPUAccessFlags = 0; // No read back
	cubeDesc.Format = faceDesc.Format; // Match the loaded texture's color format
	cubeDesc.Width = faceDesc.Width;  // Match the size
	cubeDesc.Height = faceDesc.Height; // Match the size
	cubeDesc.MipLevels = 1; // Only need 1
	cubeDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE; // A CUBE MAP, not 6 separate textures
	cubeDesc.Usage = D3D11_USAGE_DEFAULT; // Standard usage
	cubeDesc.SampleDesc.Count = 1;
	cubeDesc.SampleDesc.Quality = 0;

	// Create the actual texture resource
	ID3D11Texture2D* cubeMapTexture = 0;
	device->CreateTexture2D(&cubeDesc, 0, &cubeMapTexture);

	// Loop through the individual face textures and copy them,
	// one at a time, to the cube map texure
	for (int i = 0; i < 6; i++)
	{
		// Calculate the subresource position to copy into
		unsigned int subresource = D3D11CalcSubresource(
			0,	// Which mip (zero, since there's only one)
			i,	// Which array element?
			1); 	// How many mip levels are in the texture?

		// Copy from one resource (texture) to another
		context->CopySubresourceRegion(
			cubeMapTexture, // Destination resource
			subresource,	// Dest subresource index (one of the array elements)
			0, 0, 0,		// XYZ location of copy
			textures[i],	// Source resource
			0,	// Source subresource index (we're assuming there's only one)
			0);	// Source subresource "box" of data to copy (zero means the whole thing)
	}

	// At this point, all of the faces have been copied into the
	// cube map texture, so we can describe a shader resource view for it
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = cubeDesc.Format; 	// Same format as texture
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE; // Treat this as a cube!
	srvDesc.TextureCube.MipLevels = 1;	// Only need access to 1 mip
	srvDesc.TextureCube.MostDetailedMip = 0; // Index of the first mip we want to see

	// Make the SRV
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeSRV;
	device->CreateShaderResourceView(cubeMapTexture, &srvDesc, cubeSRV.GetAddressOf());

	// Now that we're done, clean up the stuff we don't need anymore
	cubeMapTexture->Release();  // Done with this particular reference (the SRV has another)
	for (int i = 0; i < 6; i++)
		textures[i]->Release();

	// Send back the SRV, which is what we need for our shaders
	return cubeSRV;
}
