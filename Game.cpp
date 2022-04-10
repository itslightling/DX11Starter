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
	vertexShaderPBR = std::make_shared<SimpleVertexShader>(device, context, GetFullPathTo_Wide(L"SimpleVertexPBR.cso").c_str());
	pixelShaderPBR = std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"SimplePixelPBR.cso").c_str());

	XMFLOAT3 white = XMFLOAT3(1.0f, 1.0f, 1.0f);

	materials = {
		std::make_shared<Material>(false, white, 0, vertexShader, pixelShader),
		std::make_shared<Material>(true, white, 0, vertexShaderPBR, pixelShaderPBR),
		std::make_shared<Material>(true, white, 0, vertexShaderPBR, pixelShaderPBR),
		std::make_shared<Material>(true, white, 0, vertexShaderPBR, pixelShaderPBR),
		std::make_shared<Material>(true, white, 0, vertexShaderPBR, pixelShaderPBR),
		std::make_shared<Material>(true, white, 0, vertexShaderPBR, pixelShaderPBR),
		std::make_shared<Material>(true, white, 0, vertexShaderPBR, pixelShaderPBR),
		std::make_shared<Material>(true, white, 0, vertexShaderPBR, pixelShaderPBR),
	};
}

// --------------------------------------------------------
// Loads textures and pushes them to the loaded materials
// --------------------------------------------------------
void Game::LoadTextures()
{
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.MaxAnisotropy = 16;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&sampDesc, sampler.GetAddressOf());

	demoCubemap = CreateCubemap(
		device,
		context,
		L"Assets/Textures/Skies/planets/right.png",
		L"Assets/Textures/Skies/planets/left.png",
		L"Assets/Textures/Skies/planets/up.png",
		L"Assets/Textures/Skies/planets/down.png",
		L"Assets/Textures/Skies/planets/front.png",
		L"Assets/Textures/Skies/planets/back.png"
	);

	materials[0]->PushSampler("BasicSampler", sampler);
	materials[0]->PushTexture(TEXTYPE_REFLECTION, demoCubemap);
	materials[0]->hasReflectionMap = true;
	materials[0]->LoadTexture(L"Assets/Textures/HQGame/structure-endgame-deepfloor_albedo.png", TEXTYPE_ALBEDO, device.Get(), context.Get());
	materials[0]->LoadTexture(L"Assets/Textures/HQGame/structure-endgame-deepfloor_specular.png", TEXTYPE_SPECULAR, device.Get(), context.Get());
	materials[0]->LoadTexture(L"Assets/Textures/HQGame/structure-endgame-deepfloor_emissive.png", TEXTYPE_EMISSIVE, device.Get(), context.Get());

	materials[1]->PushSampler("BasicSampler", sampler);
	materials[1]->LoadTexture(L"Assets/Textures/PBR/bronze_albedo.png", TEXTYPE_ALBEDO, device.Get(), context.Get());
	materials[1]->LoadTexture(L"Assets/Textures/PBR/bronze_metal.png", TEXTYPE_METALNESS, device.Get(), context.Get());
	materials[1]->LoadTexture(L"Assets/Textures/PBR/bronze_roughness.png", TEXTYPE_ROUGHNESS, device.Get(), context.Get());
	materials[1]->LoadTexture(L"Assets/Textures/PBR/bronze_normals.png", TEXTYPE_NORMAL, device.Get(), context.Get());

	materials[2]->PushSampler("BasicSampler", sampler);
	materials[2]->LoadTexture(L"Assets/Textures/PBR/cobblestone_albedo.png", TEXTYPE_ALBEDO, device.Get(), context.Get());
	materials[2]->LoadTexture(L"Assets/Textures/PBR/cobblestone_metal.png", TEXTYPE_METALNESS, device.Get(), context.Get());
	materials[2]->LoadTexture(L"Assets/Textures/PBR/cobblestone_roughness.png", TEXTYPE_ROUGHNESS, device.Get(), context.Get());
	materials[2]->LoadTexture(L"Assets/Textures/PBR/cobblestone_normals.png", TEXTYPE_NORMAL, device.Get(), context.Get());

	materials[3]->PushSampler("BasicSampler", sampler);
	materials[3]->LoadTexture(L"Assets/Textures/PBR/floor_albedo.png", TEXTYPE_ALBEDO, device.Get(), context.Get());
	materials[3]->LoadTexture(L"Assets/Textures/PBR/floor_metal.png", TEXTYPE_METALNESS, device.Get(), context.Get());
	materials[3]->LoadTexture(L"Assets/Textures/PBR/floor_roughness.png", TEXTYPE_ROUGHNESS, device.Get(), context.Get());
	materials[3]->LoadTexture(L"Assets/Textures/PBR/floor_normals.png", TEXTYPE_NORMAL, device.Get(), context.Get());

	materials[4]->PushSampler("BasicSampler", sampler);
	materials[4]->LoadTexture(L"Assets/Textures/PBR/paint_albedo.png", TEXTYPE_ALBEDO, device.Get(), context.Get());
	materials[4]->LoadTexture(L"Assets/Textures/PBR/paint_metal.png", TEXTYPE_METALNESS, device.Get(), context.Get());
	materials[4]->LoadTexture(L"Assets/Textures/PBR/paint_roughness.png", TEXTYPE_ROUGHNESS, device.Get(), context.Get());
	materials[4]->LoadTexture(L"Assets/Textures/PBR/paint_normals.png", TEXTYPE_NORMAL, device.Get(), context.Get());

	materials[5]->PushSampler("BasicSampler", sampler);
	materials[5]->LoadTexture(L"Assets/Textures/PBR/rough_albedo.png", TEXTYPE_ALBEDO, device.Get(), context.Get());
	materials[5]->LoadTexture(L"Assets/Textures/PBR/rough_metal.png", TEXTYPE_METALNESS, device.Get(), context.Get());
	materials[5]->LoadTexture(L"Assets/Textures/PBR/rough_roughness.png", TEXTYPE_ROUGHNESS, device.Get(), context.Get());
	materials[5]->LoadTexture(L"Assets/Textures/PBR/rough_normals.png", TEXTYPE_NORMAL, device.Get(), context.Get());

	materials[6]->PushSampler("BasicSampler", sampler);
	materials[6]->LoadTexture(L"Assets/Textures/PBR/scratched_albedo.png", TEXTYPE_ALBEDO, device.Get(), context.Get());
	materials[6]->LoadTexture(L"Assets/Textures/PBR/scratched_metal.png", TEXTYPE_METALNESS, device.Get(), context.Get());
	materials[6]->LoadTexture(L"Assets/Textures/PBR/scratched_roughness.png", TEXTYPE_ROUGHNESS, device.Get(), context.Get());
	materials[6]->LoadTexture(L"Assets/Textures/PBR/scratched_normals.png", TEXTYPE_NORMAL, device.Get(), context.Get());

	materials[7]->PushSampler("BasicSampler", sampler);
	materials[7]->LoadTexture(L"Assets/Textures/PBR/wood_albedo.png", TEXTYPE_ALBEDO, device.Get(), context.Get());
	materials[7]->LoadTexture(L"Assets/Textures/PBR/wood_metal.png", TEXTYPE_METALNESS, device.Get(), context.Get());
	materials[7]->LoadTexture(L"Assets/Textures/PBR/wood_roughness.png", TEXTYPE_ROUGHNESS, device.Get(), context.Get());
	materials[7]->LoadTexture(L"Assets/Textures/PBR/wood_normals.png", TEXTYPE_NORMAL, device.Get(), context.Get());
}

