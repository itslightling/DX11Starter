#pragma once

#include "Transform.h"
#include <memory>

class Camera
{
public:
	Camera(float _x, float _y, float _z, float _aspect, float _fov, float _near, float _far);
	~Camera();

	void				Update(float _dt);
	void				UpdateViewMatrix();
	void				UpdateProjectionMatrix();

	Transform*			GetTransform();
	DirectX::XMFLOAT4X4 GetViewMatrix();
	DirectX::XMFLOAT4X4 GetProjectionMatrix();

	void				SetAspect(float _aspect);
	void				SetFOV(float _fov);
	void				SetNearClip(float _near);
	void				SetFarClip(float _far);

private:
	float				aspect;
	float				fovYRadians;
	float				clipNear;
	float				clipFar;

	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;

	Transform			transform;
};
