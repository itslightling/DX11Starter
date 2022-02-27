#pragma once

#include "Mesh.h"
#include "Transform.h"
#include "Material.h"
#include <memory>

class Entity
{
public:
	Entity(
		std::shared_ptr<Material>	_material,
		std::shared_ptr<Mesh>		_mesh);

	Transform*						GetTransform();
	std::shared_ptr<Mesh>			GetMesh();
	std::shared_ptr<Material>		GetMaterial();

	void							SetMaterial(std::shared_ptr<Material>	_material);

private:
	Transform						transform;
	std::shared_ptr<Mesh>			mesh;
	std::shared_ptr<Material>		material;
};
