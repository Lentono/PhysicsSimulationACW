#pragma once

#include <DirectXMath.h>

using namespace DirectX;

class Velocity
{
public:
	Velocity();
	Velocity(const XMFLOAT3 velocity);
	Velocity(const float x, const float y, const float z);
	Velocity(const Velocity& other); // Copy Constructor
	Velocity(Velocity&& other) noexcept; // Move Constructor
	~Velocity(); // Destructor

	Velocity& operator = (const Velocity& other); // Copy Assignment Operator
	Velocity& operator = (Velocity&& other) noexcept; // Move Assignment Operator

	XMFLOAT3 GetVelocity() const;
	void UpdateVelocity();

	void SetNewVelocity(const XMFLOAT3 newVelocity);
	void SetNewVelocity(const float x, const float y, const float z);

	void AddVelocity(const XMFLOAT3 otherVelocity);
	void AddVelocity(const float x, const float y, const float z);

private:
	XMFLOAT3 m_velocity;
	XMFLOAT3 m_newVelocity;
};