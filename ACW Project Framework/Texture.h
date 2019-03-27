#pragma once

//DirectX Libraries
#include <d3d11.h>
#include <d3d11_1.h>

#include "ResourceManager.h"

using namespace DirectX;

class Texture
{
public:
	Texture(ID3D11Device* device, const WCHAR* fileName, ResourceManager* resourceManager); // Default Constructor
	Texture(const Texture& other); // Copy Constructor
	Texture(Texture&& other); // Move Constructor
	~Texture(); // Destructor

	Texture& operator = (const Texture& other); // Copy Assignment Operator
	Texture& operator = (Texture&& other) noexcept; // Move Assignment Operator

	ID3D11ShaderResourceView* GetTexture() const;
	void ChangeRandomTexture();

	bool GetInitializationState() const;

private:
	ID3D11ShaderResourceView* m_texture;

	ResourceManager* m_resourceManager;

	bool m_initializationFailed;
};