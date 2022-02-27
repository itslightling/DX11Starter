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
	camera = std::make_shared<Camera>(0.0f, 0.0f, -4.0f, (float)width / height, 60, 0.01f, 1000.0f);
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
	pixelShader = std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"PixelShader.cso").c_str());

	// thanks to https://harry7557558.github.io/tools/colorpicker.html for having the only 0f-1f picker i could find
	XMFLOAT4 white = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4 deeppink = XMFLOAT4(1.0f, 0.08f, 0.4f, 1.0f);
	XMFLOAT4 deepcoral = XMFLOAT4(1.0f, 0.39f, 0.22f, 1.0f);

	materials = {
		std::make_shared<Material>(white, vertexShader, pixelShader),
		std::make_shared<Material>(deeppink, vertexShader, pixelShader),
		std::make_shared<Material>(deepcoral, vertexShader, pixelShader),
	};
}

// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	XMFLOAT4 black = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 white = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	Vertex verts1[] = {
		{ XMFLOAT3(+0.00f, +0.00f, +0.25f), white },
		{ XMFLOAT3(-0.25f, -0.25f, -0.25f), red },
		{ XMFLOAT3(+0.00f, +0.25f, -0.25f), green },
		{ XMFLOAT3(+0.25f, -0.25f, -0.25f), blue },
	};
	unsigned int ind1[] = { 0,1,2 , 0,2,3 , 0,3,1 , 3,2,1 };

	Vertex verts2[] = {
		{ XMFLOAT3(-0.75f, +0.50f, +0.00f), red },
		{ XMFLOAT3(-0.50f, +0.50f, +0.00f), blue },
		{ XMFLOAT3(-0.50f, +0.20f, +0.00f), red },
		{ XMFLOAT3(-0.75f, +0.20f, +0.00f), blue },
	};
	unsigned int ind2[] = { 0,1,2, 0,2,3 , 3,2,0 , 2,1,0 };

	Vertex verts3[] = {
		{ XMFLOAT3(+0.00f, +0.30f, +0.15f), white },
		{ XMFLOAT3(+0.30f, +0.15f, +0.00f), black },
		{ XMFLOAT3(+0.30f, -0.15f, +0.00f), white },
		{ XMFLOAT3(+0.00f, -0.30f, +0.15f), black },
		{ XMFLOAT3(-0.30f, -0.15f, +0.00f), white },
		{ XMFLOAT3(-0.30f, +0.15f, +0.00f), black },
	};
	unsigned int ind3[] = { 0,1,5 , 1,2,5 , 2,3,4 , 2,4,5 , 5,4,2 , 4,3,2 , 5,2,1 , 5,1,0 };

	shapes = {
		std::make_shared<Mesh>(verts1, 4, ind1, 12, device, context),
		std::make_shared<Mesh>(verts2, 4, ind2, 12, device, context),
		std::make_shared<Mesh>(verts3, 6, ind3, 24, device, context),
	};

	entities = {
		std::make_shared<Entity>(materials[0], shapes[0]),
		std::make_shared<Entity>(materials[1], shapes[0]),
		std::make_shared<Entity>(materials[2], shapes[0]),
		std::make_shared<Entity>(materials[0], shapes[1]),
		std::make_shared<Entity>(materials[1], shapes[1]),
		std::make_shared<Entity>(materials[2], shapes[1]),
		std::make_shared<Entity>(materials[0], shapes[2]),
		std::make_shared<Entity>(materials[1], shapes[2]),
		std::make_shared<Entity>(materials[2], shapes[2]),
	};
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
		entities[i]->GetTransform()->SetScale(0.2f * (i + 1), 0.2f * (i + 1), 0.2f * (i + 1));
		entities[i]->GetTransform()->SetRotation(0.1f * (i + 1) * sin(totalTime), 0.1f * (i + 1) * sin(totalTime), 0.1f * (i + 1) * sin(totalTime));
		// this range uses shapes[0] for testing
		if (i < 3)
		{
			entities[i]->GetTransform()->SetPosition(tan((double)totalTime * ((double)i + (double)1)) * 0.1f, sin(totalTime) * 0.1f, (double)i * 0.1f);
		}
		// this range uses shapes[1] for testing
		else if (i < 6)
		{
			entities[i]->GetTransform()->SetPosition(sin((double)totalTime * ((double)i + (double)1)) * 0.1f, cos(totalTime) * 0.1f, (double)i * 0.1f);
		}
		// this range uses shapes[2] for testing
		else
		{
			entities[i]->GetTransform()->SetPosition(sin((double)totalTime * ((double)i + (double)1)) * cos(totalTime) * 0.1f, 0, (double)i * 0.1f);
		}
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
		// create constant buffer
		std::shared_ptr<SimpleVertexShader> vs = entity->GetMaterial()->GetVertexShader();
		vs->SetFloat4("colorTint", entity->GetMaterial()->GetTint());
		vs->SetMatrix4x4("world", entity->GetTransform()->GetWorldMatrix());
		vs->SetMatrix4x4("view", camera->GetViewMatrix());
		vs->SetMatrix4x4("projection", camera->GetProjectionMatrix());
		vs->CopyAllBufferData();

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