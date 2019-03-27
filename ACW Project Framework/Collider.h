#pragma once

#include <DirectXMath.h>
#include "XMFLOAT3Maths.h"

using namespace DirectX;

class Collider
{
public:
	enum ColliderType
	{
		Sphere,
		AABBCube,
		OBBCube,
		Plane,
		Cylinder
	};

	Collider();
	virtual ~Collider();

	virtual ColliderType GetCollider() const = 0;
};

class SphereCollider : public Collider {

public:
	SphereCollider();
	~SphereCollider();

	ColliderType GetCollider() const override;
};

class AABBCubeCollider : public Collider {

public:
	AABBCubeCollider();
	~AABBCubeCollider();

	ColliderType GetCollider() const override;
};

class OBBCubeCollider : public Collider {

public:
	OBBCubeCollider();
	~OBBCubeCollider();

	ColliderType GetCollider() const override;
};

class PlaneCollider : public Collider {

public:
	PlaneCollider();
	~PlaneCollider();

	ColliderType GetCollider() const override;
	XMVECTOR GetNormal() const;
	float GetOffset() const;

	void SetNormal(const XMFLOAT3& centre, const XMFLOAT3& pointOne, const XMFLOAT3& pointTwo);
	void SetOffset(const float offset);

private:
	XMVECTOR m_normal;
	float m_offset;
	float m_offsetTest;

};

class CylinderCollider : public Collider {

public:
	CylinderCollider();
	~CylinderCollider();

	ColliderType GetCollider() const override;
};