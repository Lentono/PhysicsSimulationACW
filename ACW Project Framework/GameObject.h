#pragma once

#include "Model.h"
#include "Texture.h"
#include "Position.h"
#include "Rotation.h"
#include "Scale.h"
#include "RigidBody.h"
#include "Shader.h"
#include "Velocity.h"
#include "Collider.h"

class GameObject
{
public:
	GameObject(HWND hwnd); // Default Constructor (Empty GameObject)
	//GameObject(ID3D11Device* device, const ModelType modelType, ResourceManager* resourceManager); //GameObject with model
	//GameObject(ID3D11Device* device, const ModelType modelType, const WCHAR* textureFileName, ResourceManager* resourceManager); //GameObject with model/ texture
	GameObject(const GameObject& other); // Copy Constructor
	GameObject(GameObject&& other) noexcept; // Move Constructor
	~GameObject(); // Destructor

	GameObject& operator = (const GameObject& other); // Copy Assignment Operator
	GameObject& operator = (GameObject&& other) noexcept; // Move Assignment Operator

	void AddPositionComponent();
	void AddPositionComponent(const XMFLOAT3 position);
	void AddPositionComponent(const float x, const float y, const float z);

	void AddRotationComponent();
	void AddRotationComponent(const XMFLOAT4 rotation);
	void AddRotationComponent(const float x, const float y, const float z, const float w);

	void AddScaleComponent();
	void AddScaleComponent(const XMFLOAT3 scale);
	void AddScaleComponent(const float x, const float y, const float z);

	void AddVelocityComponent();
	void AddVelocityComponent(const XMFLOAT3 velocity);
	void AddVelocityComponent(const float x, const float y, const float z);

	void AddRigidBodyComponent(const bool useGravity, const float mass, const float drag, const float angularDrag, const XMFLOAT3 position, const XMFLOAT4 rotation, const XMFLOAT3 velocity, const XMFLOAT3 angularVelocity);
	void AddColliderComponent(const Collider::ColliderType colliderType);
	void SetPlaneColliderData(const XMFLOAT3& centre, const XMFLOAT3& pointOne, const XMFLOAT3& pointTwo, const float offset) const;

	void AddModelComponent(ID3D11Device* device, const Model::ModelType modelType, ResourceManager* resourceManager);
	void AddTextureComponent(ID3D11Device* device, const WCHAR* textureFileName, ResourceManager* resourceManager);
	void AddShaderComponent(Shader* shader);

	Position* GetPositionComponent() const;
	XMFLOAT3 GetPosition() const;

	Rotation* GetRotationComponent() const;
	XMFLOAT4 GetRotation() const;

	Scale* GetScaleComponent() const;
	void GetScale(XMVECTOR &scale) const;

	Velocity* GetVelocityComponent() const;
	XMFLOAT3 GetVelocity() const;

	RigidBody* GetRigidBodyComponent() const;
	Collider* GetColliderComponent() const;

	int GetIndexCount() const;
	ID3D11ShaderResourceView* GetTexture() const;

	bool GetInitializationState() const;

	bool Render(ID3D11DeviceContext* deviceContext, XMMATRIX &worldMatrix, XMMATRIX &viewMatrix, XMMATRIX &projectionMatrix, XMFLOAT4 diffuseLight, XMFLOAT3 lightDirection);

	void ChangeRandomTexture();

private:

	bool m_initializationFailed;

	HWND m_hwnd;

	Position* m_position;
	Rotation* m_rotation;
	Scale* m_scale;
	Velocity* m_velocity;

	RigidBody* m_rigidBody;
	Collider* m_collider;

	Model* m_model;
	Texture* m_texture;

	Shader* m_shader;
};

