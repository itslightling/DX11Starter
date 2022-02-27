#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include "Vertex.h"

class Mesh
{
public:
	Mesh(
		Vertex*                                     _vertices,
		int                                         _vertexCount,
		unsigned int*                               _indices,
		int                                         _indexCount,
		Microsoft::WRL::ComPtr<ID3D11Device>        _device,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> _context);
	Mesh(
		const char*									_file,
		Microsoft::WRL::ComPtr<ID3D11Device>        _device,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> _context);
	~Mesh();

	void                                            Draw();
	Microsoft::WRL::ComPtr<ID3D11Buffer>*           GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer>*           GetIndexBuffer();
	int                                             GetIndexCount();

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer>            bufferVertex;
	Microsoft::WRL::ComPtr<ID3D11Buffer>            bufferIndex;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>     deviceContext;
	int                                             countIndex;

	void											CreateMesh(
														Vertex*										_vertices,
														int                                         _vertexCount,
														unsigned int*								_indices,
														int                                         _indexCount,
														Microsoft::WRL::ComPtr<ID3D11Device>        _device,
														Microsoft::WRL::ComPtr<ID3D11DeviceContext> _context);
};
