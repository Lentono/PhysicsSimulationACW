#include "CollisionManager.h"


CollisionManager::CollisionManager(vector<GameObject*> &gameObjects, float friction, float restitution) : m_randomTexture(false), m_friction(friction), m_restitution(restitution), m_gameObjects(gameObjects), m_contactManifold(new ContactManifold())
{
	//functionMap.insert(tuple<type_info(Collider*), type_info(Collider*)>(make_tuple(typeid(SphereCollider*), typeid(SphereCollider*))));

	m_functionMap[make_pair(Collider::ColliderType::Sphere, Collider::ColliderType::Sphere)] = &CollisionManager::SphereOnSphereDetection;
	m_functionMap[make_pair(Collider::ColliderType::Cylinder, Collider::ColliderType::Sphere)] = &CollisionManager::CylinderOnSphereDetection;
	m_functionMap[make_pair(Collider::ColliderType::Sphere, Collider::ColliderType::Cylinder)] = &CollisionManager::SphereOnCylinderDetection;
	m_functionMap[make_pair(Collider::ColliderType::Plane, Collider::ColliderType::Sphere)] = &CollisionManager::PlaneOnSphereDetection;
	m_functionMap[make_pair(Collider::ColliderType::Sphere, Collider::ColliderType::Plane)] = &CollisionManager::SphereOnPlaneDetection;
	m_functionMap[make_pair(Collider::ColliderType::AABBCube, Collider::ColliderType::Sphere)] = &CollisionManager::AABBOnSphereDetection;
	m_functionMap[make_pair(Collider::ColliderType::Sphere, Collider::ColliderType::AABBCube)] = &CollisionManager::SphereOnAABBDetection;
	m_functionMap[make_pair(Collider::ColliderType::OBBCube, Collider::ColliderType::Sphere)] = &CollisionManager::OBBOnSphereDetection;
	m_functionMap[make_pair(Collider::ColliderType::Sphere, Collider::ColliderType::OBBCube)] = &CollisionManager::SphereOnOBBDetection;
	m_functionMap[make_pair(Collider::ColliderType::OBBCube, Collider::ColliderType::Cylinder)] = &CollisionManager::OBBOnCylinderDetection;
	m_functionMap[make_pair(Collider::ColliderType::Cylinder, Collider::ColliderType::OBBCube)] = &CollisionManager::CylinderOnOBBDetection;

	//Not implemented, we just treat one of the OBBs as a sphere and redirect it to a different function
	m_functionMap[make_pair(Collider::ColliderType::OBBCube, Collider::ColliderType::Plane)] = &CollisionManager::OBBOnPlaneDetection;
	m_functionMap[make_pair(Collider::ColliderType::Plane, Collider::ColliderType::OBBCube)] = &CollisionManager::PlaneOnOBBDetection;
	m_functionMap[make_pair(Collider::ColliderType::OBBCube, Collider::ColliderType::OBBCube)] = &CollisionManager::OBBOnOBBDetection;
}


CollisionManager::~CollisionManager()
{
	if (m_contactManifold)
	{
		delete m_contactManifold;
		m_contactManifold = nullptr;
	}
}

void CollisionManager::SetFriction(const float friction)
{
	m_friction = friction;
}

void CollisionManager::SetRestitution(const float restitution)
{
	m_restitution = restitution;
}

void CollisionManager::ToggleRandomTexture() {
	m_randomTexture = !m_randomTexture;
}

void CollisionManager::DynamicCollisionDetection() {
	m_contactManifold->Clear();

	for (unsigned int i = 0; i < m_gameObjects.size(); i++)
	{
		for (unsigned int j = i + 1; j < m_gameObjects.size(); j++)
		{
			const auto functionPointer = m_functionMap[make_pair(m_gameObjects[i]->GetColliderComponent()->GetCollider(), m_gameObjects[j]->GetColliderComponent()->GetCollider())];

			if (!functionPointer)
			{
				continue;
			}

			(this->*functionPointer)(m_gameObjects[i], m_gameObjects[j]);
		}
	}
}

ContactManifold* CollisionManager::GetContactManifoldReference() const
{
	return m_contactManifold;
}

