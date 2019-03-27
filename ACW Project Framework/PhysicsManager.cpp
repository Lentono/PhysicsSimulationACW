#include "PhysicsManager.h"

PhysicsManager::PhysicsManager(vector<GameObject*> &gameObjects) : m_gameObjects(gameObjects)
{
	XMFLOAT3 gravity(0.0f, -9.81f, 0.0f);
	m_gravity = XMLoadFloat3(&gravity);
}

PhysicsManager::~PhysicsManager() = default;

void PhysicsManager::CalculateGameObjectPhysics(const float dt)
{
	for (auto* gameObject : m_gameObjects)
	{
		if (gameObject->GetRigidBodyComponent())
		{
			//Improved Euler, all my other physics implementations are in the simulation loop project
			if (gameObject->GetRigidBodyComponent()->GetUseGravity())
			{
				auto* rigidBody = gameObject->GetRigidBodyComponent();

				//Return if the rigidbody is asleep, we only wake up if another object makes contact with it
				if (!rigidBody->GetIsAwake())
				{
					return;
				}

				const auto mass = rigidBody->GetMass();
				const auto inverseMass = rigidBody->GetInverseMass();
				auto velocity = XMVECTOR();
				auto position = XMVECTOR();
				auto rotation = XMVECTOR();
				auto angularVelocity = XMVECTOR();

				rigidBody->GetVelocity(velocity);
				rigidBody->GetPosition(position);
				rigidBody->GetRotation(rotation);
				rigidBody->GetAngularVelocity(angularVelocity);

				//XMStoreFloat3(&rigidBody->m_lastFrameAcceleration, XMVectorScale(XMLoadFloat3(&force), inverseMass));

				//Add gravity force to our rigidBody
				rigidBody->AddAccumulatedForce(m_gravity * mass);

				//Perform rigidBody integration to apply translation and rotational dynamics

				//Get our accumulated force and calculate our linear acceleration
				auto accumulatedForce = XMVECTOR();
				auto accumulatedToque = XMVECTOR();

				rigidBody->GetAccumulatedForce(accumulatedForce);
				rigidBody->GetAccumulatedTorque(accumulatedToque);

				auto linearAcceleration = accumulatedForce / mass;

				linearAcceleration = linearAcceleration + (accumulatedForce * inverseMass);

				rigidBody->SetLastFrameAcceleration(linearAcceleration);

				auto inverseInertiaTensorWorld = XMMATRIX();
				rigidBody->GetInverseInertiaTensorWorld(inverseInertiaTensorWorld);

				//Calculate angularAcceleration
				auto angularAcceleration = XMVECTOR();
				angularAcceleration = XMVector3Transform(accumulatedToque, inverseInertiaTensorWorld);

				//Update velocity and angular velocity
				auto newVelocity = velocity + (linearAcceleration * dt);

				auto newAngularVelocity = angularVelocity + (angularAcceleration * dt);

				//Add drag to our velocity and angular velocity
				const auto drag = rigidBody->GetDrag();
				const auto angularDrag = rigidBody->GetAngularDrag();

				newVelocity = newVelocity * powf(drag, dt);
				newAngularVelocity = newAngularVelocity * powf(angularDrag, dt);

				const auto newPosition = position + (((velocity + newVelocity) / 2) * dt);

				auto angularVelocityQuaternion = XMVECTOR();

				angularVelocityQuaternion = XMQuaternionRotationRollPitchYawFromVector(newAngularVelocity * dt);

				const auto newRotation = XMQuaternionMultiply(rotation, angularVelocityQuaternion);

				//Update position, velocity, rotation, angular velocity and calculate our new inverse inertia tensor in world space and clear any accumulated force.

				rigidBody->SetNewPosition(newPosition);
				rigidBody->SetNewVelocity(newVelocity);
				rigidBody->SetRotation(newRotation);
				rigidBody->SetAngularVelocity(newAngularVelocity);

				rigidBody->CalculateDerivedData();
				rigidBody->ClearAccumulators();

				//Check and see if the rigidbody needs to be put to sleep
				//auto currentMotion = 0.0f;

				//XMStoreFloat(&currentMotion, XMVectorAdd(XMVector3Dot(newVelocity, newVelocity), XMVector3Dot(newAngularVelocity, newAngularVelocity)));

				//const auto bias = pow(0.5f, dt);

				//rigidBody->SetMotion(bias*rigidBody->GetMotion() + (1 - bias) * currentMotion);

				//if (rigidBody->GetMotion() < rigidBody->GetSleepEpsilon() && XMVectorGetY(newPosition) < 10.0f) //0.1f is the sleep epsilon
				//{
				//	rigidBody->SetIsAwake(false);
				//}
				//else if (rigidBody->GetMotion() > 10 * rigidBody->GetSleepEpsilon())
				//{
				//	rigidBody->SetMotion(10 * rigidBody->GetSleepEpsilon());
				//}

				//Improved Euler, using Ian Millingtons Newton-Euler Implementation instead due to adding rotational dynamics for rigidbodies
				/*XMFLOAT3 acceleration = force / mass;


				XMFLOAT3 newVelocity = velocity + (acceleration * dt);

				//gameObject->GetVelocityComponent()->SetNewVelocity(newVelocity);
				//gameObject->GetPositionComponent()->SetNewPosition(position + (((velocity + newVelocity) / 2) * dt));

				gameObject->GetrigidBodyComponent()->SetNewVelocity(newVelocity);
				gameObject->GetrigidBodyComponent()->SetNewPosition(position + (((velocity + newVelocity) / 2) * dt));*/
			}
		}

	}
}

void PhysicsManager::UpdateGameObjectPhysics()
{
	for (auto* gameObject : m_gameObjects)
	{
		//Improved Euler, all my other physics implementations are in the simulation loop
		if (gameObject->GetRigidBodyComponent()->GetUseGravity())
		{
			//gameObject->GetVelocityComponent()->UpdateVelocity();
			//gameObject->GetPositionComponent()->UpdatePosition();
			gameObject->GetRigidBodyComponent()->UpdateVelocity();
			gameObject->GetRigidBodyComponent()->UpdatePosition();
		}

	}
}
