#include "ShaderManager.h"

//So we only need one instance of each shader and the gameobject just declares the shader choice

ShaderManager::ShaderManager(ID3D11Device* device, HWND hwnd) : m_initializationFailed(false), m_colourShader(nullptr), m_lightShader(nullptr), m_textureShader(nullptr)
{
	//Initialize our shaders
	m_colourShader = new ColourShader(device, hwnd);

	if (m_colourShader->GetInitializationState())
	{
		m_initializationFailed = true;
		MessageBox(hwnd, "Could not initialize the colour shader", "Error", MB_OK);
		return;
	}

	m_lightShader = new LightShader(device, hwnd);

	if (m_lightShader->GetInitializationState())
	{
		m_initializationFailed = true;
		MessageBox(hwnd, "Could not initialize the light shader", "Error", MB_OK);
		return;
	}

	m_textureShader = new TextureShader(device, hwnd);

	if (m_textureShader->GetInitializationState())
	{
		m_initializationFailed = true;
		MessageBox(hwnd, "Could not initialize the texture shader", "Error", MB_OK);
	}
}

ShaderManager::ShaderManager(const ShaderManager& other) = default;

ShaderManager::ShaderManager(ShaderManager&& other) noexcept = default;

ShaderManager::~ShaderManager()
{
	if (m_textureShader)
	{
		delete m_textureShader;
		m_textureShader = nullptr;
	}

	if (m_lightShader)
	{
		delete m_lightShader;
		m_lightShader = nullptr;
	}

	if (m_colourShader)
	{
		delete m_colourShader;
		m_colourShader = nullptr;
	}
}

ShaderManager& ShaderManager::operator=(const ShaderManager& other) = default;

ShaderManager& ShaderManager::operator=(ShaderManager&& other) noexcept = default;

Shader* ShaderManager::GetColourShader() const {
	return m_colourShader;
}

Shader* ShaderManager::GetLightShader() const {
	return m_lightShader;
}

Shader* ShaderManager::GetTextureShader() const {
	return m_textureShader;
}



bool ShaderManager::GetInitializationState() const
{
	return m_initializationFailed;
}