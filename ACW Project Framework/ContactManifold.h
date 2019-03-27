#pragma once

#include <DirectXMath.h>
#include <vector>

#include "GameObject.h"

using namespace DirectX;

struct ManifoldPoint
{
	RigidBody* contactID[2];

	XMVECTOR contactPoint;
	XMVECTOR contactNormal;

	float friction;
	float restitution;
	float penetrationDepth;

	XMMATRIX contactToWorld;
	XMVECTOR contactVelocity;
	float desiredDeltaVelocity;
	XMVECTOR relativeContactPosition[2];

	int testcount = 0;

	void MatchAwakeState()
	{
		//If the other contact is null then there's nothing to match
		if (!contactID[1])
		{
			return;
		}

		auto contactOneAwake = contactID[0]->GetIsAwake();
		auto contactTwoAwake = contactID[1]->GetIsAwake();

		//Match awake states
		if (contactOneAwake ^ contactTwoAwake)
		{
			//If one is awake then we'll wake up contact2
			if (contactOneAwake)
			{
				contactID[1]->SetIsAwake(true);
			}
			else
			{
				contactID[0]->SetIsAwake(true);
			}

			testcount++;
		}
	}

	void CalculateInternals(const float dt)
	{
		CalculateContactBasis();

		auto position = XMVECTOR();

		contactID[0]->GetPosition(position);

		relativeContactPosition[0] = contactPoint - position;

		if (contactID[1])
		{
			position = XMVECTOR();

			contactID[0]->GetPosition(position);

			relativeContactPosition[1] = contactPoint - position;
		}

		contactVelocity = CalculateLocalVelocity(0, dt);

		if (contactID[1])
		{
			contactVelocity = contactVelocity - CalculateLocalVelocity(1, dt);
		}

		CalculateDesiredDeltaVelocity(dt);
	}

	void CalculateContactBasis()
	{
		XMFLOAT3 contactTangent[2];

		const auto contactNormalX = XMVectorGetX(contactNormal);
		const auto contactNormalY = XMVectorGetY(contactNormal);
		const auto contactNormalZ = XMVectorGetZ(contactNormal);

		if (abs(contactNormalX) > abs(contactNormalY))
		{
			//Scale factor to normalize results
			const auto scalar = 1.0f / sqrt(contactNormalZ * contactNormalZ + contactNormalX * contactNormalX);

			contactTangent[0].x = contactNormalZ * scalar;
			contactTangent[0].y = 0;
			contactTangent[0].z = -contactNormalX * scalar;

			contactTangent[1].x = contactNormalY * contactTangent[0].x;
			contactTangent[1].y = contactNormalZ * contactTangent[0].x - contactNormalX * contactTangent[0].z;
			contactTangent[1].z = -contactNormalY * contactTangent[0].x;
		}
		else
		{
			//Scale factor to normalize results
			const auto scalar = 1.0f / sqrt(contactNormalZ * contactNormalZ + contactNormalY * contactNormalY);

			contactTangent[0].x = 0;
			contactTangent[0].y = -contactNormalZ * scalar;
			contactTangent[0].z = contactNormalY * scalar;

			contactTangent[1].x = contactNormalY * contactTangent[0].z - contactNormalZ * contactTangent[0].y;
			contactTangent[1].y = -contactNormalX * contactTangent[0].z;
			contactTangent[1].z = contactNormalX * contactTangent[0].y;
		}

		auto floatMatrix = XMFLOAT3X3(contactNormalX, contactNormalY, contactNormalZ,
			contactTangent[0].x, contactTangent[0].y, contactTangent[0].z,
			contactTangent[1].x, contactTangent[1].y, contactTangent[1].z);

		contactToWorld = XMLoadFloat3x3(&floatMatrix);
	}

