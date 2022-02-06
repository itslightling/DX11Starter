#include "Entity.h"

Entity::Entity(std::shared_ptr<Mesh> _mesh)
{
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
