#include "Velocity.h"



Velocity::Velocity() : m_velocity(0.0f, 0.0f, 0.0f), m_newVelocity(0.0f, 0.0f, 0.0f)
{

}

Velocity::Velocity(const XMFLOAT3 velocity) : m_velocity(velocity), m_newVelocity(0.0f, 0.0f, 0.0f)
{

}

Velocity::Velocity(const float x, const float y, const float z) : m_velocity(x, y, z), m_newVelocity(0.0f, 0.0f, 0.0f)
{

}

Velocity::Velocity(const Velocity& other) = default;

Velocity::Velocity(Velocity&& other) noexcept = default;

Velocity::~Velocity() = default;

Velocity& Velocity::operator=(const Velocity& other) = default;

//Velocity& Velocity::operator=(Velocity&& other) noexcept = default;

XMFLOAT3 Velocity::GetVelocity() const
{
	return m_velocity;
}

void Velocity::UpdateVelocity()
{
	m_velocity = m_newVelocity;
}


void Velocity::SetNewVelocity(const XMFLOAT3 newVelocity)
{
	m_newVelocity = newVelocity;
}

void Velocity::SetNewVelocity(const float x, const float y, const float z)
{
	m_newVelocity = XMFLOAT3(x, y, z);
}

void Velocity::AddVelocity(const XMFLOAT3 otherVelocity)
{
	m_velocity = XMFLOAT3(m_velocity.x + otherVelocity.x, m_velocity.y + otherVelocity.y, m_velocity.z + otherVelocity.z);
}

void Velocity::AddVelocity(const float x, const float y, const float z)
{
	m_velocity = XMFLOAT3(m_velocity.x + x, m_velocity.y + y, m_velocity.z + z);

}