	XMVECTOR CalculateLocalVelocity(const int index, const float dt)
	{
		RigidBody *rigidBody = contactID[index];

		//Work our contact velocity at the contact point with both gameObjects
		auto velocity = XMVECTOR();
		rigidBody->GetNewVelocity(velocity);

		auto angularVelocity = XMVECTOR();
		rigidBody->GetAngularVelocity(angularVelocity);

		velocity += XMVector3Cross(angularVelocity, relativeContactPosition[index]);

		auto contactVelocity = XMVECTOR();
		const auto contactToWorldTranspose = XMMatrixTranspose(contactToWorld);

		contactVelocity = XMVector3Transform(velocity, contactToWorldTranspose);

		auto lastFrameAcceleration = XMVECTOR();
		rigidBody->GetLastFrameAcceleration(lastFrameAcceleration);

		auto accumulatedVelocity = lastFrameAcceleration * dt;

		accumulatedVelocity = XMVector3Transform(accumulatedVelocity, contactToWorldTranspose);

		accumulatedVelocity = XMVectorSetX(accumulatedVelocity, 0.0f);

		contactVelocity = contactVelocity + accumulatedVelocity;

		return contactVelocity;
	}

	void CalculateDesiredDeltaVelocity(const float dt)
	{
		//Now calculate the desired change in velocity for our gameobjects (Is this needed?)
		const auto velocityLimit = 0.25f;

		auto velocityFromAcceleration = 0.0f;

		auto scaledAcceleration = XMVECTOR();

		//Might have to implement an awake later
		if (contactID[0]->GetIsAwake())
		{
			auto lastFrameAcceleration = XMVECTOR();
			contactID[0]->GetLastFrameAcceleration(lastFrameAcceleration);
			scaledAcceleration = XMVectorScale(lastFrameAcceleration, dt);
			XMStoreFloat(&velocityFromAcceleration, XMVector3Dot(scaledAcceleration, contactNormal));
		}

		if (contactID[1] && contactID[1]->GetIsAwake())
		{
			auto lastFrameAcceleration = XMVECTOR();
			contactID[1]->GetLastFrameAcceleration(lastFrameAcceleration);
			scaledAcceleration = XMVectorScale(lastFrameAcceleration, dt);
			XMStoreFloat(&velocityFromAcceleration, XMVectorSubtract(XMLoadFloat(&velocityFromAcceleration), XMVector3Dot(scaledAcceleration, contactNormal)));
		}

		//If our velocity is low then we don't use the resitution

		auto thisRestitution = restitution;

		const auto contactVelocityX = XMVectorGetX(contactVelocity);
		const auto contactVelocityY = XMVectorGetY(contactVelocity);
		const auto contactVelocityZ = XMVectorGetZ(contactVelocity);

		if (sqrt(contactVelocityX * contactVelocityX + contactVelocityY * contactVelocityY + contactVelocityZ * contactVelocityZ) < velocityLimit)
		{
			thisRestitution = 0.0f;
		}

		/*if (abs(contactVelocity.x) < velocityLimit)
		{
			thisRestitution = 0.0f;
		}*/

		desiredDeltaVelocity = -contactVelocityX -thisRestitution * (contactVelocityX - velocityFromAcceleration);
	}

