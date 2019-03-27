#pragma once
#include <DirectXMath.h>

using namespace DirectX;

class Position
{
public:
	Position();
	Position(XMFLOAT3 position);
	Position(float x, float y, float z);
	Position(const Position& other); // Copy Constructor
	Position(Position&& other) noexcept; // Move Constructor
	~Position(); //Destructor

	Position& operator = (const Position& other); // Copy Assignment Operator
	Position& operator = (Position&& other) noexcept; // Move Assignment Operator

	XMFLOAT3 GetPosition() const;
	void UpdatePosition();

	void SetNewPosition(const XMFLOAT3 newPosition);
	void SetNewPosition(const float x, const float y, const float z);

	void TranslatePosition(const XMFLOAT3 translate);
	void TranslatePosition(const float x, const float y, const float z);

private:
	XMFLOAT3 m_position;
	XMFLOAT3 m_newPosition;
};