// --------------------------------------------------------
// Instantiates all the lighting in the scene
// --------------------------------------------------------
void Game::LoadLighting()
{
	ambient = XMFLOAT3(0.1f, 0.1f, 0.15f);

	lights = {
		Light::Directional(XMFLOAT3(1, 0.5f, 0.5f), XMFLOAT3(1, 1, 1), 2.00f),
		Light::Directional(XMFLOAT3(-0.25f, -1, 0.75f), XMFLOAT3(1, 1, 1), 0.25f),
		Light::Directional(XMFLOAT3(-1, 1, -0.5f), XMFLOAT3(1, 1, 1), 0.25f),
		Light::Point(XMFLOAT3(-1.5f, 0, 0), XMFLOAT3(1, 1, 1), 0.25f, 10),
		Light::Point(XMFLOAT3(1.5f, 0, 0), XMFLOAT3(1, 1, 1), 0.25f, 10),
		Light::Point(XMFLOAT3(0, 2, 0), XMFLOAT3(1, 0, 0), 0.25f, 10),
		Light::Point(XMFLOAT3(-27.5f, 0, 0), XMFLOAT3(1, 1, 0.5f), 0.25f, 20),
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
		std::make_shared<Entity>(materials[1], shapes[3]),
		std::make_shared<Entity>(materials[2], shapes[3]),
		std::make_shared<Entity>(materials[3], shapes[3]),
		std::make_shared<Entity>(materials[4], shapes[3]),
		std::make_shared<Entity>(materials[5], shapes[3]),
		std::make_shared<Entity>(materials[6], shapes[3]),
		std::make_shared<Entity>(materials[7], shapes[3]),
		std::make_shared<Entity>(materials[0], shapes[3]),
	};

	for (int i = 0; i < entities.size(); ++i)
	{
		entities[i]->GetTransform()->SetPosition((-(int)(entities.size() / 2) + i) * 5, 0, 0);
	}

	skybox = std::make_shared<Sky>(
		shapes[0],
		std::make_shared<SimpleVertexShader>(device, context, GetFullPathTo_Wide(L"SkyboxVertexShader.cso").c_str()),
		std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"SkyboxPixelShader.cso").c_str()),
		demoCubemap,
		sampler,
		device
	);
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

	skybox->Draw(context, camera);

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
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