	void ResolvePenetration(XMVECTOR linearChange[2], XMVECTOR angularChange[2], float maxPenetration)
	{
		//Resolve the penetration and apply the positional change
		//Does Ben use same value?
		const auto angularLimit = 0.2f;

		float linearMove[2];
		float angularMove[2];
		
		auto totalInertia = 0.0f;
		float linearInertia[2];
		float angularInertia[2];

		//Calculate the inertia of each object in the direction of the contact normal

		for (unsigned int i = 0; i < 2; i++) if (contactID[i])
		{
			auto angularInertiaWorld = XMVECTOR();
			auto inverseInertiaTensor = XMMATRIX();
			contactID[i]->GetInverseInertiaTensorWorld(inverseInertiaTensor);

			angularInertiaWorld = XMVector3Cross(relativeContactPosition[i], contactNormal);

			angularInertiaWorld = XMVector3Transform(angularInertiaWorld, inverseInertiaTensor);

			angularInertiaWorld = XMVector3Cross(angularInertiaWorld, relativeContactPosition[i]);

			XMStoreFloat(&angularInertia[i], XMVector3Dot(angularInertiaWorld, contactNormal));

			linearInertia[i] = contactID[i]->GetInverseMass();

			totalInertia += linearInertia[i] + angularInertia[i];
		}

		//Apply position change
		for (unsigned int i = 0; i < 2; i++) if (contactID[i])
		{
			const float sign = (i == 0) ? 1 : -1;

			angularMove[i] = sign * maxPenetration * (angularInertia[i] / totalInertia);
			linearMove[i] = sign * maxPenetration * (linearInertia[i] / totalInertia);

			auto projection = relativeContactPosition[i];

			auto scalar = 0.0f;

			XMStoreFloat(&scalar, XMVector3Dot(-projection, contactNormal));

			projection += XMVectorScale(contactNormal, scalar);

			auto projectionMagnitude = 0.0f;

			XMStoreFloat(&projectionMagnitude, XMVector3Length(projection));

			const auto maxMagnitude = angularLimit * projectionMagnitude;

			if (angularMove[i] < -maxMagnitude)
			{
				const auto totalMove = angularMove[i] + linearMove[i];
				angularMove[i] = -maxMagnitude;
				linearMove[i] = totalMove - angularMove[i];
			}
			else if (angularMove[i] > maxMagnitude)
			{
				const auto totalMove = angularMove[i] + linearMove[i];
				angularMove[i] = maxMagnitude;
				linearMove[i] = totalMove - angularMove[i];
			}

			//If there is no angular movement we don't need to rotate it so we clear the angular change
			if (angularMove[i] == 0)
			{
				angularChange[i] = XMVECTOR();
			}
			else
			{
				auto inverseInertiaTensor = XMMATRIX();
				contactID[i]->GetInverseInertiaTensorWorld(inverseInertiaTensor);

				auto angularDirection = XMVECTOR();

				angularDirection = XMVector3Cross(relativeContactPosition[i], contactNormal);

				auto angularDirectionTransform = XMVECTOR();

				angularDirectionTransform = XMVector3Transform(angularDirection, inverseInertiaTensor);

				angularChange[i] = XMVectorScale(angularDirectionTransform, angularMove[i] / angularInertia[i]);
			}

			//linearChange = contactNormal * linearMove;
			linearChange[i] = XMVectorScale(contactNormal, linearMove[i]);

			//Apply the changes to our game object
			auto newPosition = XMVECTOR();
			contactID[i]->GetNewPosition(newPosition);
			newPosition = XMVectorAdd(newPosition, linearChange[i]);
			contactID[i]->SetNewPosition(newPosition);

			auto newRotation = XMVECTOR();
			contactID[i]->GetRotation(newRotation);
			auto scaledAngularChange = XMVECTOR();
			auto angularChangeQuaternion = XMVECTOR();

			//Is this correct? Ian Millingtons Quaternion.addScaledVector...
			scaledAngularChange = XMVectorScale(angularChange[i], 1.0f);
			angularChangeQuaternion = XMQuaternionRotationRollPitchYawFromVector(scaledAngularChange);
			newRotation = XMQuaternionMultiply(newRotation, angularChangeQuaternion);

			contactID[i]->SetRotation(newRotation);

			//Updated derived data if it's awake
			if (!contactID[i]->GetIsAwake())
			{
				contactID[i]->CalculateDerivedData();
			}
		}
	}