void CollisionManager::SphereOnSphereDetection(GameObject* gameObjectOne, GameObject* gameObjectTwo) {
	auto sphereOnePosition = XMVECTOR();
	auto sphereTwoPosition = XMVECTOR();
	auto sphereOneScale = XMVECTOR();
	auto sphereTwoScale = XMVECTOR();

	gameObjectOne->GetRigidBodyComponent()->GetNewPosition(sphereOnePosition);
	gameObjectTwo->GetRigidBodyComponent()->GetNewPosition(sphereTwoPosition);
	gameObjectOne->GetScale(sphereOneScale);
	gameObjectTwo->GetScale(sphereTwoScale);

	const auto distance = sphereOnePosition - sphereTwoPosition;

	auto size = 0.0f;

	XMStoreFloat(&size, XMVector3Length(distance));

	const auto radiusSum = XMVectorGetX(sphereOneScale) + XMVectorGetX(sphereTwoScale);

	//See if the squared distance is less than the squared radius sum
	if (size * size <= radiusSum * radiusSum)
	{
		//Generate contact (distance can be used as our midline)
		const auto normal = distance * (1.0f / size);

		const XMVECTOR contactPoint = sphereOnePosition + distance * 0.5f;

		ManifoldPoint contact = ManifoldPoint();
		contact.contactID[0] = gameObjectOne->GetRigidBodyComponent();
		contact.contactID[1] = gameObjectTwo->GetRigidBodyComponent();
		contact.contactNormal = normal;
		contact.contactPoint = contactPoint;
		contact.penetrationDepth = radiusSum - size;
		contact.friction = m_friction;
		contact.restitution = m_restitution;

		m_contactManifold->Add(contact);

		if (m_randomTexture)
		{
			gameObjectOne->ChangeRandomTexture();
			gameObjectTwo->ChangeRandomTexture();
		}
	}
}

void CollisionManager::CylinderOnSphereDetection(GameObject* gameObjectOne, GameObject* gameObjectTwo)
{
	SphereOnCylinderDetection(gameObjectTwo, gameObjectOne);
}

void CollisionManager::SphereOnCylinderDetection(GameObject* gameObjectOne, GameObject* gameObjectTwo)
{
	auto spherePosition = XMVECTOR();
	auto cylinderPosition = XMVECTOR();
	auto sphereScale = XMVECTOR();
	auto cylinderScale = XMVECTOR();

	gameObjectOne->GetScale(sphereScale);
	gameObjectTwo->GetScale(cylinderScale);

	gameObjectOne->GetRigidBodyComponent()->GetNewPosition(spherePosition);
	gameObjectTwo->GetRigidBodyComponent()->GetPosition(cylinderPosition);

	//Lock cylinder z position to the spheres z position, this way we treat it as a 2D sphere collision
	XMVECTOR tempCylinderPosition = cylinderPosition;
	tempCylinderPosition = XMVectorSetZ(tempCylinderPosition, XMVectorGetZ(spherePosition));

	const auto distance = spherePosition - tempCylinderPosition;

	auto size = 0.0f;

	XMStoreFloat(&size, XMVector3Length(distance));

	const auto radiusSum = XMVectorGetX(sphereScale) + XMVectorGetX(cylinderScale);

	//See if the squared distance is less than the squared radius sum
	if (size * size <= radiusSum * radiusSum)
	{
		//Generate contact (distance can be used as our midline)
		const auto normal = distance * (1.0f / size);

		const XMVECTOR contactPoint = spherePosition + distance * 0.5f;

		ManifoldPoint contact = ManifoldPoint();
		contact.contactID[0] = gameObjectOne->GetRigidBodyComponent();
		contact.contactID[1] = nullptr;
		contact.contactNormal = normal;
		contact.contactPoint = contactPoint;
		contact.penetrationDepth = radiusSum - size;
		contact.friction = m_friction;
		contact.restitution = m_restitution;

		m_contactManifold->Add(contact);
	}
}

//Just switch them around and redirect to the other function
void CollisionManager::PlaneOnSphereDetection(GameObject* gameObjectOne, GameObject* gameObjectTwo) {
	SphereOnPlaneDetection(gameObjectTwo, gameObjectOne);
}

