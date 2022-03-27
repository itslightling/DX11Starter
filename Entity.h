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

	void							Draw(std::shared_ptr<Camera> _camera, DirectX::XMFLOAT3 _ambient, std::vector<Light> _lights);

	Transform*						GetTransform();
	std::shared_ptr<Mesh>			GetMesh();
	std::shared_ptr<Material>		GetMaterial();

	void							SetMaterial(std::shared_ptr<Material>	_material);

private:
	Transform						transform;
	std::shared_ptr<Mesh>			mesh;
	std::shared_ptr<Material>		material;
};
