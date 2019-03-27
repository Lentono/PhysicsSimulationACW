#include "RigidBody.h"
#include <complex>

RigidBody::RigidBody(const bool useGravity, const float mass, const float drag, const float angularDrag, const XMFLOAT3 position, const XMFLOAT4 rotation, const XMFLOAT3 velocity, const XMFLOAT3 angularVelocity, const XMFLOAT3X3 inertiaTensor) 
	: m_isAwake(true), m_useGravity(useGravity), m_motion(0.0f), m_inverseMass(1.0f / mass), m_drag(drag), m_angularDrag(angularDrag), m_lastFrameAcceleration(XMVECTOR()), m_position(XMLoadFloat3(&position)), m_newPosition(XMVECTOR()), m_rotation(XMLoadFloat4(&rotation)), m_velocity(XMLoadFloat3(&velocity)), m_newVelocity(XMVECTOR()), m_angularVelocity(XMLoadFloat3(&angularVelocity)), m_accumulatedForce(XMVECTOR()), m_accumulatedTorque(XMVECTOR()) {

	m_rotation = XMQuaternionNormalize(m_rotation);

	SetInertiaTensor(inertiaTensor);

	//m_accumulatedTorque = angularVelocity / 2;
}

RigidBody::RigidBody(const RigidBody& other) = default;

RigidBody::RigidBody(RigidBody&& other) noexcept = default;

RigidBody::~RigidBody() = default;

RigidBody& RigidBody::operator=(const RigidBody& other) = default;

//RigidBody& RigidBody::operator=(RigidBody&& other) noexcept = default;

bool RigidBody::GetIsAwake() const
{
	return m_isAwake;
}

bool RigidBody::GetUseGravity() const {
	return m_useGravity;
}

float RigidBody::GetSleepEpsilon() const
{
	//const auto sleepEpsilon = 0.3f;
	return 0.2f;
}


float RigidBody::GetMotion() const
{
	return m_motion;
}

float RigidBody::GetMass() const {
	
	if (m_inverseMass == 0)
	{
		return numeric_limits<float>::max();
	}
		
	return (1.0f) / m_inverseMass;
}

float RigidBody::GetInverseMass() const {
	return m_inverseMass;
}

float RigidBody::GetDrag() const {
	return m_drag;
}

float RigidBody::GetAngularDrag() const {
	return m_angularDrag;
}

void RigidBody::GetLastFrameAcceleration(XMVECTOR &getLastFrameAcceleration) const
{
	getLastFrameAcceleration = m_lastFrameAcceleration;
}

void RigidBody::GetPosition(XMVECTOR &position) const
{
	position = m_position;
}

void RigidBody::GetNewPosition(XMVECTOR &position) const
{
	position = m_newPosition;
}

void RigidBody::GetRotation(XMVECTOR &rotation) const
{
	rotation = m_rotation;
}

void RigidBody::GetVelocity(XMVECTOR &velocity) const
{
	velocity = m_velocity;
}

void RigidBody::GetNewVelocity(XMVECTOR &velocity) const
{
	velocity = m_newVelocity;
}


void RigidBody::GetAccumulatedForce(XMVECTOR &accumulatedForce) const
{
	accumulatedForce = m_accumulatedForce;
}

void RigidBody::GetAccumulatedTorque(XMVECTOR &accumulatedTorque) const
{
	accumulatedTorque = m_accumulatedTorque;
}

void RigidBody::GetAngularVelocity(XMVECTOR &angularVelocity) const
{
	angularVelocity = m_angularVelocity;
}

XMFLOAT3X3 RigidBody::GetInertiaTensor() const
{
	auto inertiaTensor = XMFLOAT3X3();

	auto determinant = XMMatrixDeterminant(XMLoadFloat3x3(&m_inverseInertiaTensor));

	XMStoreFloat3x3(&inertiaTensor, XMMatrixInverse(&determinant, XMLoadFloat3x3(&m_inverseInertiaTensor)));

	return inertiaTensor;
}

void RigidBody::GetInverseInertiaTensorWorld(XMMATRIX &inverseInertiaTensorInWorld) const
{
	inverseInertiaTensorInWorld = m_inverseInertiaTensorInWorld;
}

void RigidBody::SetIsAwake(const bool isAwake)
{
	if (isAwake)
	{
		m_isAwake = isAwake;

		//Need to add a bit of motion so the object doesn't immediately fall asleep
		m_motion = GetSleepEpsilon() * 2.0f;
	}
	else
	{
		m_isAwake = isAwake;
		m_velocity = XMVECTOR();
		m_newVelocity = XMVECTOR();
		m_angularVelocity = XMVECTOR();
	}
}

void RigidBody::SetUseGravity(const bool useGravity) {
	m_useGravity = useGravity;
}

void RigidBody::SetMotion(const float motion)
{
	m_motion = motion;
}

void RigidBody::SetMass(const float mass) {
	m_inverseMass = 1.0f / mass;
}

void RigidBody::SetInverseMass(const float inverseMass)
{
	m_inverseMass = inverseMass;
}

void RigidBody::SetDrag(const float drag) {
	m_drag = drag;
}