void CollisionManager::SphereOnPlaneDetection(GameObject* gameObjectOne, GameObject* gameObjectTwo) {
	auto spherePosition = XMVECTOR();
	auto sphereScale = XMVECTOR();
	
	gameObjectOne->GetScale(sphereScale);

	gameObjectOne->GetRigidBodyComponent()->GetNewPosition(spherePosition);

	const auto* planeCollider = dynamic_cast<PlaneCollider*>(gameObjectTwo->GetColliderComponent());
	auto planeNormal = planeCollider->GetNormal();

	planeNormal = XMVectorSetY(planeNormal, -XMVectorGetY(planeNormal));

	auto sphereDistance = 0.0f;

	XMStoreFloat(&sphereDistance, XMVector3Dot(spherePosition, planeNormal));

	sphereDistance += XMVectorGetX(sphereScale);
	sphereDistance += planeCollider->GetOffset();

	if (sphereDistance <= 0.0f)
	{
		const XMVECTOR contactPoint = spherePosition - planeNormal * (sphereDistance + XMVectorGetX(sphereScale));

		ManifoldPoint contact = ManifoldPoint();
		contact.contactID[0] = gameObjectOne->GetRigidBodyComponent();
		contact.contactID[1] = nullptr;
		contact.contactNormal = planeNormal;
		contact.contactPoint = contactPoint;
		contact.penetrationDepth = -sphereDistance;
		contact.friction = m_friction;
		contact.restitution = m_restitution;

		m_contactManifold->Add(contact);
	}

}

void CollisionManager::AABBOnSphereDetection(GameObject* gameObjectOne, GameObject* gameObjectTwo)
{
	SphereOnAABBDetection(gameObjectTwo, gameObjectOne);
}

void CollisionManager::SphereOnAABBDetection(GameObject* gameObjectOne, GameObject* gameObjectTwo)
{
	//Need to get the closest point on our ABB to test against our sphere

	auto spherePosition = XMVECTOR();
	auto sphereScale = XMVECTOR();

	auto cubePosition = XMVECTOR(); 
	auto cubeScale = XMVECTOR(); 

	gameObjectOne->GetRigidBodyComponent()->GetNewPosition(spherePosition);
	gameObjectTwo->GetRigidBodyComponent()->GetPosition(cubePosition);
	gameObjectOne->GetScale(sphereScale);
	gameObjectTwo->GetScale(cubeScale);

	auto closestPoint = XMVECTOR();

	const auto cubePositionX = XMVectorGetX(cubePosition);
	const auto cubePositionY = XMVectorGetY(cubePosition);
	const auto cubePositionZ = XMVectorGetZ(cubePosition);

	const auto cubeScaleX = XMVectorGetX(cubeScale);
	const auto cubeScaleY = XMVectorGetY(cubeScale);
	const auto cubeScaleZ = XMVectorGetZ(cubeScale);

	//Need to get the closest point on our ABB to test against our sphere
	closestPoint = XMVectorSetX(closestPoint, max(cubePositionX - cubeScaleX, min(XMVectorGetX(spherePosition), cubePositionX + cubeScaleX)));
	closestPoint = XMVectorSetY(closestPoint, max(cubePositionY - cubeScaleY, min(XMVectorGetY(spherePosition), cubePositionY + cubeScaleY)));
	closestPoint = XMVectorSetZ(closestPoint, max(cubePositionZ - cubeScaleZ, min(XMVectorGetZ(spherePosition), cubePositionZ + cubeScaleZ)));

	const auto v = closestPoint - spherePosition;

	const auto vX = XMVectorGetX(v);
	const auto vY = XMVectorGetY(v);
	const auto vZ = XMVectorGetZ(v);

	const auto distanceSqrt = sqrt(vX * vX + vY * vY + vZ * vZ);

	if (distanceSqrt < XMVectorGetX(sphereScale))
	{
		ManifoldPoint contact = ManifoldPoint();
		contact.contactID[0] = gameObjectOne->GetRigidBodyComponent();
		contact.contactID[1] = nullptr;

		auto contactNormal = spherePosition - closestPoint;

		contactNormal = XMVector3Normalize(contactNormal);

		contact.contactNormal = contactNormal;
		contact.contactPoint = closestPoint;
		//This may be wrong
		contact.penetrationDepth = XMVectorGetX(sphereScale) - distanceSqrt;
		contact.friction = m_friction;
		contact.restitution = m_restitution;

		m_contactManifold->Add(contact);
	}

	//This is just an ABB vs ABB collision test, oops
	//const auto spherePosition = gameObjectOne->GetRigidBodyComponent()->GetPosition();
	//const auto sphereOneRadius = gameObjectOne->GetScale().x;

	//const auto cubePosition = gameObjectTwo->GetRigidBodyComponent()->GetPosition();
	//const auto cubeScale = gameObjectTwo->GetScale();

	//if (abs(spherePosition.x - cubePosition.x) > (sphereOneRadius + cubeScale.x))
	//{
	//	//No Collision
	//	return;
	//}

	//if (abs(spherePosition.y - cubePosition.y) > (sphereOneRadius + cubeScale.y))
	//{
	//	//No Collision
	//	return;
	//}

	//if (abs(spherePosition.z - cubePosition.z) > (sphereOneRadius + cubeScale.z))
	//{
	//	//No Collision
	//	return;
	//}

	////We have a collision
	//auto wehaveacollisionboys = 0.0f;
}

