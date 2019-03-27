#pragma once

#include <DirectXMath.h>

using namespace DirectX;

class Scale
{

public:
	Scale(); // Default Constructor
	Scale(const XMFLOAT3 scale);
	Scale(const Scale& other); // Copy Constructor
	Scale(Scale&& other) noexcept; // Move Constructor
	~Scale(); // Destructor

	Scale& operator = (const Scale& other); // Copy Assignment Operator
	Scale& operator = (Scale&& other) noexcept; // Move Assignment Operator

	void GetScale(XMVECTOR &scale) const;

	void SetScale(const XMVECTOR newScale);
	//void SetScale(const float x, const float y, const float z);

	void AddScale(const XMVECTOR otherScale);
	//void AddScale(const float x, const float y, const float z);

private:
	XMVECTOR m_scale;
};