void RigidBody::SetAngularDrag(const float angularDrag) {
	m_angularDrag = angularDrag;
}

void RigidBody::SetLastFrameAcceleration(const XMVECTOR &newLastFrameAcceleration)
{
	m_lastFrameAcceleration = newLastFrameAcceleration;
}

void RigidBody::SetNewPosition(const XMVECTOR &newPosition)
{
	m_newPosition = newPosition;
}

//void RigidBody::SetNewPosition(const float x, const float y, const float z)
//{
//	m_newPosition = XMFLOAT3(x, y, z);
//}

void RigidBody::SetRotation(const XMVECTOR &newRotation)
{
	m_rotation = XMQuaternionNormalize(newRotation);
}

//void RigidBody::SetRotation(const float x, const float y, const float z, const float w)
//{
//	m_rotation = XMFLOAT4(x, y, z, w);
//}

void RigidBody::SetNewVelocity(const XMVECTOR &newVelocity)
{
	m_newVelocity = newVelocity;
}

//void RigidBody::SetNewVelocity(const float x, const float y, const float z)
//{
//	m_newVelocity = XMFLOAT3(x, y, z);
//}

void RigidBody::SetAngularVelocity(const XMVECTOR &angularVelocity)
{
	m_angularVelocity = angularVelocity;
}

void RigidBody::SetInertiaTensor(const XMFLOAT3X3 &inertiaTensor)
{
	auto determinant = XMMatrixDeterminant(XMLoadFloat3x3(&inertiaTensor));

	XMStoreFloat3x3(&m_inverseInertiaTensor, XMMatrixInverse(&determinant, XMLoadFloat3x3(&inertiaTensor)));
}

void RigidBody::UpdatePosition()
{
	m_position = m_newPosition;
}

void RigidBody::UpdateVelocity()
{
	m_velocity = m_newVelocity;
}

bool RigidBody::HasFiniteMass() const
{
	return m_inverseMass >= 0.0f;
}

void RigidBody::AddAccumulatedForce(const XMVECTOR& force)
{
	m_accumulatedForce += force;
	m_isAwake = true;
}

void RigidBody::AddAccumulatedTorque(const XMVECTOR& torque)
{
	m_accumulatedTorque += torque;
	m_isAwake = true;
}

void RigidBody::ClearAccumulators()
{
	m_accumulatedForce = XMVECTOR();
	m_accumulatedTorque = XMVECTOR();
}

void RigidBody::AddForceAtLocalPoint(const XMFLOAT3& force, const XMFLOAT3& point)
{
	//const auto worldPoint = GetPointInWorldSpace(point);
	//AddForceAtPoint(force, worldPoint);
}

void RigidBody::CalculateDerivedData()
{
	m_rotation = XMQuaternionNormalize(m_rotation);

	m_transformMatrix = XMMatrixRotationQuaternion(m_rotation);
	m_transformMatrix = XMMatrixMultiply(m_transformMatrix, XMMatrixTranslationFromVector(m_newPosition));

	//CalculateTransformMatrix(m_transformMatrix, m_newPosition, m_rotation);
	//Calculate the transform matrix for the rigidbody

	auto tempTransformMatrix = XMFLOAT4X4();
	XMStoreFloat4x4(&tempTransformMatrix, m_transformMatrix);

	auto inverseInertiaTensorInWorld = XMFLOAT3X3();

	InertiaTensorTransformLocalToWorld(inverseInertiaTensorInWorld, m_inverseInertiaTensor, tempTransformMatrix);

	m_inverseInertiaTensorInWorld = XMLoadFloat3x3(&inverseInertiaTensorInWorld);
}

//XMFLOAT3 RigidBody::GetPointInWorldSpace(const XMFLOAT3& point) const
//{
//	return { point.x * m_transformMatrix._11 +
//			point.y * m_transformMatrix._12 +
//			point.z * m_transformMatrix._13 + m_transformMatrix._14,
//		
//			point.x * m_transformMatrix._21 +
//			point.y * m_transformMatrix._22 +
//			point.z * m_transformMatrix._23 + m_transformMatrix._24,
//
//			point.x * m_transformMatrix._31 +
//			point.y * m_transformMatrix._32 +
//			point.z * m_transformMatrix._33 + m_transformMatrix._34 
//	};
//}

//void RigidBody::AddForceAtPoint(const XMFLOAT3& force, const XMFLOAT3& point)
//{
//	XMFLOAT3 pt = point;
//	pt = pt - m_position;
//
//	m_accumulatedForce = m_accumulatedForce + force;
//
//	auto crossProduct = XMFLOAT3();
//
//	XMStoreFloat3(&crossProduct, XMVector3Cross(XMLoadFloat3(&pt), XMLoadFloat3(&force)));
//
//	m_accumulatedTorque = m_accumulatedTorque + (crossProduct);
//}

