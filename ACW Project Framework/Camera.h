#pragma once

#include <DirectXMath.h>
#include <memory>

using namespace DirectX;

class Camera
{
public:
	Camera(); // Default Constructor
	Camera(const Camera& other); // Copy Constructor
	Camera(Camera && other); // Move Constructor
	~Camera();

	Camera& operator = (const Camera& other); // Copy Assignment Operator
	//CameraClass& operator = (CameraClass && other) noexcept; // Move Assignment Operator

	XMFLOAT3 GetPosition() const;
	XMFLOAT3 GetRotation() const;

	void SetPosition(const float x, const float y, const float z);
	void SetRotation(const float x, const float y, const float z);

	void AddPositionX(const float x);
	void AddPositionY(const float y);
	void AddPositionZ(const float z);

	void GetViewMatrix(XMMATRIX &viewMatrix) const;

	void Render();

private:
	float m_positionX;
	float m_positionY;
	float m_positionZ;

	float m_rotationX;
	float m_rotationY;
	float m_rotationZ;

	XMMATRIX m_viewMatrix;
};