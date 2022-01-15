#include "Mesh.h"

Mesh::Mesh(Vertex* _vertices, int _vertexCount, unsigned int* _indices, int _indexCount, Microsoft::WRL::ComPtr<ID3D11Device> _device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> _context)
{
}

Mesh::~Mesh()
{
}

void Mesh::Draw()
{
}

Microsoft::WRL::ComPtr<ID3D11Buffer>* Mesh::GetVertexBuffer()
{
	return nullptr;
}

Microsoft::WRL::ComPtr<ID3D11Buffer>* Mesh::GetIndexBuffer()
{
	return nullptr;
}

int Mesh::GetIndexCount()
{
	return 0;
}
