#pragma once
#include "ContactManifold.h"

#include <algorithm>
#include <cmath>

using namespace std;

class CollisionManager
{

#include <typeinfo>

public:
	CollisionManager(vector<GameObject*> &gameObjects, float friction, float restitution);
	~CollisionManager();

	void SetFriction(const float friction);
	void SetRestitution(const float restitution);

	void ToggleRandomTexture();

	void DynamicCollisionDetection();

	ContactManifold* GetContactManifoldReference() const;

	typedef void (CollisionManager::*CollisionFunction)(GameObject*, GameObject*);

private:
	//Sphere Collision Detection
	void SphereOnSphereDetection(GameObject* gameObjectOne, GameObject* gameObjectTwo);

	//Cylinder Collision Detection
	void CylinderOnSphereDetection(GameObject* gameObjectOne, GameObject* gameObjectTwo);
	void SphereOnCylinderDetection(GameObject* gameObjectOne, GameObject* gameObjectTwo);

	//Half-Space Sphere Collision Detection
	void PlaneOnSphereDetection(GameObject* gameObjectOne, GameObject* gameObjectTwo);
	void SphereOnPlaneDetection(GameObject* gameObjectOne, GameObject* gameObjectTwo);

	//ABB Sphere Collision Detection
	void AABBOnSphereDetection(GameObject* gameObjectOne, GameObject* gameObjectTwo);
	void SphereOnAABBDetection(GameObject* gameObjectOne, GameObject* gameObjectTwo);

	//OBB Sphere Collision Detection
	void OBBOnSphereDetection(GameObject* gameObjectOne, GameObject* gameObjectTwo);
	void SphereOnOBBDetection(GameObject* gameObjectOne, GameObject* gameObjectTwo);

	//OBB Cylinder Collision Detection
	void OBBOnCylinderDetection(GameObject* gameObjectOne, GameObject* gameObjectTwo);
	void CylinderOnOBBDetection(GameObject* gameObjectOne, GameObject* gameObjectTwo);

	//OBB Plane Collision Detection (Not Implemented Properly)
	void OBBOnPlaneDetection(GameObject* gameObjectOne, GameObject* gameObjectTwo);
	void PlaneOnOBBDetection(GameObject* gameObjectOne, GameObject* gameObjectTwo);

	//OBB OBB Collision Detection (Not Implemented Properly)
	void OBBOnOBBDetection(GameObject* gameObjectOne, GameObject* gameObjectTwo);

	bool m_randomTexture;

	float m_friction;
	float m_restitution;

	vector<GameObject*> &m_gameObjects;
	ContactManifold* m_contactManifold;

	map<pair<Collider::ColliderType, Collider::ColliderType>, CollisionFunction> m_functionMap;
};