void CollisionManager::OBBOnSphereDetection(GameObject* gameObjectOne, GameObject* gameObjectTwo)
{
	SphereOnOBBDetection(gameObjectTwo, gameObjectOne);
}

void CollisionManager::SphereOnOBBDetection(GameObject* gameObjectOne, GameObject* gameObjectTwo)
{
	//Projects the spherePosition within the local space of our OBB cube to get the closest point to it and then back to world coordinates to
	//test the distance between the sphere and the closest point on our OBB

	auto spherePosition = XMVECTOR();
	auto sphereScale = XMVECTOR();

	auto cubePosition = XMVECTOR();
	auto cubeScale = XMVECTOR();
	auto cubeRotation = XMVECTOR();

	gameObjectOne->GetRigidBodyComponent()->GetNewPosition(spherePosition);
	gameObjectOne->GetScale(sphereScale);

	gameObjectTwo->GetRigidBodyComponent()->GetPosition(cubePosition);
	gameObjectTwo->GetScale(cubeScale);
	gameObjectTwo->GetRigidBodyComponent()->GetRotation(cubeRotation);

	//Transform the sphere position to the local space of the cube
	auto relativeSpherePosition = XMVECTOR();
		
	XMMATRIX cubeTransform = XMMatrixRotationQuaternion(cubeRotation);

	cubeTransform = XMMatrixMultiply(cubeTransform, XMMatrixTranslationFromVector(cubePosition));

	auto determinant = XMMatrixDeterminant(cubeTransform);

	relativeSpherePosition = XMVector3Transform(spherePosition, XMMatrixInverse(&determinant, cubeTransform));

	auto closestPoint = XMVECTOR();
	auto distance = XMVectorGetX(relativeSpherePosition);

	const auto cubeScaleX = XMVectorGetX(cubeScale);
	const auto cubeScaleY = XMVectorGetY(cubeScale);
	const auto cubeScaleZ = XMVectorGetZ(cubeScale);

	if (distance > cubeScaleX)
	{
		distance = cubeScaleX;
	}

	if (distance < -cubeScaleX)
	{
		distance = -cubeScaleX;
	}

	closestPoint = XMVectorSetX(closestPoint, distance);

	distance = XMVectorGetY(relativeSpherePosition);
	if (distance > cubeScaleY)
	{
		distance = cubeScaleY;
	}

	if (distance < -cubeScaleY)
	{
		distance = -cubeScaleY;
	}

	closestPoint = XMVectorSetY(closestPoint, distance);

	distance = XMVectorGetZ(relativeSpherePosition);

	if (distance > cubeScaleZ)
	{
		distance = cubeScaleZ;
	}

	if (distance < -cubeScaleZ)
	{
		distance = -cubeScaleZ;
	}

	closestPoint = XMVectorSetZ(closestPoint, distance);

	const auto v = closestPoint - relativeSpherePosition;

	const auto vX = XMVectorGetX(v);
	const auto vY = XMVectorGetY(v);
	const auto vZ = XMVectorGetZ(v);

	const auto distanceSqrt = sqrt(vX * vX + vY * vY + vZ * vZ);

	if (distanceSqrt < XMVectorGetX(sphereScale))
	{
		auto closestPointWorld = XMVECTOR();

		closestPointWorld = XMVector3Transform(closestPoint, cubeTransform);

		ManifoldPoint contact = ManifoldPoint();
		contact.contactID[0] = gameObjectOne->GetRigidBodyComponent();
		contact.contactID[1] = nullptr;

		if (!gameObjectOne->GetRigidBodyComponent()->GetUseGravity())
		{
			return;
		}

		if (gameObjectTwo->GetRigidBodyComponent()->GetUseGravity())
		{
			contact.contactID[1] = gameObjectTwo->GetRigidBodyComponent();
		}

		contact.contactID[1] = nullptr;

		auto contactNormal = spherePosition - closestPointWorld;

		//contactNormal = { -contactNormal.x, -contactNormal.y, -contactNormal.z };

		contactNormal = XMVector3Normalize(contactNormal);

		contact.contactNormal = contactNormal;
		contact.contactPoint = closestPointWorld;
		//This may be wrong
		contact.penetrationDepth = XMVectorGetX(sphereScale) - distanceSqrt;
		contact.friction = m_friction;
		contact.restitution = m_restitution;

		m_contactManifold->Add(contact);
	}

	//auto wehaveacollisionboys = 0.0f;

	//XMVector3Transform(XMLoadFloat3(&spherePosition), inverse)

	////XMFLOAT4X4 rotationMatrix = {};

	////XMFLOAT3 axisVectors[3];

	////XMStoreFloat4x4(&rotationMatrix, XMMatrixRotationQuaternion(XMLoadFloat4(&cubeRotation)));

	//XMVECTOR axis = {};
	//float angle = 0.0f;

	//XMQuaternionToAxisAngle(&axis, &angle, XMLoadFloat4(&cubeRotation));

	//XMFLOAT3 axisVector = {};

	//XMStoreFloat3(&axisVector, axis);

	//XMStoreFloat3(&axisVector, XMVector3Normalize(XMVectorScale(XMLoadFloat3(&axisVector), angle)));

	//XMFLOAT3 axisVectorArray[3] = { XMFLOAT3(axisVector.x, 0.0f, 0.0f), XMFLOAT3(0.0f, axisVector.y, 0.0f), XMFLOAT3(0.0f, 0.0f, axisVector.z)};

	//float axisExtends[3] = { cubeScale.x, cubeScale.y, cubeScale.z };

	//XMFLOAT3 d = spherePosition - cubePosition;

	//XMFLOAT3 q = cubePosition;

	//for (unsigned int i = 0; i < 3; i++)
	//{
	//	float dist;

	//	XMStoreFloat(&dist, XMVector3Dot(XMLoadFloat3(&d), XMLoadFloat3(&axisVectorArray[i])));

	//	if (dist > axisExtends[i])
	//	{
	//		dist = axisExtends[i];
	//	}

	//	if (dist < -axisExtends[i])
	//	{
	//		dist = -axisExtends[i];
	//	}

	//	q = q + dist * axisVectorArray[i];
	//}

	//XMFLOAT3 v = q - spherePosition;

	//float squaredDistance;

	//XMStoreFloat(&squaredDistance, XMVector3Dot(XMLoadFloat3(&v), XMLoadFloat3(&v)));

	//if (squaredDistance <= sphereScale.x * sphereScale.x)
	//{
	//	auto wehaveacollisionboys = 0.0f;
	//}
}

