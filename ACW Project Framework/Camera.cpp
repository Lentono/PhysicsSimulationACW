#include "Camera.h"
#include <memory>

#define _USE_MATH_DEFINES_

Camera::Camera() : m_positionX(0), m_positionY(0), m_positionZ(0), m_rotationX(0), m_rotationY(0), m_rotationZ(0)
{
}

Camera::Camera(const Camera& other) = default;

Camera::Camera(Camera&& other) = default;

Camera::~Camera() = default;

Camera& Camera::operator=(const Camera& other) = default;

//CameraClass& CameraClass::operator=(CameraClass&& other) noexcept = default;

XMFLOAT3 Camera::GetPosition() const {
	return XMFLOAT3(m_positionX, m_positionY, m_positionZ);
}

XMFLOAT3 Camera::GetRotation() const {
	return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
}

void Camera::SetPosition(const float x, const float y, const float z) {
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
}

void Camera::SetRotation(const float x, const float y, const float z) {
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
}

void Camera::AddPositionX(const float x)
{
	m_positionX += x;
}

void Camera::AddPositionY(const float y)
{
	m_positionY += y;
}

void Camera::AddPositionZ(const float z)
{
	m_positionZ += z;
}

void Camera::GetViewMatrix(XMMATRIX& viewMatrix) const {
	viewMatrix = m_viewMatrix;
}

void Camera::Render() {
	const auto positionVector = XMVectorSet(m_positionX, m_positionY, m_positionZ, 0.0f);

	const auto yaw = XMConvertToRadians(m_rotationX);
	const auto pitch = XMConvertToRadians(m_rotationY);
	const auto roll = XMConvertToRadians(m_rotationZ);

	const auto rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	//Transform the upVector and lookAtVector by our rotation matrix so the camera is rotated at the origin
	const auto upVector = XMVector3TransformCoord(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), rotationMatrix);
	auto lookAtVector = XMVector3TransformCoord(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotationMatrix);

	//Translate camera to look at position
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	//Create view matrix
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}
