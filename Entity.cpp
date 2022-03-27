#include "Entity.h"

Entity::Entity(std::shared_ptr<Material> _material, std::shared_ptr<Mesh> _mesh)
{
	material = _material;
	mesh = _mesh;
}

void Entity::Draw(std::shared_ptr<Camera> _camera, DirectX::XMFLOAT3 _ambient, std::vector<Light> _lights)
{
	material->Activate(&transform, _camera, _ambient, _lights);
	mesh->Draw();
}

Transform* Entity::GetTransform()
{
	return &transform;
}

std::shared_ptr<Mesh> Entity::GetMesh()
{
	return mesh;
}

std::shared_ptr<Material> Entity::GetMaterial()
{
	return material;
}

void Entity::SetMaterial(std::shared_ptr<Material> _material)
{
	material = _material;
}
