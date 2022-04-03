#pragma once

#include <DirectXMath.h>

constexpr auto LIGHT_TYPE_DIRECTIONAL	= 0;
constexpr auto LIGHT_TYPE_POINT			= 1;
constexpr auto LIGHT_TYPE_SPOT			= 2;

struct Light
{
	int					Type;
	DirectX::XMFLOAT3	Direction;
	float				Range;
	DirectX::XMFLOAT3	Position;
	float				Intensity;
	DirectX::XMFLOAT3	Color;
	float				SpotFalloff;
	DirectX::XMFLOAT3	Padding;

	static Light Directional(DirectX::XMFLOAT3 _direction, DirectX::XMFLOAT3 _color, float _intensity)
	{
		Light light = {};
		light.Type = LIGHT_TYPE_DIRECTIONAL;
		light.Direction = _direction;
		light.Color = _color;
		light.Intensity = _intensity;
		return light;
	};

	static Light Point(DirectX::XMFLOAT3 _position, DirectX::XMFLOAT3 _color, float _intensity, float _range)
	{
		Light light = {};
		light.Type = LIGHT_TYPE_POINT;
		light.Position = _position;
		light.Color = _color;
		light.Intensity = _intensity;
		light.Range = _range;
		return light;
	};
};
