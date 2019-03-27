#pragma once
#include "GameObject.h"
#include <vector>

class GameObjectFactory
{
public:
	GameObjectFactory(vector<GameObject*> &gameObjects);
	~GameObjectFactory();

	bool AddGameObject(const HWND hwnd, ID3D11Device* device, 
		const XMFLOAT3 &position, const XMFLOAT3 &rotation, const XMFLOAT3 &scale, const XMFLOAT3 &velocity, const XMFLOAT3 &angularVelocity,
		const Collider::ColliderType &colliderType, const Model::ModelType &modelType, 
		const bool &useGravity, const float &mass, const float &drag, const float &angularDrag,
		Shader* shader, const WCHAR* textureFileName, ResourceManager* resourceManager);

private:

	vector<GameObject*> &m_gameObjects;
};

