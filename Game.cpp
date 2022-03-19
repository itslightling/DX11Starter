#include "Game.h"
#include "Vertex.h"
#include "Input.h"
#include "SimpleShader.h"

// Needed for a helper function to read compiled shader files from the hard drive
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>
#include <iostream>

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
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	CreateBasicGeometry();
	
	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = std::make_shared<SimpleVertexShader>(device, context, GetFullPathTo_Wide(L"VertexShader.cso").c_str());
	pixelShader = //std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"PixelShader.cso").c_str());
		//std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"RandomPixelShader.cso").c_str());
		std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"SimplePixelShader.cso").c_str());

	// thanks to https://harry7557558.github.io/tools/colorpicker.html for having the only 0f-1f picker i could find
	XMFLOAT4 white = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4 deeppink = XMFLOAT4(1.0f, 0.08f, 0.4f, 1.0f);
	XMFLOAT4 deepcoral = XMFLOAT4(1.0f, 0.39f, 0.22f, 1.0f);

	materials = {
		std::make_shared<Material>(white, 0, vertexShader, pixelShader),
		std::make_shared<Material>(deeppink, 0, vertexShader, pixelShader),
		std::make_shared<Material>(deepcoral, 0, vertexShader, pixelShader),
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
			GetFullPathTo("Assets/Models/quad.obj").c_str(),
			device, context),
		std::make_shared<Mesh>(
			GetFullPathTo("Assets/Models/quad_double_sided.obj").c_str(),
			device, context),
		std::make_shared<Mesh>(
			GetFullPathTo("Assets/Models/sphere.obj").c_str(),
			device, context),
		std::make_shared<Mesh>(
			GetFullPathTo("Assets/Models/torus.obj").c_str(),
			device, context),
	};

	entities = {
		std::make_shared<Entity>(materials[0], shapes[0]),
		std::make_shared<Entity>(materials[1], shapes[1]),
		std::make_shared<Entity>(materials[2], shapes[2]),
		std::make_shared<Entity>(materials[0], shapes[3]),
		std::make_shared<Entity>(materials[1], shapes[4]),
		std::make_shared<Entity>(materials[2], shapes[5]),
		std::make_shared<Entity>(materials[0], shapes[6]),
	};

	for (int i = 0; i < entities.size(); ++i)
	{
		entities[i]->GetTransform()->SetPosition((-(int)(entities.size() / 2) + i) * 5, 0, 0);
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
	// Example input checking: Quit if the escape key is pressed
	if (Input::GetInstance().KeyDown(VK_ESCAPE))
		Quit();

	camera->Update(deltaTime);

	for (int i = 0; i < entities.size(); ++i)
	{
		entities[i]->GetTransform()->SetRotation(1.0f * (i + 1) * sin(totalTime), 1.0f * (i + 1) * sin(totalTime), 1.0f * (i + 1) * sin(totalTime));
		entities[i]->GetMaterial()->SetRoughness(sin(totalTime) * 0.5f + 0.5f);
	}
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	static const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

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
		std::shared_ptr<SimpleVertexShader> vs = entity->GetMaterial()->GetVertexShader();
		vs->SetMatrix4x4("world", entity->GetTransform()->GetWorldMatrix());
		vs->SetMatrix4x4("view", camera->GetViewMatrix());
		vs->SetMatrix4x4("projection", camera->GetProjectionMatrix());
		vs->CopyAllBufferData();

		std::shared_ptr<SimplePixelShader> ps = entity->GetMaterial()->GetPixelShader();
		ps->SetFloat3("cameraPosition", camera->GetTransform()->GetPosition());
		ps->SetFloat("roughness", entity->GetMaterial()->GetRoughness());
		ps->CopyAllBufferData();

		entity->GetMaterial()->GetVertexShader()->SetShader();
		entity->GetMaterial()->GetPixelShader()->SetShader();
		entity->GetMesh()->Draw();
	}

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(vsync ? 1 : 0, 0);

	// Due to the usage of a more sophisticated swap chain,
	// the render target must be re-bound after every call to Present()
	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthStencilView.Get());
}