void CollisionManager::OBBOnCylinderDetection(GameObject* gameObjectOne, GameObject* gameObjectTwo)
{
	CylinderOnOBBDetection(gameObjectTwo, gameObjectOne);
}

void CollisionManager::CylinderOnOBBDetection(GameObject* gameObjectOne, GameObject* gameObjectTwo)
{
	if (!gameObjectTwo->GetRigidBodyComponent()->GetUseGravity())
	{
		return;
	}

	//Projects the cylinder within the local space of our OBB cube to get the closest point to it and then back to world coordinates to
	//test the distance between the sphere and the closest point on our OBB

	auto cylinderPosition = XMVECTOR();
	auto cylinderScale = XMVECTOR();

	auto cubePosition = XMVECTOR();
	auto cubeScale = XMVECTOR();
	auto cubeRotation = XMVECTOR();

	gameObjectOne->GetRigidBodyComponent()->GetPosition(cylinderPosition);
	gameObjectOne->GetScale(cylinderScale);

	gameObjectTwo->GetRigidBodyComponent()->GetNewPosition(cubePosition);
	gameObjectTwo->GetScale(cubeScale);
	gameObjectTwo->GetRigidBodyComponent()->GetRotation(cubeRotation);

	//Lock the cylinder to the z axis of our cube as we're only doing a 2D sphere collision really
	XMVECTOR tempCylinderPosition = cylinderPosition;
	tempCylinderPosition = XMVectorSetZ(tempCylinderPosition, XMVectorGetZ(cubePosition));

	//Transform the cylinder position to the local space of the cube

	//XMMATRIX cubeTransform = {};

	//XMMATRIX cubeTransform = XMMatrixScaling(cubeScale.x, cubeScale.y, cubeScale.z);

	XMMATRIX cubeTransform = XMMatrixRotationQuaternion(cubeRotation);

	//cubeTransform = XMMatrixMultiply(cubeTransform, XMMatrixRotationQuaternion(XMLoadFloat4(&cubeRotation)));

	cubeTransform = XMMatrixMultiply(cubeTransform, XMMatrixTranslationFromVector(cubePosition));

	auto determinant = XMMatrixDeterminant(cubeTransform);

	auto relativeCylinderPosition = XMVECTOR();

	relativeCylinderPosition = XMVector3Transform(tempCylinderPosition, XMMatrixInverse(&determinant, cubeTransform));

	auto closestPoint = XMVECTOR();
	auto distance = XMVectorGetX(relativeCylinderPosition);

	const auto cubeScaleX = XMVectorGetX(cubeScale);
	const auto cubeScaleY = XMVectorGetY(cubeScale);
	const auto cubeScaleZ = XMVectorGetZ(cubeScale);

	if (distance > cubeScaleX)
	{
		distance = cubeScaleX;
	}

	if (distance < -cubeScaleX)
	{
		distance = -cubeScaleX;
	}
	closestPoint = XMVectorSetX(closestPoint, distance);

	distance = XMVectorGetY(relativeCylinderPosition);
	if (distance > cubeScaleY)
	{
		distance = cubeScaleY;
	}

	if (distance < -cubeScaleY)
	{
		distance = -cubeScaleY;
	}

	closestPoint = XMVectorSetY(closestPoint, distance);

	distance = XMVectorGetZ(relativeCylinderPosition);

	if (distance > cubeScaleZ)
	{
		distance = cubeScaleZ;
	}

	if (distance < -cubeScaleZ)
	{
		distance = -cubeScaleZ;
	}

	closestPoint = XMVectorSetZ(closestPoint, distance);

	const auto v = closestPoint - relativeCylinderPosition;

	const auto vX = XMVectorGetX(v);
	const auto vY = XMVectorGetY(v);
	const auto vZ = XMVectorGetZ(v);

	const auto distanceSqrt = sqrt(vX * vX + vY * vY + vZ * vZ);

	if (distanceSqrt < XMVectorGetX(cylinderScale))
	{
		auto closestPointWorld = XMVECTOR();

		closestPointWorld = XMVector3Transform(closestPoint, cubeTransform);

		ManifoldPoint contact = ManifoldPoint();
		contact.contactID[0] = gameObjectTwo->GetRigidBodyComponent();
		contact.contactID[1] = nullptr;

		auto contactNormal = tempCylinderPosition + closestPointWorld;

		//contactNormal = { -contactNormal.x, -contactNormal.y, -contactNormal.z };

		contactNormal = XMVector3Normalize(contactNormal);

		contact.contactNormal = contactNormal;
		contact.contactPoint = closestPointWorld;
		//This may be wrong
		contact.penetrationDepth = XMVectorGetX(cylinderScale) - distanceSqrt;
		contact.friction = m_friction;
		contact.restitution = m_restitution;

		m_contactManifold->Add(contact);
	}
}

//Not implemented, so we'll just treat it as a sphere for now
void CollisionManager::OBBOnPlaneDetection(GameObject* gameObjectOne, GameObject* gameObjectTwo)
{
	SphereOnPlaneDetection(gameObjectOne, gameObjectTwo);
}

//Not implemented, so we'll just treat it as a sphere for now
void CollisionManager::PlaneOnOBBDetection(GameObject* gameObjectOne, GameObject* gameObjectTwo)
{
	SphereOnPlaneDetection(gameObjectTwo, gameObjectOne);
}

//Not implemented, so we'll just treat one of the OBBs as a sphere for now
void CollisionManager::OBBOnOBBDetection(GameObject* gameObjectOne, GameObject* gameObjectTwo)
{
	SphereOnOBBDetection(gameObjectOne, gameObjectTwo);
}
