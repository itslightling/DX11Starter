#pragma once

#include <DirectXMath.h>

class Transform
{
public:
	Transform();

	DirectX::XMFLOAT3		GetPosition();
	DirectX::XMFLOAT3		GetEulerAngles();
	DirectX::XMFLOAT3		GetPitchYawRoll(); // an alternative name for euler angles in case preferred
	DirectX::XMFLOAT3		GetScale();
	DirectX::XMFLOAT4X4		GetWorldMatrix();
	DirectX::XMFLOAT4X4		GetWorldMatrixInverseTranspose();
	DirectX::XMFLOAT3		GetRight();
	DirectX::XMFLOAT3		GetUp();
	DirectX::XMFLOAT3		GetForward();

	void					SetPosition(float _x, float _y, float _z);
	void					SetRotation(float _pitch, float _yaw, float _roll);
	void					SetScale(float _x, float _y, float _z);

	void					TranslateAbsolute(float _x, float _y, float _z);
	void					TranslateRelative(float _x, float _y, float _z);
	void					Rotate(float _pitch, float _yaw, float _roll);
	void					Scale(float _x, float _y, float _z);

private:
	DirectX::XMFLOAT3		position;
	DirectX::XMFLOAT3		eulerAngles;
	DirectX::XMFLOAT3		scale;
	DirectX::XMFLOAT4X4		worldMatrix;
	DirectX::XMFLOAT4X4		worldMatrixInverseTranspose;
	bool					worldMatrixChanged;
	DirectX::XMFLOAT3		right;
	DirectX::XMFLOAT3		up;
	DirectX::XMFLOAT3		forward;

	void					UpdateWorldMatrix();
	void					UpdateDirections();
};
