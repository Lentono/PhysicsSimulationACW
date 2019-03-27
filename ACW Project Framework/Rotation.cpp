#include "Rotation.h"



Rotation::Rotation() : m_rotation(0.0f, 0.0f, 0.0f, 0.0f)
{
}

Rotation::Rotation(const XMFLOAT4 rotation) : m_rotation(rotation) {
}

Rotation::Rotation(const float x, const float y, const float z, const float w) : m_rotation(x, y, z, w) {
}

Rotation::Rotation(const Rotation& other) = default;

Rotation::Rotation(Rotation&& other) noexcept = default;

Rotation::~Rotation() = default;

Rotation& Rotation::operator=(const Rotation& other) = default;

//Rotation& Rotation::operator=(Rotation&& other) noexcept = default;

XMFLOAT4 Rotation::GetRotation() const {
	return m_rotation;
}

void Rotation::SetRotation(const XMFLOAT4 newRotation) {
	m_rotation = newRotation;
	Normalize();
}

void Rotation::SetRotation(const float x, const float y, const float z, const float w) {
	m_rotation = XMFLOAT4(x, y, z, w);
	Normalize();
}

void Rotation::AddRotation(const XMFLOAT4 otherRotation) {
	//m_rotation += otherRotation;
}

void Rotation::AddRotation(const float x, const float y, const float z, const float w) {
	//m_rotation += XMFLOAT4(x, y, z, w);
}

void Rotation::Normalize()
{
	auto d = m_rotation.x * m_rotation.x +
			m_rotation.y * m_rotation.y +
			m_rotation.z * m_rotation.z +
			m_rotation.w * m_rotation.w;

	//I don't get this bit
	if (d == 0)
	{
		m_rotation.x = 1;
		return;
	}

	d = (1.0f) / sqrt(d);

	m_rotation.x *= d;
	m_rotation.y *= d;
	m_rotation.z *= d;
	m_rotation.w *= d;
}
