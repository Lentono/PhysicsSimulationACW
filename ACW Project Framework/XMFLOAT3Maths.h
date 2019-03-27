#pragma once

#include <DirectXMath.h>

using namespace DirectX;

class XMFLOAT3Maths
{
public:

private:

	XMFLOAT3Maths();
	~XMFLOAT3Maths();
};

static XMFLOAT3 operator+ (const XMFLOAT3 &lhs, const XMFLOAT3 &rhs)
{
	return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
};

static XMFLOAT3 operator- (const XMFLOAT3 &lhs, const XMFLOAT3 &rhs)
{
	return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
};

static XMFLOAT3 operator* (const XMFLOAT3 &lhs, const float n)
{
	return { lhs.x * n, lhs.y * n, lhs.z * n };
};

static XMFLOAT3 operator* (const float n, const XMFLOAT3 &rhs)
{
	return { rhs.x * n, rhs.y * n, rhs.z * n };
};

static XMFLOAT3 operator/ (const XMFLOAT3 &lhs, const float n)
{
	return { lhs.x / n, lhs.y / n, lhs.z / n };
};

static XMFLOAT4 operator+ (const XMFLOAT4 &lhs, const XMFLOAT4 &rhs)
{

	auto quaternion = XMFLOAT4();

	XMStoreFloat4(&quaternion, XMQuaternionMultiply(XMLoadFloat4(&lhs), XMLoadFloat4(&rhs)));

	//Vector actually gets a value in the w...
	//XMVECTOR testtwo = XMQuaternionMultiply(XMLoadFloat4(&lhs), XMLoadFloat4(&rhs));

	//return { lhs.x + quaternion.x * 0.5f, lhs.y + quaternion.y * 0.5f, lhs.z + quaternion.z * 0.5f, lhs.z + quaternion.z * 0.5f};
	return quaternion;
}