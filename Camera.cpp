#include "Camera.h"
#include "Input.h"

using namespace DirectX;

Camera::Camera(float _x, float _y, float _z, float _aspect, float _fov, float _near, float _far, float _moveSpeed)
{
	transform.SetPosition(_x, _y, _z);

	aspect = _aspect;
	fovYRadians = XMConvertToRadians(_fov);
	clipNear = _near;
	clipFar = _far;
	moveSpeed = _moveSpeed;

	UpdateViewMatrix();
	UpdateProjectionMatrix();
}

Camera::~Camera()
{
}

void Camera::Update(float _dt)
{
	ReadInput(_dt);
	ClampRotation();
	UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
	XMFLOAT3 worldUp = XMFLOAT3(0, 1, 0);
	XMFLOAT3 position = transform.GetPosition();
	XMFLOAT3 forward = transform.GetForward();

	XMMATRIX view = XMMatrixLookToLH(XMLoadFloat3(&position), XMLoadFloat3(&forward), XMLoadFloat3(&worldUp));
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

void Camera::ReadInput(float _dt)
{
	Input& input = Input::GetInstance();

	float modify = 1.0f;
	float moveLong = 0;
	float moveLat = 0;
	float moveVert = 0;

	if (input.KeyDown('W')) moveLong += 1.0f;
	if (input.KeyDown('S')) moveLong -= 1.0f;
	if (input.KeyDown('D')) moveLat  += 1.0f;
	if (input.KeyDown('A')) moveLat  -= 1.0f;
	if (input.KeyDown('E')) moveVert += 1.0f;
	if (input.KeyDown('Q')) moveVert -= 1.0f;
	if (input.KeyDown(VK_SHIFT))   modify *= 2.0f;
	if (input.KeyDown(VK_CONTROL)) modify /= 2.0f;

	transform.TranslateRelative(moveLat * _dt * modify * moveSpeed, 0, moveLong * _dt * modify * moveSpeed);
	transform.TranslateAbsolute(0, moveVert * _dt * modify * moveSpeed, 0);

	if (input.MouseLeftDown())
	{
		float cursorX = (float)input.GetMouseXDelta();
		float cursorY = (float)input.GetMouseYDelta();
		static const float mouseSpeed = 0.1f;

		transform.Rotate(cursorY * _dt * mouseSpeed, cursorX * _dt * mouseSpeed, 0);
	}

	float rotateX = 0;
	float rotateY = 0;

	if (input.KeyDown('C')) rotateX += 1.0f;
	if (input.KeyDown('Z')) rotateX -= 1.0f;
	if (input.KeyDown('F')) rotateY += 1.0f;
	if (input.KeyDown('R')) rotateY -= 1.0f;
	transform.Rotate(rotateY * _dt, rotateX * _dt, 0);
}

void Camera::ClampRotation()
{
	static const float rotationLimit = XM_PIDIV2 - XM_1DIV2PI;
	XMFLOAT3 eulerAngles = transform.GetEulerAngles();
	if (eulerAngles.x > rotationLimit) transform.SetRotation(rotationLimit, eulerAngles.y, eulerAngles.z);
	if (eulerAngles.x < -rotationLimit) transform.SetRotation(-rotationLimit, eulerAngles.y, eulerAngles.z);
}
