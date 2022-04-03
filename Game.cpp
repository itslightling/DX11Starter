#include "Game.h"
#include "Vertex.h"
#include "Input.h"
#include "SimpleShader.h"

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
	camera = std::make_shared<Camera>(0.0f, 0.0f, -20.0f, (float)width / height, 60, 0.01f, 1000.0f);
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
	LoadLighting();
	CreateBasicGeometry();
	
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

	XMFLOAT3 white = XMFLOAT3(1.0f, 1.0f, 1.0f);

	materials = {
		std::make_shared<Material>(white, 0, vertexShader, pixelShader),
		std::make_shared<Material>(white, 0, vertexShader, pixelShader),
		std::make_shared<Material>(white, 0, vertexShader, pixelShader),
		std::make_shared<Material>(white, 0, vertexShader, pixelShader),
	};
}

// --------------------------------------------------------
// Loads textures and pushes them to the loaded materials
// --------------------------------------------------------
void Game::LoadTextures()
{
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;

	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.MaxAnisotropy = 16;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&sampDesc, sampler.GetAddressOf());

	materials[0]->PushSampler("BasicSampler", sampler);
	materials[0]->LoadTexture(L"Assets/Textures/HQGame/structure-endgame-deepfloor_albedo.png", TEXTYPE_ALBEDO, device.Get(), context.Get());
	materials[0]->LoadTexture(L"Assets/Textures/HQGame/structure-endgame-deepfloor_specular.png", TEXTYPE_SPECULAR, device.Get(), context.Get());
	materials[0]->LoadTexture(L"Assets/Textures/HQGame/structure-endgame-deepfloor_emissive.png", TEXTYPE_EMISSIVE, device.Get(), context.Get());

	materials[1]->PushSampler("BasicSampler", sampler);
	materials[1]->LoadTexture(L"Assets/Textures/WithNormals/cobblestone.png", TEXTYPE_ALBEDO, device.Get(), context.Get());
	materials[1]->LoadTexture(L"Assets/Textures/WithNormals/cobblestone_specular.png", TEXTYPE_SPECULAR, device.Get(), context.Get());
	materials[1]->LoadTexture(L"Assets/Textures/WithNormals/cobblestone_normals.png", TEXTYPE_NORMAL, device.Get(), context.Get());

	materials[2]->PushSampler("BasicSampler", sampler);
	materials[2]->LoadTexture(L"Assets/Textures/WithNormals/rock.png", TEXTYPE_ALBEDO, device.Get(), context.Get());
	materials[2]->LoadTexture(L"Assets/Textures/WithNormals/rock_specular.png", TEXTYPE_SPECULAR, device.Get(), context.Get());
	materials[2]->LoadTexture(L"Assets/Textures/WithNormals/rock_normals.png", TEXTYPE_NORMAL, device.Get(), context.Get());

	materials[3]->PushSampler("BasicSampler", sampler);
	materials[3]->LoadTexture(L"Assets/Textures/WithNormals/cushion.png", TEXTYPE_ALBEDO, device.Get(), context.Get());
	materials[3]->LoadTexture(L"Assets/Textures/WithNormals/cushion_specular.png", TEXTYPE_SPECULAR, device.Get(), context.Get());
	materials[3]->LoadTexture(L"Assets/Textures/WithNormals/cushion_normals.png", TEXTYPE_NORMAL, device.Get(), context.Get());
}

// --------------------------------------------------------
// Instantiates all the lighting in the scene
// --------------------------------------------------------
void Game::LoadLighting()
{
	ambient = XMFLOAT3(0.1f, 0.1f, 0.15f);

	lights = {
		Light::Directional(XMFLOAT3(1, 0.5f, 0.5f), XMFLOAT3(1, 1, 1), 0.75f),
		Light::Directional(XMFLOAT3(-0.25f, -1, 0.75f), XMFLOAT3(1, 1, 1), 0.75f),
		Light::Directional(XMFLOAT3(-1, 1, -0.5f), XMFLOAT3(1, 1, 1), 0.75f),
		Light::Point(XMFLOAT3(-1.5f, 0, 0), XMFLOAT3(1, 1, 1), 0.5f, 10),
		Light::Point(XMFLOAT3(1.5f, 0, 0), XMFLOAT3(1, 1, 1), 0.25f, 10),
		Light::Point(XMFLOAT3(0, 2, 0), XMFLOAT3(1, 0, 0), 0.25f, 10),
		Light::Point(XMFLOAT3(-27.5f, 0, 0), XMFLOAT3(1, 1, 0.5f), 5, 20),
	};
}

// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
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
	};

	entities = {
		std::make_shared<Entity>(materials[1], shapes[0]),
		std::make_shared<Entity>(materials[2], shapes[1]),
		std::make_shared<Entity>(materials[3], shapes[2]),
		std::make_shared<Entity>(materials[2], shapes[3]),
		std::make_shared<Entity>(materials[1], shapes[4]),
		std::make_shared<Entity>(materials[0], shapes[5]),
		std::make_shared<Entity>(materials[0], shapes[6]),
	};

	entities[6]->GetMaterial()->SetEmitAmount(0.75f);

	for (int i = 0; i < entities.size(); ++i)
	{
		entities[i]->GetTransform()->SetPosition((-(int)(entities.size() / 2) + i) * 5, 0, 0);
		entities[i]->GetMaterial()->SetRoughness(0.60f);
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

	camera->Update(deltaTime);

	for (int i = 0; i < entities.size(); ++i)
	{
		entities[i]->GetTransform()->SetRotation(sin(totalTime / 720) * 360, 0, 0);
	}
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color for clearing
	static const float color[4] = { 0.1f, 0.1f, 0.1f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV.Get(), color);
	context->ClearDepthStencilView(
		depthStencilView.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	for (auto entity : entities)
	{
		entity->Draw(camera, ambient, lights);
	}

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(vsync ? 1 : 0, 0);

	// Due to the usage of a more sophisticated swap chain,
	// the render target must be re-bound after every call to Present()
	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthStencilView.Get());
}