//Automated optimised code used from Ian Millingtons book on Game Physics Engine Development: Edition 1
void RigidBody::CalculateTransformMatrix(XMFLOAT4X4 &transformMatrix, const XMFLOAT3 &position, const XMFLOAT4 &rotation)
{

	transformMatrix._11 = 1 - 2 * rotation.y * rotation.y - 2 * rotation.z * rotation.z;
	transformMatrix._12 = 2 * rotation.x * rotation.y - 2 * rotation.w  *rotation.z;
	transformMatrix._13 = 2 * rotation.x * rotation.z + 2 * rotation.w * rotation.y;
	transformMatrix._14 = position.x;

	transformMatrix._21 = 2 * rotation.x * rotation.y + 2 * rotation.w * rotation.z;
	transformMatrix._22 = 1 - 2 * rotation.x * rotation.x - 2 * rotation.z * rotation.z;
	transformMatrix._23 = 2 * rotation.y * rotation.z - 2 * rotation.w * rotation.x;
	transformMatrix._24 = position.y;

	transformMatrix._31 = 2 * rotation.x * rotation.z - 2 * rotation.w * rotation.y;
	transformMatrix._32 = 2 * rotation.y * rotation.z + 2 * rotation.w * rotation.x;
	transformMatrix._33 = 1 - 2 * rotation.x * rotation.x - 2 * rotation.y * rotation.y;
	transformMatrix._34 = position.z;

	transformMatrix._41 = 0;
	transformMatrix._42 = 0;
	transformMatrix._43 = 0;
	transformMatrix._44 = 1;
}

//Automated optimised code used from Ian Millingtons book on Game Physics Engine Development: Edition 1
void RigidBody::InertiaTensorTransformLocalToWorld(XMFLOAT3X3& inverseInertiaTensorInWorld, const XMFLOAT3X3& inertiaTensorInLocal, const XMFLOAT4X4& transformMatrix)
{
	//Only using the rotation data from our transform matrix
	auto t4 = transformMatrix._11 * inertiaTensorInLocal._11 +
		transformMatrix._12 * inertiaTensorInLocal._21 +
		transformMatrix._13 * inertiaTensorInLocal._31;
	auto t9 = transformMatrix._11 * inertiaTensorInLocal._12 +
		transformMatrix._12 * inertiaTensorInLocal._22 +
		transformMatrix._13 * inertiaTensorInLocal._32;
	auto t14 = transformMatrix._11 * inertiaTensorInLocal._13 +
		transformMatrix._12 * inertiaTensorInLocal._23 +
		transformMatrix._13 * inertiaTensorInLocal._33;

	auto t28 = transformMatrix._21 * inertiaTensorInLocal._11 +
		transformMatrix._22 * inertiaTensorInLocal._21 +
		transformMatrix._23 * inertiaTensorInLocal._31;
	auto t33 = transformMatrix._21 * inertiaTensorInLocal._12 +
		transformMatrix._22 * inertiaTensorInLocal._22 +
		transformMatrix._23 * inertiaTensorInLocal._32;
	auto t38 = transformMatrix._21 * inertiaTensorInLocal._13 +
		transformMatrix._22 * inertiaTensorInLocal._23 +
		transformMatrix._23 * inertiaTensorInLocal._33;

	auto t52 = transformMatrix._31 * inertiaTensorInLocal._11 +
		transformMatrix._32 * inertiaTensorInLocal._21 +
		transformMatrix._33 * inertiaTensorInLocal._31;
	auto t57 = transformMatrix._31 * inertiaTensorInLocal._12 +
		transformMatrix._32 * inertiaTensorInLocal._22 +
		transformMatrix._33 * inertiaTensorInLocal._32;
	auto t62 = transformMatrix._31 * inertiaTensorInLocal._13 +
		transformMatrix._32 * inertiaTensorInLocal._23 +
		transformMatrix._33 * inertiaTensorInLocal._33;

	inverseInertiaTensorInWorld._11 = t4 * transformMatrix._11 +
		t9 * transformMatrix._12 +
		t14 * transformMatrix._13;
	inverseInertiaTensorInWorld._12 = t4 * transformMatrix._21 +
		t9 * transformMatrix._22 +
		t14 * transformMatrix._23;
	inverseInertiaTensorInWorld._13 = t4 * transformMatrix._31 +
		t9 * transformMatrix._32 +
		t14 * transformMatrix._33;

	inverseInertiaTensorInWorld._21 = t28 * transformMatrix._11 +
		t33 * transformMatrix._12 +
		t38 * transformMatrix._13;
	inverseInertiaTensorInWorld._22 = t28 * transformMatrix._21 +
		t33 * transformMatrix._22 +
		t38 * transformMatrix._23;
	inverseInertiaTensorInWorld._23 = t28 * transformMatrix._31 +
		t33 * transformMatrix._32 +
		t38 * transformMatrix._33;

	inverseInertiaTensorInWorld._31 = t52 * transformMatrix._11 +
		t57 * transformMatrix._12 +
		t62 * transformMatrix._13;
	inverseInertiaTensorInWorld._32 = t52 * transformMatrix._21 +
		t57 * transformMatrix._22 +
		t62 * transformMatrix._23;
	inverseInertiaTensorInWorld._33 = t52 * transformMatrix._31 +
		t57 * transformMatrix._32 +
		t62 * transformMatrix._33;
}
