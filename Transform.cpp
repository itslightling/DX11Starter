#include "Transform.h"

using namespace DirectX;

Transform::Transform()
{
	SetPosition(0, 0, 0);
	SetRotation(0, 0, 0);
	SetScale(1, 1, 1);

	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&worldMatrixInverseTranspose, XMMatrixIdentity());
	worldMatrixChanged = false;
}

DirectX::XMFLOAT3 Transform::GetPosition()		{ return position; }
DirectX::XMFLOAT3 Transform::GetEulerAngles()	{ return eulerAngles; }
DirectX::XMFLOAT3 Transform::GetPitchYawRoll()	{ return eulerAngles;  }
DirectX::XMFLOAT3 Transform::GetScale()			{ return scale; }

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
	if (worldMatrixChanged)
	{
		UpdateWorldMatrix();
		worldMatrixChanged = false;
	}
	return worldMatrix;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrixInverseTranspose()
{
	if (worldMatrixChanged)
	{
		UpdateWorldMatrix();
		worldMatrixChanged = false;
	}
	return worldMatrixInverseTranspose;
}

DirectX::XMFLOAT3 Transform::GetRight()
{
	return right;
}

DirectX::XMFLOAT3 Transform::GetUp()
{
	return up;
}

DirectX::XMFLOAT3 Transform::GetForward()
{
	return forward;
}

// XMVECTOR & XMStoreFloat compiles down to something faster than position += x,y,z because it happens all at once

void Transform::SetPosition(float _x, float _y, float _z)
{
	XMVECTOR newPosition = XMVectorSet(_x, _y, _z, 0);
	XMStoreFloat3(&position, newPosition);
	worldMatrixChanged = true;
}

void Transform::SetRotation(float _pitch, float _yaw, float _roll)
{
	XMVECTOR newRotation = XMVectorSet(_pitch, _yaw, _roll, 0);
	XMStoreFloat3(&eulerAngles, newRotation);
	worldMatrixChanged = true;
	UpdateDirections();
}

void Transform::SetScale(float _x, float _y, float _z)
{
	XMVECTOR newScale = XMVectorSet(_x, _y, _z, 0);
	XMStoreFloat3(&scale, newScale);
	worldMatrixChanged = true;
}

void Transform::TranslateAbsolute(float _x, float _y, float _z)
{
	XMVECTOR newPosition = XMLoadFloat3(&position);
	XMVECTOR offset = XMVectorSet(_x, _y, _z, 0);
	XMStoreFloat3(&position, newPosition + offset);
	worldMatrixChanged = true;
}

void Transform::TranslateRelative(float _x, float _y, float _z)
{
	XMVECTOR moveVector = XMVectorSet(_x, _y, _z, 0);
	XMVECTOR rotateVector = XMVector3Rotate(moveVector, XMQuaternionRotationRollPitchYaw(eulerAngles.x, eulerAngles.y, eulerAngles.z));
	XMVECTOR newPosition = XMLoadFloat3(&position) + rotateVector;
	XMStoreFloat3(&position, newPosition);
	worldMatrixChanged = true;
}

void Transform::Rotate(float _pitch, float _yaw, float _roll)
{
	XMVECTOR newRotation = XMLoadFloat3(&position);
	XMVECTOR offset = XMVectorSet(_pitch, _yaw, _roll, 0);
	XMStoreFloat3(&eulerAngles, newRotation + offset);
	worldMatrixChanged = true;
	UpdateDirections();
}

void Transform::Scale(float _x, float _y, float _z)
{
	XMVECTOR newScale = XMLoadFloat3(&position);
	XMVECTOR offset = XMVectorSet(_x, _y, _z, 0);
	XMStoreFloat3(&scale, newScale + offset);
	worldMatrixChanged = true;
}

void Transform::UpdateWorldMatrix()
{
	XMMATRIX matrixPosition = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX matrixRotation = XMMatrixRotationRollPitchYaw(eulerAngles.x, eulerAngles.y, eulerAngles.z);
	XMMATRIX matrixScale = XMMatrixScaling(scale.x, scale.y, scale.z);

	XMStoreFloat4x4(&worldMatrix, matrixScale * matrixRotation * matrixPosition);
	XMStoreFloat4x4(&worldMatrixInverseTranspose, XMMatrixInverse(0, XMMatrixTranspose(XMLoadFloat4x4(&worldMatrix))));
}

void Transform::UpdateDirections()
{
	XMStoreFloat3(&right,   XMVector3Rotate(XMVectorSet(1, 0, 0, 0), XMQuaternionRotationRollPitchYaw(eulerAngles.x, eulerAngles.y, eulerAngles.z)));
	XMStoreFloat3(&up,      XMVector3Rotate(XMVectorSet(0, 1, 0, 0), XMQuaternionRotationRollPitchYaw(eulerAngles.x, eulerAngles.y, eulerAngles.z)));
	XMStoreFloat3(&forward, XMVector3Rotate(XMVectorSet(0, 0, 1, 0), XMQuaternionRotationRollPitchYaw(eulerAngles.x, eulerAngles.y, eulerAngles.z)));
}
