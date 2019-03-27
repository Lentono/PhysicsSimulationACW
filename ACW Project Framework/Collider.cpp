#include "Collider.h"

Collider::Collider() = default;

Collider::~Collider() = default;

SphereCollider::SphereCollider() = default;

SphereCollider::~SphereCollider() = default;

Collider::ColliderType SphereCollider::GetCollider() const
{
	return Sphere;
}

AABBCubeCollider::AABBCubeCollider() = default;

AABBCubeCollider::~AABBCubeCollider() = default;

Collider::ColliderType AABBCubeCollider::GetCollider() const
{
	return AABBCube;
}

OBBCubeCollider::OBBCubeCollider() = default;

OBBCubeCollider::~OBBCubeCollider() = default;

Collider::ColliderType OBBCubeCollider::GetCollider() const
{
	return OBBCube;
}

PlaneCollider::PlaneCollider() = default;

PlaneCollider::~PlaneCollider() = default;

Collider::ColliderType PlaneCollider::GetCollider() const
{
	return Plane;
}

XMVECTOR PlaneCollider::GetNormal() const
{
	return m_normal;
}

float PlaneCollider::GetOffset() const
{
	return m_offset;
}

void PlaneCollider::SetNormal(const XMFLOAT3& centre, const XMFLOAT3& pointOne, const XMFLOAT3& pointTwo)
{
	const auto crossOne = pointOne - centre;
	const auto crossTwo = pointTwo - centre;

	m_normal = XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&crossOne), XMLoadFloat3(&crossTwo)));

	XMStoreFloat(&m_offsetTest, XMVector3Dot(m_normal, XMLoadFloat3(&centre)));
}

void PlaneCollider::SetOffset(const float offset)
{
	m_offset = offset;
}

CylinderCollider::CylinderCollider() = default;

CylinderCollider::~CylinderCollider() = default;

Collider::ColliderType CylinderCollider::GetCollider() const
{
	return Cylinder;
}
