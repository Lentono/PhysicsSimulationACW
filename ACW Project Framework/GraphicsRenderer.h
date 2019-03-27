#pragma once

#include <Windows.h>
#include <vector>
#include <DirectXMath.h>

#include "D3DContainer.h"

#include "Camera.h"
#include "GameObject.h"
#include "Light.h"

#include "ShaderManager.h"
#include "ResourceManager.h"
#include "PhysicsManager.h"
#include "CollisionManager.h"
#include "GameObjectFactory.h"
#include "ResolutionManager.h"

using namespace DirectX;

auto const FULL_SCREEN = false;
auto const VSYNC_ENABLED = true;
auto const SCREEN_DEPTH = 1000.0f;
auto const SCREEN_NEAR = 0.1f;

class GraphicsRenderer
{
public:
	GraphicsRenderer(int screenWidth, int screenHeight, HWND hwnd); // Default Constructor
	GraphicsRenderer(const GraphicsRenderer& other); // Copy Constructor
	GraphicsRenderer(GraphicsRenderer&& other) noexcept; // Move Constructor
	~GraphicsRenderer(); // Destructor

	GraphicsRenderer& operator = (const GraphicsRenderer& other); // Copy Assignment Operator
	GraphicsRenderer& operator = (GraphicsRenderer&& other) noexcept; // Move Assignment Operator

	Camera* GetCamera() const;

	void TogglePauseSimulation();
	void ToggleRandomTexture();

	void ClearMoveableGameObjects();

	void AddNumberOfSpheres(const HWND hwnd);
	void AddCube(const HWND hwnd);

	void AddTimeScale(const int number);
	void AddNumberOfSpheres(const int number);
	void AddSphereDiameter(const float diameter);
	void AddFriction(const float friction);
	void AddRestitution(const float restitution);

	void UpdateConsole();

	bool Frame();

	bool GetInitializationState() const;

private:
	bool Render();

	bool m_initializationFailed;

	D3DContainer* m_d3D;
	Camera* m_camera;
	Light* m_light;

	GameObjectFactory* m_gameObjectFactory;

	vector<GameObject*> m_gameObjects;

	PhysicsManager* m_physicsManager;
	CollisionManager* m_collisionManager;
	ResolutionManager* m_resolutionManager;

	ShaderManager* m_shaderManager;
	ResourceManager* m_resourceManager;

	FILE* m_consoleOutputFile;

	bool m_pauseSimulation;

	int m_timeScale;
	int m_totalSpheresInSystem;
	int m_totalCubesInSystem;
	int m_numberOfSpheresToAdd;
	float m_sphereDiameter;
	float m_friction;
	float m_restitution;

	float m_dt;
	float m_fps;
	LARGE_INTEGER m_start;
	LARGE_INTEGER m_end;
	LARGE_INTEGER m_frequency;
};