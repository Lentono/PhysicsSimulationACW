#pragma once

#include <DirectXMath.h>

using namespace DirectX;

class Light
{
public:
	Light(); // Default Constructor
	Light(const Light& other); // Copy Constructor
	Light(Light&& other) noexcept; // Move Constructor
	~Light(); // Destructor

	Light& operator = (const Light& other); // Copy Assignment Operator
	//Light& operator = (Light&& other) noexcept; // Move Assignment Operator

	void SetDiffuseColour(const float red, const float green, const float blue, const float alpha);
	void SetLightDirection(const float x, const float y, const float z);

	XMFLOAT4 GetDiffuseColour() const;
	XMFLOAT3 GetLightDirection() const;

private:
	XMFLOAT4 m_diffuseColour;
	XMFLOAT3 m_lightDirection;

};