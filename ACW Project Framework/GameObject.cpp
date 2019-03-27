#include "GameObject.h"
#include "TextureShader.h"

//For adding default components or making it empty (defaults components: Position, Rotation, Scale)
GameObject::GameObject(HWND hwnd) : m_initializationFailed(false), m_hwnd(hwnd), m_position(nullptr), m_rotation(nullptr), m_scale(nullptr), m_velocity(nullptr), m_rigidBody(nullptr), m_collider(nullptr), m_model(nullptr), m_texture(nullptr), m_shader(nullptr)
{
	//Empty GameObject with no components
	//Position* m_position;
	//Rotation* m_rotation;
	//Scale* m_scale;
	//Velocity* m_velocity;

	//RigidBody* m_rigidBody;
	//Collider* m_collider;

	//Model* m_model;
	//Texture* m_texture;

	//Shader* m_shader;
}

GameObject::GameObject(const GameObject& other) = default;

GameObject::GameObject(GameObject&& other) noexcept = default;

GameObject::~GameObject()
{
	if (m_hwnd)
	{
		//Don't delete as it only stores a reference
		m_hwnd = nullptr;
	}

	if (m_shader)
	{
		//Don't delete as it's a shared shader
		m_shader = nullptr;
	}

	if (m_texture)
	{
		delete m_texture;
		m_texture = nullptr;
	}

	if (m_model)
	{
		delete m_model;
		m_model = nullptr;
	}

	if (m_collider)
	{
		delete m_collider;
		m_collider = nullptr;
	}

	if (m_rigidBody)
	{
		delete m_rigidBody;
		m_rigidBody = nullptr;
	}

	if (m_scale)
	{
		delete m_scale;
		m_scale = nullptr;
	}

	if (m_rotation)
	{
		delete m_rotation;
		m_rotation = nullptr;
	}

	if (m_position)
	{
		delete m_position;
		m_position = nullptr;
	}
}

GameObject& GameObject::operator=(const GameObject& other) = default;

GameObject& GameObject::operator=(GameObject&& other) noexcept = default;

void GameObject::AddPositionComponent() {
	m_position = new Position();
}

void GameObject::AddPositionComponent(const XMFLOAT3 position) {
	m_position = new Position(position);
}

void GameObject::AddPositionComponent(const float x, const float y, const float z) {
	m_position = new Position(x, y, z);
}

void GameObject::AddRotationComponent() {
	m_rotation = new Rotation();
}

void GameObject::AddRotationComponent(const XMFLOAT4 rotation) {
	m_rotation = new Rotation(rotation);
}

void GameObject::AddRotationComponent(const float x, const float y, const float z, const float w) {
	m_rotation = new Rotation(x, y, z, w);
}

void GameObject::AddScaleComponent() {
	m_scale = new Scale();
}

void GameObject::AddScaleComponent(const XMFLOAT3 scale) {
	m_scale = new Scale(scale);
}

//void GameObject::AddScaleComponent(const float x, const float y, const float z) {
//
//	m_scale = new Scale(x, y, z);
//}

void GameObject::AddVelocityComponent()
{
	m_velocity = new Velocity();
}

void GameObject::AddVelocityComponent(const XMFLOAT3 velocity)
{
	m_velocity = new Velocity(velocity);
}

void GameObject::AddVelocityComponent(const float x, const float y, const float z)
{
	m_velocity = new Velocity(x, y, z);
}

//Inertia tensor is based off the model type, if the model isn't initialised before the rigidbody then it will try the colliders type, else it throws an error stating this
void GameObject::AddRigidBodyComponent(const bool useGravity, const float mass, const float drag, const float angularDrag, const XMFLOAT3 position, const XMFLOAT4 rotation, const XMFLOAT3 velocity, const XMFLOAT3 angularVelocity) {
	
	auto inertiaTensor = XMFLOAT3X3();
	auto scale = XMVECTOR();


	if (m_scale)
	{
		m_scale->GetScale(scale);
	}
	else
	{
		MessageBox(m_hwnd, "You need to define a scale component before the rigidbody!", "Error: Missing Component", MB_OK);
		m_initializationFailed = true;
		return;
	}

	if (m_model)
	{
		const auto scaleX = XMVectorGetX(scale);
		const auto scaleY = XMVectorGetY(scale);
		const auto scaleZ = XMVectorGetZ(scale);

		switch (m_model->GetModelType())
		{
			case Model::ModelType::Sphere:
				inertiaTensor = XMFLOAT3X3(0.4f * (mass * (scaleX * scaleX)), 0.0f, 0.0f,
								0.0f, 0.4f * (mass * (scaleX * scaleX)) , 0.0f,
								0.0f, 0.0f, 0.4f * (mass * (scaleX * scaleX)));
				break;
			case Model::ModelType::Cube:
				inertiaTensor = XMFLOAT3X3(1.0f/ 12.0f * (mass * (scaleY * scaleY + scaleZ * scaleZ)), 0.0f, 0.0f,
								0.0f, 1.0f / 12.0f * (mass * (scaleX * scaleX + scaleZ * scaleZ)), 0.0f,
								0.0f, 0.0f, 1.0f / 12.0f * (mass * (scaleX * scaleX + scaleY * scaleY)));
				break;
				case Model::ModelType::Plane:
				//No inertia tensor needed
				break;
			case Model::ModelType::Cylinder:
				inertiaTensor = XMFLOAT3X3(((1.0f / 12.0f) * (mass * (scaleY * scaleY))) + 0.25f * (mass * (scaleX * scaleX)), 0.0f, 0.0f,
								0.0f, 0.5f * (mass * (scaleX * scaleX)), 0.0f,
								0.0f, 0.0f, ((1.0f / 12.0f) * (mass * (scaleY * scaleY))) + 0.25f * (mass * (scaleX * scaleX)));
				break;
			default:
				m_initializationFailed = true;
				return;
		}
	}
	else
	{
		MessageBox(m_hwnd, "You need to define a model component before the rigidbody!", "Error: Missing Component", MB_OK);
		m_initializationFailed = true;
		return;
	}
	
	m_rigidBody = new RigidBody(useGravity, mass, drag, angularDrag, position, rotation, velocity, angularVelocity, inertiaTensor);
}

