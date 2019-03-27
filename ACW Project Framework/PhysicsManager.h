#pragma once

#include <vector>
#include "GameObject.h"
#include "XMFLOAT3Maths.h"

using namespace std;

class PhysicsManager
{
public:
	PhysicsManager(vector<GameObject*> &gameObjects);
	~PhysicsManager();

	void CalculateGameObjectPhysics(const float dt);
	void UpdateGameObjectPhysics();

private:
	XMVECTOR m_gravity;

	vector<GameObject*> &m_gameObjects;
};

