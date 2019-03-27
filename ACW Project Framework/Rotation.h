#pragma once

#include <DirectXMath.h>

using namespace DirectX;

class Rotation
{
public:
	Rotation(); // Default Constructor
	Rotation(const XMFLOAT4 rotation);
	Rotation(const float x, const float y, const float z, const float w);
	Rotation(const Rotation& other); // Copy Constructor
	Rotation(Rotation&& other) noexcept; // Move Constructor
	~Rotation();

	Rotation& operator = (const Rotation& other); // Copy Assignment Operator
	Rotation& operator = (Rotation&& other) noexcept; // Move Assignment Operator

	XMFLOAT4 GetRotation() const;

	void SetRotation(const XMFLOAT4 newRotation);
	void SetRotation(const float x, const float y, const float z, const float w);

	void AddRotation(const XMFLOAT4 otherRotation);
	void AddRotation(const float x, const float y, const float z, const float w);

private:
	void Normalize();

	//Our angular orientation as a quaternion
	XMFLOAT4 m_rotation;
};

