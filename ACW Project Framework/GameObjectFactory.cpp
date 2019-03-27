#include "GameObjectFactory.h"

GameObjectFactory::GameObjectFactory(vector<GameObject*> &gameObjects) : m_gameObjects(gameObjects)
{
}

GameObjectFactory::~GameObjectFactory() = default;

bool GameObjectFactory::AddGameObject(const HWND hwnd, ID3D11Device* device, const XMFLOAT3& position, const XMFLOAT3& rotation, const XMFLOAT3& scale, const XMFLOAT3& velocity, const XMFLOAT3& angularVelocity, const Collider::ColliderType& colliderType, const Model::ModelType& modelType, const bool& useGravity, const float& mass, const float& drag, const float& angularDrag, Shader* shader, const WCHAR* textureFileName, ResourceManager* resourceManager)
{
	auto quaternionRotation = XMFLOAT4();

	XMStoreFloat4(&quaternionRotation, XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z));

	m_gameObjects.push_back(new GameObject(hwnd));

	if (m_gameObjects.back()->GetInitializationState())
	{
		return true;
	}

	m_gameObjects.back()->AddScaleComponent(scale);
	m_gameObjects.back()->AddColliderComponent(colliderType);
	m_gameObjects.back()->AddModelComponent(device, modelType, resourceManager);
	m_gameObjects.back()->AddRigidBodyComponent(useGravity, mass, drag, angularDrag, position, quaternionRotation, velocity, angularVelocity);
	m_gameObjects.back()->AddTextureComponent(device, textureFileName, resourceManager);
	m_gameObjects.back()->AddShaderComponent(shader);

	return false;
}
