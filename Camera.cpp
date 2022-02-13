#include "Camera.h"

using namespace DirectX;

Camera::Camera(float _x, float _y, float _z, float _aspect, float _fov, float _near, float _far)
{
	transform.SetPosition(_x, _y, _z);

	aspect = _aspect;
	fovYRadians = XMConvertToRadians(_fov);
	clipNear = _near;
	clipFar = _far;

	UpdateViewMatrix();
	UpdateProjectionMatrix();
}

Camera::~Camera()
{
}

void Camera::Update(float _dt)
{
	transform.TranslateRelative(0, 0, _dt);
	UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
	XMFLOAT3 worldUp = XMFLOAT3(0, 1, 0);
	XMMATRIX view = XMMatrixLookToLH(XMLoadFloat3(&transform.GetPosition()), XMLoadFloat3(&transform.GetForward()), XMLoadFloat3(&worldUp));
	XMStoreFloat4x4(&viewMatrix, view);
}

void Camera::UpdateProjectionMatrix()
{
	XMMATRIX projection = XMMatrixPerspectiveFovLH(fovYRadians, aspect, clipNear, clipFar);
	XMStoreFloat4x4(&projectionMatrix, projection);
}

Transform* Camera::GetTransform()
{
	return &transform;
}

DirectX::XMFLOAT4X4 Camera::GetViewMatrix()
{
	return viewMatrix;
}

DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return projectionMatrix;
}

void Camera::SetAspect(float _aspect)
{
	aspect = _aspect;
	UpdateProjectionMatrix();
}

void Camera::SetFOV(float _fov)
{
	fovYRadians = XMConvertToRadians(_fov);
	UpdateProjectionMatrix();
}

void Camera::SetNearClip(float _near)
{
	clipNear = _near;
	UpdateProjectionMatrix();
}

void Camera::SetFarClip(float _far)
{
	clipFar = _far;
	UpdateProjectionMatrix();
}
