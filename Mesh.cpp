#include "Mesh.h"

using namespace DirectX;

Mesh::Mesh(Vertex* _vertices, int _vertexCount, unsigned int* _indices, int _indexCount, Microsoft::WRL::ComPtr<ID3D11Device> _device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> _context)
{
	// Create the VERTEX BUFFER description
	D3D11_BUFFER_DESC vbd = {};
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * _vertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	// Create the proper struct to hold the initial vertex data
	// - This is how we put the initial data into the buffer
	D3D11_SUBRESOURCE_DATA initialVertexData = {};
	initialVertexData.pSysMem = _vertices;

	// Actually create the buffer with the initial data
	_device->CreateBuffer(&vbd, &initialVertexData, bufferVertex.GetAddressOf());

	// Create the INDEX BUFFER description
	D3D11_BUFFER_DESC ibd = {};
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(unsigned int) * _indexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	// Create the proper struct to hold the initial index data
	D3D11_SUBRESOURCE_DATA initialIndexData = {};
	initialIndexData.pSysMem = _indices;
	countIndex = _indexCount;

	// Create the buffer with the initial data
	_device->CreateBuffer(&ibd, &initialIndexData, bufferIndex.GetAddressOf());

	deviceContext = _context;
}

Mesh::~Mesh()
{
	// Because this is using ComPtr, destructor for now is unnecessary
}

void Mesh::Draw()
{
	// Set buffers in the input assembler
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, bufferVertex.GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(bufferIndex.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Do the actual drawing
	deviceContext->DrawIndexed(
		countIndex, // The number of indices to use (we could draw a subset if we wanted)
		0,          // Offset to the first index we want to use
		0);         // Offset to add to each index when looking up vertices
}

Microsoft::WRL::ComPtr<ID3D11Buffer>* Mesh::GetVertexBuffer()
{
	return &bufferVertex;
}

Microsoft::WRL::ComPtr<ID3D11Buffer>* Mesh::GetIndexBuffer()
{
	return &bufferIndex;
}

int Mesh::GetIndexCount()
{
	return countIndex;
}
