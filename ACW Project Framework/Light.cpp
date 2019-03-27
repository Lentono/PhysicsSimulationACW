#include "Light.h"



Light::Light() : m_diffuseColour(0.0f, 0.0f, 0.0f, 0.0f), m_lightDirection(0.0f, 0.0f, 0.0f) {
	
}

Light::Light(const Light& other) = default;

Light::Light(Light&& other) noexcept = default;

Light::~Light() = default;

Light& Light::operator=(const Light& other) = default;

//Light& Light::operator=(Light&& other) noexcept = default;

void Light::SetDiffuseColour(const float red, const float green, const float blue, const float alpha) {
	m_diffuseColour = XMFLOAT4(red, green, blue, alpha);
}

void Light::SetLightDirection(const float x, const float y, const float z) {
	m_lightDirection = XMFLOAT3(x, y, z);
}

XMFLOAT4 Light::GetDiffuseColour() const {
	return m_diffuseColour;
}

XMFLOAT3 Light::GetLightDirection() const {
	return m_lightDirection;
}

