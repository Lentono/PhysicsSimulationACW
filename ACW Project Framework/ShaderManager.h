#pragma once

#include "D3DContainer.h"

//Include all shader header files
#include "ColourShader.h"
#include "LightShader.h"
#include "TextureShader.h"

class ShaderManager
{
public:

	ShaderManager(ID3D11Device* device, HWND hwnd); //Default Constructor
	ShaderManager(const ShaderManager& other); //Copy Constructor
	ShaderManager(ShaderManager&& other) noexcept; //Move Constructor
	~ShaderManager(); //Destructor

	ShaderManager& operator = (const ShaderManager& other); //Copy Assignment Operator
	ShaderManager& operator = (ShaderManager&& other) noexcept; //Move Assignment Operator

	Shader* GetColourShader() const;
	Shader* GetLightShader() const;
	Shader* GetTextureShader() const;

	//Example methods for current shaders, will need to make one for each new shader created or final ones used
	//bool RenderColourShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);
	//bool RenderLightShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 diffuseColour, XMFLOAT3 lightDirection);
	//bool RenderTextureShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture);

	bool GetInitializationState() const;

private:

	bool m_initializationFailed;

	Shader* m_colourShader;
	Shader* m_lightShader;
	Shader* m_textureShader;
};

