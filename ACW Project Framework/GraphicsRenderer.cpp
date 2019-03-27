#include "GraphicsRenderer.h"
#include <iostream>

GraphicsRenderer::GraphicsRenderer(int screenWidth, int screenHeight, HWND hwnd) : m_initializationFailed(false), m_d3D(nullptr), m_camera(nullptr), m_light(nullptr), m_gameObjectFactory(nullptr), m_physicsManager(nullptr), m_shaderManager(nullptr), m_resourceManager(nullptr), m_consoleOutputFile(nullptr), m_pauseSimulation(false), m_timeScale(1), m_totalSpheresInSystem(0), m_totalCubesInSystem(0), m_numberOfSpheresToAdd(200), m_sphereDiameter(0.7f), m_friction(0.4f), m_restitution(0.4f) {
	//Create D3D object
	m_d3D = new D3DContainer(screenWidth, screenHeight, hwnd, FULL_SCREEN, VSYNC_ENABLED, SCREEN_DEPTH, SCREEN_NEAR);

	if (m_d3D->GetInitializationState()) {
		m_initializationFailed = true;
		MessageBox(hwnd, "Direct3D failed to Initialize", "Error", MB_OK);
		return;
	}

	m_shaderManager = new ShaderManager(m_d3D->GetDevice(), hwnd);

	if (m_shaderManager->GetInitializationState())
	{
		m_initializationFailed = true;
		MessageBox(hwnd, "ShaderManager failed to Initialize", "Error", MB_OK);
		return;
	}

	m_resourceManager = new ResourceManager(m_d3D->GetDevice());

	if (!m_resourceManager)
	{
		m_initializationFailed = true;
		return;
	}

	//Create camera
	m_camera = new Camera();

	if (!m_camera)
	{
		m_initializationFailed = true;
		return;
	}

	//Set camera position
	m_camera->SetPosition(0.0f, 25.0f, -70.0f);
	//m_camera->SetPosition(0.0f, 1.0f, -30.0f);

	m_light = new Light();

	if (!m_light)
	{
		m_initializationFailed = true;
		return;
	}

	//Light colour is set to red
	m_light->SetDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	m_light->SetLightDirection(0.0f, 1.0f, 1.0f);

	m_gameObjectFactory = new GameObjectFactory(m_gameObjects);

	m_gameObjects.push_back(new GameObject(hwnd));
	//m_gameObjects.back()->AddPositionComponent(0.0f, -3.0f, 0.0f);
	//m_gameObjects.back()->AddRotationComponent(0.0f, 0.0f, 0.0f);
	m_gameObjects.back()->AddScaleComponent(XMFLOAT3(9.375f, 1.0f, 3.0f));
	m_gameObjects.back()->AddColliderComponent(Collider::ColliderType::Plane);
	m_gameObjects.back()->SetPlaneColliderData(XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(9.375f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 9.375f), -1.0f);
	m_gameObjects.back()->AddModelComponent(m_d3D->GetDevice(), Model::ModelType::Plane, m_resourceManager);
	m_gameObjects.back()->AddRigidBodyComponent(false, 0.5f, 0.2f, 0.1f, XMFLOAT3(0.0f, 0.375f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT3(), XMFLOAT3());
	m_gameObjects.back()->AddTextureComponent(m_d3D->GetDevice(), L"walls.dds", m_resourceManager);
	m_gameObjects.back()->AddShaderComponent(m_shaderManager->GetTextureShader());

	//Left wall
	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(-9.375f, 16.875f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.375f, 16.5f, 3.0f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::AABBCube, Model::ModelType::Cube, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"walls.dds", m_resourceManager);

	//Right wall
	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(9.375f, 16.875f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.375f, 16.5f, 3.0f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::AABBCube, Model::ModelType::Cube, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"walls.dds", m_resourceManager);

	//Back wall
	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(0.0f, 16.875f, 3.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(9.0f, 16.5f, 0.375f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::AABBCube, Model::ModelType::Cube, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"walls.dds", m_resourceManager);

	//Top wall left
	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(-5.25f, 33.75f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(3.75f, 0.375f, 3.0f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::AABBCube, Model::ModelType::Cube, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"walls.dds", m_resourceManager);

	//Top wall right
	//Bottom floor
	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(5.25f, 33.75f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(3.75f, 0.375f, 3.0f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::AABBCube, Model::ModelType::Cube, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"walls.dds", m_resourceManager);

	//bins
	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(-7.5f, 7.875f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0375f, 7.5f, 3.0f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::AABBCube, Model::ModelType::Cube, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(-6.0f, 7.875f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0375f, 7.5f, 3.0f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::AABBCube, Model::ModelType::Cube, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(-4.5f, 7.875f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0375f, 7.5f, 3.0f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::AABBCube, Model::ModelType::Cube, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(-3.0f, 7.875f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0375f, 7.5f, 3.0f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::AABBCube, Model::ModelType::Cube, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(-1.5f, 7.875f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0375f, 7.5f, 3.0f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::AABBCube, Model::ModelType::Cube, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(0.0f, 7.875f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0375f, 7.5f, 3.0f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::AABBCube, Model::ModelType::Cube, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(1.5f, 7.875f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0375f, 7.5f, 3.0f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::AABBCube, Model::ModelType::Cube, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(3.0f, 7.875f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0375f, 7.5f, 3.0f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::AABBCube, Model::ModelType::Cube, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(4.5f, 7.875f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0375f, 7.5f, 3.0f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::AABBCube, Model::ModelType::Cube, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(6.0f, 7.875f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0375f, 7.5f, 3.0f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::AABBCube, Model::ModelType::Cube, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(7.5f, 7.875f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0375f, 7.5f, 3.0f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::AABBCube, Model::ModelType::Cube, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	//Pegs - Row 1
	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(-6.75f, 30.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(-5.25f, 30.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(-3.75f, 30.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(-2.25f, 30.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(-0.75f, 30.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(0.75f, 30.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(2.25f, 30.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(3.75f, 30.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(5.25f, 30.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(6.75f, 30.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	//Pegs - Row 2
	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(-6.0f, 27.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(-4.5f, 27.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(-3.0f, 27.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(-1.5f, 27.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(0.0f, 27.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(1.5f, 27.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(3.0f, 27.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(4.5f, 27.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(6.0f, 27.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	//Pegs - Row 3
	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(-6.75f, 24.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(-5.25f, 24.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(-3.75f, 24.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(-2.25f, 24.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(-0.75f, 24.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(0.75f, 24.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(2.25f, 24.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(3.75f, 24.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(5.25f, 24.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(6.75f, 24.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	//Pegs - Row 4
	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(-6.0f, 21.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(-4.5f, 21.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(-3.0f, 21.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(-1.5f, 21.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(0.0f, 21.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(1.5f, 21.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(3.0f, 21.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(4.5f, 21.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(6.0f, 21.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	//Pegs - Row 5
	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(-6.75f, 18.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(-5.25f, 18.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(-3.75f, 18.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(-2.25f, 18.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(-0.75f, 18.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(0.75f, 18.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(2.25f, 18.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(3.75f, 18.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(5.25f, 18.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(6.75f, 18.0f, 0.0f), XMFLOAT3(XM_PIDIV2, 0.0f, 0.0), XMFLOAT3(0.075f, 3.0f, 0.075f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::Cylinder, Model::ModelType::Cylinder, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"bins.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(-5.75f, 36.75f, 0.0f), XMFLOAT3(0.0f, 0.0f, -XM_PI/6), XMFLOAT3(4.5f, 0.75f, 3.0f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::OBBCube, Model::ModelType::Cube, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"walls.dds", m_resourceManager);

	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(5.75f, 36.75f, 0.0f), XMFLOAT3(0.0f, 0.0f, XM_PI / 6), XMFLOAT3(4.5f, 0.75f, 3.0f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::OBBCube, Model::ModelType::Cube, false, 0.5f, 0.0f, 0.0f,
		m_shaderManager->GetTextureShader(), L"walls.dds", m_resourceManager);

	auto quaternionRotation = XMFLOAT4();

	XMStoreFloat4(&quaternionRotation, XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, -XM_PIDIV2));

	m_gameObjects.push_back(new GameObject(hwnd));
	//m_gameObjects.back()->AddPositionComponent(0.0f, -3.0f, 0.0f);
	//m_gameObjects.back()->AddRotationComponent(0.0f, 0.0f, 0.0f);
	m_gameObjects.back()->AddScaleComponent(XMFLOAT3(15.0f, 1.0f, 3.0f));
	m_gameObjects.back()->AddColliderComponent(Collider::ColliderType::Plane);
	m_gameObjects.back()->SetPlaneColliderData(XMFLOAT3(-9.375f, 45.0f, 0.0f), XMFLOAT3(-9.375f, 46.0f, 0.0f), XMFLOAT3(-9.375f, 45.0f, 1.0f), 8.5f);
	m_gameObjects.back()->AddModelComponent(m_d3D->GetDevice(), Model::ModelType::Plane, m_resourceManager);
	m_gameObjects.back()->AddRigidBodyComponent(false, 0.5f, 0.2f, 0.1f, XMFLOAT3(-9.375f, 45.0f, 0.0f), quaternionRotation, XMFLOAT3(), XMFLOAT3());
	m_gameObjects.back()->AddTextureComponent(m_d3D->GetDevice(), L"walls.dds", m_resourceManager);
	m_gameObjects.back()->AddShaderComponent(m_shaderManager->GetTextureShader());

	XMStoreFloat4(&quaternionRotation, XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, XM_PIDIV2));

	m_gameObjects.push_back(new GameObject(hwnd));
	//m_gameObjects.back()->AddPositionComponent(0.0f, -3.0f, 0.0f);
	//m_gameObjects.back()->AddRotationComponent(0.0f, 0.0f, 0.0f);
	m_gameObjects.back()->AddScaleComponent(XMFLOAT3(15.0f, 1.0f, 3.0f));
	m_gameObjects.back()->AddColliderComponent(Collider::ColliderType::Plane);
	m_gameObjects.back()->SetPlaneColliderData(XMFLOAT3(9.375f, 0.0f, 0.0f), XMFLOAT3(9.375f, 0.0f, 1.0f), XMFLOAT3(9.375f, 1.0f, 0.0f), 8.5f);
	m_gameObjects.back()->AddModelComponent(m_d3D->GetDevice(), Model::ModelType::Plane, m_resourceManager);
	m_gameObjects.back()->AddRigidBodyComponent(false, 0.5f, 0.2f, 0.1f, XMFLOAT3(9.375f, 45.0f, 0.0f), quaternionRotation, XMFLOAT3(), XMFLOAT3());
	m_gameObjects.back()->AddTextureComponent(m_d3D->GetDevice(), L"walls.dds", m_resourceManager);
	m_gameObjects.back()->AddShaderComponent(m_shaderManager->GetTextureShader());

	m_physicsManager = new PhysicsManager(m_gameObjects);
	m_collisionManager = new CollisionManager(m_gameObjects, m_friction, m_restitution);
	m_resolutionManager = new ResolutionManager(m_collisionManager->GetContactManifoldReference(), 1000, 1000, 0.001f, 0.01f);

	QueryPerformanceFrequency(&m_frequency);
	QueryPerformanceCounter(&m_start);

	//Create console window
	if (!AllocConsole())
	{
		MessageBox(nullptr, "The console window was not created", nullptr, MB_ICONEXCLAMATION);
	}

	freopen_s(&m_consoleOutputFile, "CONOUT$", "w", stdout);

	UpdateConsole();

	//For each gameObject that has a rigidbody we clear our accumulated force and calculate our derived data for the transform matrix (translation and rotation matrix)
	for (auto* gameObject : m_gameObjects)
	{
		gameObject->GetRigidBodyComponent()->ClearAccumulators();
		gameObject->GetRigidBodyComponent()->CalculateDerivedData();
	}
}

GraphicsRenderer::GraphicsRenderer(const GraphicsRenderer& other) = default;

GraphicsRenderer::GraphicsRenderer(GraphicsRenderer&& other) noexcept = default;

GraphicsRenderer::~GraphicsRenderer()
{
	//Release resources

	if (m_resourceManager)
	{
		delete m_resourceManager;
		m_resourceManager = nullptr;
	}

	if (m_shaderManager)
	{
		delete m_shaderManager;
		m_shaderManager = nullptr;
	}

	if (m_collisionManager)
	{
		delete m_collisionManager;
		m_collisionManager = nullptr;
	}

	if (m_physicsManager)
	{
		delete m_physicsManager;
		m_physicsManager = nullptr;
	}

	for (auto* gameObject : m_gameObjects)
	{
		if (gameObject)
		{
			delete gameObject;
			gameObject = nullptr;
		}
	}

	if (m_light)
	{
		delete m_light;
		m_light = nullptr;
	}

	if (m_camera)
	{
		delete m_camera;
		m_camera = nullptr;
	}

	if (m_d3D) {
		delete m_d3D;
		m_d3D = nullptr;
	}
}

GraphicsRenderer& GraphicsRenderer::operator=(const GraphicsRenderer&) = default;

GraphicsRenderer& GraphicsRenderer::operator=(GraphicsRenderer&&) noexcept = default;

Camera* GraphicsRenderer::GetCamera() const
{
	return m_camera;
}

void GraphicsRenderer::TogglePauseSimulation()
{
	m_pauseSimulation = !m_pauseSimulation;
}

void GraphicsRenderer::ToggleRandomTexture() {
	m_collisionManager->ToggleRandomTexture();
}

void GraphicsRenderer::ClearMoveableGameObjects()
{
	for (unsigned i = 0; i < m_gameObjects.size(); i++)
	{
		if (m_gameObjects[i]->GetRigidBodyComponent()->GetUseGravity())
		{
			delete m_gameObjects[i];
			m_gameObjects.at(i) = nullptr;
			m_gameObjects.erase(m_gameObjects.begin() + i);
			--i;
		}
	}

	m_totalSpheresInSystem = 0;
	m_totalCubesInSystem = 0;
	UpdateConsole();
}


void GraphicsRenderer::AddNumberOfSpheres(const HWND hwnd)
{
	XMFLOAT3 startPosition(-7.5f, 38.75f, 0.0f);
	auto distributionX = abs(startPosition.x * 2) / 7;

	int totalCount = 0;
	unsigned xCount = 1;
	unsigned yCount = 1;

	while (totalCount < m_numberOfSpheresToAdd)
	{
		if (xCount == 7)
		{
			xCount = 1;
			yCount++;
		}

		m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(startPosition.x + (xCount * distributionX), startPosition.y + (yCount * distributionX), 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(m_sphereDiameter / 2, m_sphereDiameter / 2, m_sphereDiameter / 2), XMFLOAT3(), XMFLOAT3(),
			Collider::ColliderType::Sphere, Model::ModelType::Sphere, true, 0.5f, 0.3f, 0.3f,
			m_shaderManager->GetTextureShader(), L"sphere2.dds", m_resourceManager);

		xCount++;
		totalCount++;
	}

	m_totalSpheresInSystem += m_numberOfSpheresToAdd;

	//For each gameObject that has a rigidbody we clear our accumulated force and calculate our derived data for the transform matrix (translation and rotation matrix)
	for (auto* gameObject : m_gameObjects)
	{
		gameObject->GetRigidBodyComponent()->ClearAccumulators();
		gameObject->GetRigidBodyComponent()->CalculateDerivedData();
	}

	UpdateConsole();
}

void GraphicsRenderer::AddCube(const HWND hwnd)
{
	m_gameObjectFactory->AddGameObject(hwnd, m_d3D->GetDevice(), XMFLOAT3(0.3f, 42.75f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.45f, 0.45f, 0.45f), XMFLOAT3(), XMFLOAT3(),
		Collider::ColliderType::OBBCube, Model::ModelType::Cube, true, 0.2f, 0.1f, 0.1f,
		m_shaderManager->GetTextureShader(), L"sphere.dds", m_resourceManager);

	m_totalCubesInSystem++;

	//For each gameObject that has a rigidbody we clear our accumulated force and calculate our derived data for the transform matrix (translation and rotation matrix)
	for (auto* gameObject : m_gameObjects)
	{
		gameObject->GetRigidBodyComponent()->ClearAccumulators();
		gameObject->GetRigidBodyComponent()->CalculateDerivedData();
	}

	UpdateConsole();
}

void GraphicsRenderer::AddTimeScale(const int number)
{
	m_timeScale += number;

	if (m_timeScale < 1)
	{
		m_timeScale = 1;
	}

	UpdateConsole();
}

void GraphicsRenderer::AddNumberOfSpheres(const int number)
{
	m_numberOfSpheresToAdd += number;

	if (m_numberOfSpheresToAdd < 0)
	{
		m_numberOfSpheresToAdd = 0;
	}

	UpdateConsole();
}

void GraphicsRenderer::AddSphereDiameter(const float diameter)
{
	m_sphereDiameter += diameter;

	if (m_sphereDiameter < 0.1f)
	{
		m_sphereDiameter = 0.1f;
	}

	if (m_sphereDiameter > 0.9f)
	{
		m_sphereDiameter = 0.9f;
	}

	UpdateConsole();
}

void GraphicsRenderer::AddFriction(const float friction)
{
	m_friction += friction;

	if (m_friction < 0.0f)
	{
		m_friction = 0.0f;
	}

	m_collisionManager->SetFriction(m_friction);

	UpdateConsole();
}

void GraphicsRenderer::AddRestitution(const float restitution)
{
	m_restitution += restitution;

	if (m_restitution < 0.0f)
	{
		m_restitution = 0.0f;
	}

	if (m_restitution > 1.0f)
	{
		m_restitution = 1.0f;
	}

	m_collisionManager->SetRestitution(m_restitution);

	UpdateConsole();
}

void GraphicsRenderer::UpdateConsole()
{
	system("cls");

	cout << " Number of balls in system: " << m_totalSpheresInSystem << endl;
	cout << " Number of cubes in system: " << m_totalCubesInSystem << endl;
	cout << " Friction: " << m_friction << endl;
	cout << " Restitution: " << m_restitution << endl << endl;

	cout << " 1 - Add Number of Spheres: " << m_numberOfSpheresToAdd << endl;
	cout << " 2 - Add Cube" << endl;
	cout << " R - Reset System" << endl;
	cout << " P - Toggle Pause Simulation" << endl;
	cout << " U, J - Increase/Decrease TimeScale: x" << m_timeScale << endl;
	cout << " [, ] - Increase/Decrease Number of Spheres: " << m_numberOfSpheresToAdd << endl;
	cout << " T, B - Increase/Decrease Sphere Diameter: " << m_sphereDiameter << endl;
	cout << " I, K - Increase/Decrease Friction: " << m_friction << endl;
	cout << " O, L - Increase/Decrease Restitution: " << m_restitution << endl << endl;
	cout << " W, S, A, D - Up, Down, Left, Right Camera Controls" << endl;
	cout << " Up, Down Arrow - Zoom In/Out" << endl;
}

bool GraphicsRenderer::Frame() {

	//calculate dt based on the simulation loop rate using a timer
	QueryPerformanceCounter(&m_end);
	m_dt = static_cast<float>((m_end.QuadPart - m_start.QuadPart) / static_cast<double>(m_frequency.QuadPart));
	m_start = m_end;

	m_dt *= m_timeScale;

	if (m_pauseSimulation)
	{
		m_dt = 0.0f;
	}

	m_fps = static_cast<int>(1.0 / m_dt);

	m_physicsManager->CalculateGameObjectPhysics(m_dt);

	m_collisionManager->DynamicCollisionDetection();

	//m_collisionManager->DynamicCollisionResponse(m_dt);
	m_resolutionManager->ResolveContacts(m_dt);

	m_physicsManager->UpdateGameObjectPhysics();

	//Render the graphics scene
	auto const result = Render();

	return result;
}

bool GraphicsRenderer::Render() {

	XMMATRIX viewMatrix = {};
	XMMATRIX projectionMatrix = {};
	XMMATRIX worldMatrix = {};

	//Clear the buffer and render the scene
	m_d3D->BeginScene(1.0f, 0.0f, 0.0f, 1.0f);

	m_camera->Render();

	m_camera->GetViewMatrix(viewMatrix);
	m_d3D->GetProjectionMatrix(projectionMatrix);
	m_d3D->GetWorldMatrix(worldMatrix);

	for (auto* gameObject : m_gameObjects)
	{
		const auto result = gameObject->Render(m_d3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, m_light->GetDiffuseColour(), m_light->GetLightDirection());

		if (!result)
		{
			return false;
		}

		m_d3D->GetWorldMatrix(worldMatrix);
	}

	//Present the scene
	m_d3D->EndScene();

	return true;
}


bool GraphicsRenderer::GetInitializationState() const {
	return m_initializationFailed;
}