	void ApplyVelocityChange(XMVECTOR velocityChange[2], XMVECTOR angularChange[2])
	{
		XMMATRIX inverseInertiaTensorMatrix[2];

		contactID[0]->GetInverseInertiaTensorWorld(inverseInertiaTensorMatrix[0]);

		if (contactID[1])
		{
			contactID[1]->GetInverseInertiaTensorWorld(inverseInertiaTensorMatrix[1]);
		}

		auto impulseContact = XMVECTOR();

		//Calculate impulse contact depending on friction, if friction is zero we do a frictionless impulse
		if (friction == 0.0f)
		{
			//Calculate frictionless impulse
			impulseContact = CalculateFrictionlessImpulse(inverseInertiaTensorMatrix);
		}
		else
		{
			//Calculate friction impulse
			impulseContact = CalculateFrictionImpulse(inverseInertiaTensorMatrix);
		}

		//Convert out impulse contact to world coordinates
		auto impulseWorld = XMVECTOR();
		impulseWorld = XMVector3Transform(impulseContact, contactToWorld);

		//Calculate new velocity and rotational change from the calculated impulse
		auto impulseTorqueWorld = XMVECTOR();

		impulseTorqueWorld = XMVector3Cross(relativeContactPosition[0], impulseWorld);

		//New angular velocity
		angularChange[0] = XMVector3Transform(impulseTorqueWorld, inverseInertiaTensorMatrix[0]);

		//New linear velocity
		velocityChange[0] = XMVECTOR();
		velocityChange[0] = XMVectorScale(impulseWorld, contactID[0]->GetInverseMass());

		auto velocity = XMVECTOR();
		auto angularVelocity = XMVECTOR();

		contactID[0]->GetNewVelocity(velocity);
		contactID[0]->GetAngularVelocity(angularVelocity);

		contactID[0]->SetNewVelocity(velocity + velocityChange[0]);
		contactID[0]->SetAngularVelocity(angularVelocity + angularChange[0]);

		if (contactID[1])
		{
			//Calculate new velocity and rotational change from the calculated impulse
			impulseTorqueWorld = XMVector3Cross(impulseWorld, relativeContactPosition[1]);

			//New angular velocity
			angularChange[1] = XMVector3Transform(impulseTorqueWorld, inverseInertiaTensorMatrix[1]);

			//New linear velocity
			velocityChange[1] = XMVECTOR();
			velocityChange[1] = XMVectorScale(impulseWorld, -contactID[1]->GetInverseMass());

			contactID[1]->GetNewVelocity(velocity);
			contactID[1]->GetAngularVelocity(angularVelocity);

			contactID[1]->SetNewVelocity(velocity + velocityChange[1]);
			contactID[1]->SetAngularVelocity(angularVelocity + angularChange[1]);
		}
	}
	
	XMVECTOR CalculateFrictionlessImpulse(const XMMATRIX inverseInertiaTensor[2])
	{
		//change in velocity in world space
		auto totalDeltaVelocityWorld = XMVECTOR();

		totalDeltaVelocityWorld = XMVector3Cross(relativeContactPosition[0], contactNormal);
		totalDeltaVelocityWorld = XMVector3Transform(totalDeltaVelocityWorld, inverseInertiaTensor[0]);
		totalDeltaVelocityWorld = XMVector3Cross(totalDeltaVelocityWorld, relativeContactPosition[0]);

		auto deltaVelocity = 0.0f;

		XMStoreFloat(&deltaVelocity, XMVector3Dot(totalDeltaVelocityWorld, contactNormal));

		deltaVelocity += contactID[0]->GetInverseMass();

		if (contactID[1])
		{
			totalDeltaVelocityWorld = XMVECTOR();

			totalDeltaVelocityWorld = XMVector3Cross(relativeContactPosition[1], contactNormal);
			totalDeltaVelocityWorld = XMVector3Transform(totalDeltaVelocityWorld, inverseInertiaTensor[1]);
			totalDeltaVelocityWorld = XMVector3Cross(totalDeltaVelocityWorld, relativeContactPosition[1]);

			XMStoreFloat(&deltaVelocity, XMVectorAdd(XMLoadFloat(&deltaVelocity), XMVector3Dot(totalDeltaVelocityWorld, contactNormal)));

			deltaVelocity += contactID[1]->GetInverseMass();
		}

		auto impulseContact = XMVECTOR();

		//Calculate size of impulse contact
		impulseContact = XMVectorSetX(impulseContact, desiredDeltaVelocity / deltaVelocity);
		impulseContact = XMVectorSetY(impulseContact, 0.0f);
		impulseContact = XMVectorSetZ(impulseContact, 0.0f);

		return impulseContact;
	}

