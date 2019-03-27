#pragma once
#include <limits>
#include "XMFLOAT3Maths.h"

using namespace std;
using namespace DirectX;

class RigidBody
{
public:
	RigidBody(const bool useGravity, const float mass, const float drag, const float angularDrag, const XMFLOAT3 position, const XMFLOAT4 rotation, const XMFLOAT3 velocity, const XMFLOAT3 angularVelocity, const XMFLOAT3X3 inertiaTensor);
	RigidBody(const RigidBody& other); // Copy Constructor
	RigidBody(RigidBody&& other) noexcept; // Move Constructor
	~RigidBody(); // Destructor

	RigidBody& operator = (const RigidBody& other); // Copy Assignment Operator
	RigidBody& operator = (RigidBody&& other) noexcept; // Move Assignment Operator

	//Gets
	bool GetIsAwake() const;
	bool GetUseGravity() const;
	float GetSleepEpsilon() const;
	float GetMotion() const;
	float GetMass() const;
	float GetInverseMass() const;
	float GetDrag() const;
	float GetAngularDrag() const;

	void GetLastFrameAcceleration(XMVECTOR &getLastFrameAcceleration) const;

	void GetPosition(XMVECTOR &position) const;
	void GetNewPosition(XMVECTOR &position) const;
	void GetRotation(XMVECTOR &rotation) const;
	void GetVelocity(XMVECTOR &velocity) const;
	void GetNewVelocity(XMVECTOR &velocity) const;

	void GetAccumulatedForce(XMVECTOR &accumulatedForce) const;
	void GetAccumulatedTorque(XMVECTOR &accumulatedTorque) const;
	void GetAngularVelocity(XMVECTOR &angularVelocity) const;

	XMFLOAT3X3 GetInertiaTensor() const;
	void GetInverseInertiaTensorWorld(XMMATRIX &inverseInertiaTensorInWorld) const;

	//Sets
	void SetIsAwake(const bool isAwake);
	void SetUseGravity(const bool useGravity);
	void SetMotion(const float motion);
	void SetMass(const float mass);
	void SetInverseMass(const float inverseMass);
	void SetDrag(const float drag);
	void SetAngularDrag(const float angularDrag);

	void SetLastFrameAcceleration(const XMVECTOR &newLastFrameAcceleration);

	void SetNewPosition(const XMVECTOR &newPosition);
	//void SetNewPosition(const float x, const float y, const float z);
	void SetRotation(const XMVECTOR &newRotation);
	//void SetRotation(const float x, const float y, const float z, const float w);
	void SetNewVelocity(const XMVECTOR &newVelocity);
	//void SetNewVelocity(const float x, const float y, const float z);

	void SetAngularVelocity(const XMVECTOR &angularVelocity);
	void SetInertiaTensor(const XMFLOAT3X3 &inertiaTensor);

	//Updates
	void UpdatePosition();
	void UpdateVelocity();

	bool HasFiniteMass() const;

	//We never accumulate torque or use it in our simulation
	void AddAccumulatedForce(const XMVECTOR &force);
	void AddAccumulatedTorque(const XMVECTOR &torque);
	void ClearAccumulators();

	//We don't use this unless we're using springs
	void AddForceAtLocalPoint(const XMFLOAT3 &force, const XMFLOAT3 &point);

	//Updates our derived data (transform matrix) whenever a change is made to the transform or orientation
	//Might cut this out as it's pointless, just a checking mechanism.
	void CalculateDerivedData();

private:
	//XMFLOAT3 GetPointInWorldSpace(const XMFLOAT3 &point) const;
	//void AddForceAtPoint(const XMFLOAT3 &force, const XMFLOAT3 &point);

	//Calculate the transform matrix from the position and rotation
	void CalculateTransformMatrix(XMFLOAT4X4 &transformMatrix, const XMFLOAT3 &position, const XMFLOAT4 &rotation);
	
	//Convert the inverseInertiaTensor to world space
	void InertiaTensorTransformLocalToWorld(XMFLOAT3X3 &inverseInertiaTensorInWorld, const XMFLOAT3X3 &inertiaTensorInLocal, const XMFLOAT4X4 &transformMatrix);

	bool m_isAwake;
	bool m_useGravity;
	float m_motion;
	float m_inverseMass;
	float m_drag;
	float m_angularDrag;

	XMVECTOR m_lastFrameAcceleration;

	XMVECTOR m_position;
	XMVECTOR m_newPosition;

	//"Orientation"
	XMVECTOR m_rotation;

	XMVECTOR m_velocity;
	XMVECTOR m_newVelocity;

	//"Rotation"
	XMVECTOR m_angularVelocity;

	XMVECTOR m_accumulatedForce;
	XMVECTOR m_accumulatedTorque;

	//In local space
	XMFLOAT3X3 m_inverseInertiaTensor;

	//In world space
	XMMATRIX m_inverseInertiaTensorInWorld;

	//Holds transform matrix for converting body space into world space and vice versa
	//Note: Just derived data that already exists and can be calculated and used as is with our transform position and orientation
	//Last row is just 0,0,0,1
	XMMATRIX m_transformMatrix;
};

