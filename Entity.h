#pragma once

#include "Mesh.h"
#include "Transform.h"
#include <memory>

class Entity
{
public:
	Entity(std::shared_ptr<Mesh> _mesh);

	Transform* GetTransform();
	std::shared_ptr<Mesh> GetMesh();

private:
	Transform transform;
	std::shared_ptr<Mesh> mesh;
};
