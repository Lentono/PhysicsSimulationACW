#include "Scale.h"



Scale::Scale() : m_scale(XMVECTOR())
{
	
}

Scale::Scale(const XMFLOAT3 scale) : m_scale(XMLoadFloat3(&scale)) {
}

Scale::Scale(const Scale& other) = default;

Scale::Scale(Scale&& other) noexcept = default;

Scale::~Scale() = default;

Scale& Scale::operator=(const Scale& other) = default;

//Scale& Scale::operator=(Scale&& other) noexcept = default;

void Scale::GetScale(XMVECTOR &scale) const {
	scale = m_scale;
}

void Scale::SetScale(const XMVECTOR newScale) {
	m_scale = newScale;
}

//void Scale::SetScale(const float x, const float y, const float z) {
//	m_scale = XMFLOAT3(x, y, z);
//}

void Scale::AddScale(const XMVECTOR otherScale) {
	m_scale += otherScale;
}

//void Scale::AddScale(const float x, const float y, const float z) {
//	m_scale = XMFLOAT3(m_scale.x + x, m_scale.y + y, m_scale.z + z);
//}