void GameObject::AddColliderComponent(const Collider::ColliderType colliderType) {

	switch (colliderType)
	{
		case Collider::ColliderType::Sphere:
			m_collider = new SphereCollider();
			break;
		case Collider::ColliderType::AABBCube:
			m_collider = new AABBCubeCollider();
			break;
		case Collider::ColliderType::OBBCube:
			m_collider = new OBBCubeCollider();
			break;
		case Collider::ColliderType::Plane:
			m_collider = new PlaneCollider();
			break;
		case Collider::ColliderType::Cylinder:
			m_collider = new CylinderCollider();
			break;
		default:
			m_initializationFailed = true;
			break;
	}
}

void GameObject::SetPlaneColliderData(const XMFLOAT3& centre, const XMFLOAT3& pointOne, const XMFLOAT3& pointTwo, const float offset) const
{
	auto* planeCollider = dynamic_cast<PlaneCollider*>(m_collider);

	if (!planeCollider)
	{
		MessageBox(m_hwnd, "You tried setting data to a collider type that's isn't a plane collider!", "Error: Collider Conversion", MB_OK);
		return;
	}

	planeCollider->SetNormal(centre, pointOne, pointTwo);
	planeCollider->SetOffset(offset);

	planeCollider = nullptr;
}


void GameObject::AddModelComponent(ID3D11Device* device, const Model::ModelType modelType, ResourceManager* resourceManager) {

	m_model = new Model(device, modelType, resourceManager);

	if (m_model->GetInitializationState())
	{
		delete m_model;
		m_model = nullptr;

		m_initializationFailed = true;
	}
}

void GameObject::AddTextureComponent(ID3D11Device* device, const WCHAR* textureFileName, ResourceManager* resourceManager) {
	
	//Create and load texture
	m_texture = new Texture(device, textureFileName, resourceManager);

	if (m_texture->GetInitializationState())
	{
		delete m_texture;
		m_texture = nullptr;

		m_initializationFailed = true;
	}
}

void GameObject::AddShaderComponent(Shader* shader) {
	m_shader = shader;
}


Position* GameObject::GetPositionComponent() const {
	return m_position;
}

XMFLOAT3 GameObject::GetPosition() const {
	return m_position->GetPosition();
}

Rotation* GameObject::GetRotationComponent() const {
	return m_rotation;
}

XMFLOAT4 GameObject::GetRotation() const {
	return m_rotation->GetRotation();
}

Scale* GameObject::GetScaleComponent() const {
	return m_scale;
}

void GameObject::GetScale(XMVECTOR &scale) const {

	m_scale->GetScale(scale);
}

Velocity* GameObject::GetVelocityComponent() const
{
	return m_velocity;
}

XMFLOAT3 GameObject::GetVelocity() const
{
	return m_velocity->GetVelocity();
}

RigidBody* GameObject::GetRigidBodyComponent() const {
	return m_rigidBody;
}

Collider* GameObject::GetColliderComponent() const {
	return m_collider;
}

int GameObject::GetIndexCount() const {
	return m_model->GetIndexCount();
}

ID3D11ShaderResourceView* GameObject::GetTexture() const {
	return m_texture->GetTexture();
}

bool GameObject::GetInitializationState() const {
	return m_initializationFailed;
}

bool GameObject::Render(ID3D11DeviceContext* deviceContext, XMMATRIX &worldMatrix, XMMATRIX &viewMatrix, XMMATRIX &projectionMatrix, XMFLOAT4 diffuseLight, XMFLOAT3 lightDirection) {
	
	//const auto position = m_position->GetPosition();
	//const auto rotation = m_rotation->GetRotation();
	auto position = XMVECTOR();
	auto rotation = XMVECTOR();
	auto scale = XMVECTOR();

	m_rigidBody->GetPosition(position);
	m_rigidBody->GetRotation(rotation);
	m_scale->GetScale(scale);

	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixScalingFromVector(scale));
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixRotationQuaternion(rotation));
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslationFromVector(position));

	m_model->Render(deviceContext);

	auto result = true;

	//Render shader
	if (m_shader)
	{
		result = m_shader->Render(deviceContext, GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, GetTexture(), diffuseLight, lightDirection);
	}

	return result;
}

void GameObject::ChangeRandomTexture() {
	m_texture->ChangeRandomTexture();
}