	XMVECTOR CalculateFrictionImpulse(const XMMATRIX inverseInertiaTensor[2])
	{
		auto impulseContact = XMVECTOR();

		auto inverseMass = contactID[0]->GetInverseMass();

		auto tempRelativeContactPosition = XMFLOAT3();
		XMStoreFloat3(&tempRelativeContactPosition, relativeContactPosition[0]);

		//Set skew symmetric
		auto impulseTorque = XMFLOAT3X3(0.0f, -tempRelativeContactPosition.z, tempRelativeContactPosition.y,
			tempRelativeContactPosition.z, 0.0f, -tempRelativeContactPosition.x,
			-tempRelativeContactPosition.y, tempRelativeContactPosition.x, 0.0f);

		auto impulseTorqueMatrix = XMLoadFloat3x3(&impulseTorque);

		/*impulseTorque = { 0.0f, relativeContactPosition[0].z, -relativeContactPosition[0].y,
						-relativeContactPosition[0].z , 0.0f, relativeContactPosition[0].x,
						relativeContactPosition[0].y, -relativeContactPosition[0].x, 0.0f };*/

		auto totalDeltaVelocityWorld = impulseTorqueMatrix;

		totalDeltaVelocityWorld = XMMatrixMultiply(totalDeltaVelocityWorld, inverseInertiaTensor[0]);
		totalDeltaVelocityWorld = XMMatrixMultiply(totalDeltaVelocityWorld, impulseTorqueMatrix);
		totalDeltaVelocityWorld = XMMatrixMultiply(totalDeltaVelocityWorld, XMMatrixScaling(-1.0f, -1.0f, -1.0f));

		if (contactID[1])
		{
			XMStoreFloat3(&tempRelativeContactPosition, relativeContactPosition[1]);

			//Set skew symmetric
			impulseTorque = XMFLOAT3X3(0.0f, -tempRelativeContactPosition.z, tempRelativeContactPosition.y,
				tempRelativeContactPosition.z, 0.0f, -tempRelativeContactPosition.x,
				-tempRelativeContactPosition.y, tempRelativeContactPosition.x, 0.0f);

			impulseTorqueMatrix = XMLoadFloat3x3(&impulseTorque);

			/*impulseTorque = { 0.0f, relativeContactPosition[1].z, -relativeContactPosition[1].y,
						-relativeContactPosition[1].z , 0.0f, relativeContactPosition[1].x,
						relativeContactPosition[1].y, -relativeContactPosition[1].x, 0.0f };*/

			auto deltaVelocityWorld = impulseTorqueMatrix;

			deltaVelocityWorld = XMMatrixMultiply(deltaVelocityWorld, inverseInertiaTensor[1]);
			deltaVelocityWorld = XMMatrixMultiply(deltaVelocityWorld, impulseTorqueMatrix);
			deltaVelocityWorld = XMMatrixMultiply(deltaVelocityWorld, XMMatrixScaling(-1.0f, -1.0f, -1.0f));

			totalDeltaVelocityWorld += deltaVelocityWorld;

			//totalDeltaVelocityWorld = XMFLOAT3X3(totalDeltaVelocityWorld._11 + deltaVelocityWorld._11, totalDeltaVelocityWorld._12 + deltaVelocityWorld._12, totalDeltaVelocityWorld._13 + deltaVelocityWorld._13,
			//	totalDeltaVelocityWorld._21 + deltaVelocityWorld._21, totalDeltaVelocityWorld._22 + deltaVelocityWorld._22, totalDeltaVelocityWorld._23 + deltaVelocityWorld._23,
			//	totalDeltaVelocityWorld._31 + deltaVelocityWorld._31, totalDeltaVelocityWorld._32 + deltaVelocityWorld._32, totalDeltaVelocityWorld._33 + deltaVelocityWorld._33);

			inverseMass += contactID[1]->GetInverseMass();
		}

		auto deltaVelocity = XMMATRIX();

		deltaVelocity = XMMatrixTranspose(contactToWorld);
		/*deltaVelocity = XMMatrixMultiply(deltaVelocity, totalDeltaVelocityWorld);
		deltaVelocity = XMMatrixMultiply(deltaVelocity, contactToWorld);*/
		deltaVelocity = XMMatrixMultiply(totalDeltaVelocityWorld, deltaVelocity);
		deltaVelocity = XMMatrixMultiply(contactToWorld, deltaVelocity);

		auto inverseMassFloatMatrix = XMFLOAT3X3(inverseMass, 0.0f, 0.0f,
												0.0f, inverseMass, 0.0f,
												0.0f, 0.0f, inverseMass);

		const auto inverseMassMatrix = XMLoadFloat3x3(&inverseMassFloatMatrix);

		deltaVelocity += inverseMassMatrix;

		auto determinant = XMMatrixDeterminant(deltaVelocity);

		const auto impulseMatrix = XMMatrixInverse(&determinant, deltaVelocity);

		auto velocityKiller = XMVECTOR();
		velocityKiller = XMVectorSetX(velocityKiller, desiredDeltaVelocity);
		velocityKiller = XMVectorSetY(velocityKiller, -XMVectorGetY(contactVelocity));
		velocityKiller = XMVectorSetZ(velocityKiller, -XMVectorGetZ(contactVelocity));

		//Apply velocity killer to matrix to get the impulse contact
		impulseContact = XMVector3Transform(velocityKiller, impulseMatrix);

		auto impulseContactX = XMVectorGetX(impulseContact);
		auto impulseContactY = XMVectorGetY(impulseContact);
		auto impulseContactZ = XMVectorGetZ(impulseContact);

		//Calculate planar impulse
		const auto planarImpulse = sqrt(impulseContactY * impulseContactY + impulseContactZ * impulseContactZ);

		//If the results are great we need to reduce the impulse contact add friction
		if (planarImpulse > impulseContactX * friction)
		{
			impulseContactY /= planarImpulse;
			impulseContactZ /= planarImpulse;

			auto tempDeltaVelocity = XMFLOAT3X3();
			XMStoreFloat3x3(&tempDeltaVelocity, deltaVelocity);

			impulseContactX = tempDeltaVelocity._11 + tempDeltaVelocity._12 * friction * impulseContactY + tempDeltaVelocity._13 * friction * impulseContactZ;
			impulseContactX = desiredDeltaVelocity / impulseContactX;
			impulseContactY *= friction * impulseContactX;
			impulseContactZ *= friction * impulseContactX;

			impulseContact = XMVectorSetX(impulseContact, impulseContactX);
			impulseContact = XMVectorSetY(impulseContact, impulseContactY);
			impulseContact = XMVectorSetZ(impulseContact, impulseContactZ);
		}

		return impulseContact;
	}
};

class ContactManifold
{
public:


	ContactManifold();
	~ContactManifold();

	void Add(ManifoldPoint &point);
	void Clear();
	unsigned int GetNumberOfPoints() const;
	ManifoldPoint& GetPoint(int index);

private:
	vector<ManifoldPoint> m_points;
	unsigned int m_numberOfPoints;
};

