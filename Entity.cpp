#include "Entity.h"

Entity::Entity(std::shared_ptr<Material> _material, std::shared_ptr<Mesh> _mesh)
{
	material = _material;
	mesh = _mesh;
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
