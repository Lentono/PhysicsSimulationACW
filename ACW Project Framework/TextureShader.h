#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <fstream>
#include "Shader.h"

using namespace DirectX;
using namespace std;

class TextureShader : public Shader
{
public:
	TextureShader(ID3D11Device* device, HWND hwnd); // Default Constructor
	TextureShader(const TextureShader& other); // Copy Constructor
	TextureShader(TextureShader&& other) noexcept; // Move Constructor
	~TextureShader() override; // Destructor

	TextureShader& operator = (const TextureShader& other); // Copy Assignment Operator
	TextureShader& operator = (TextureShader&& other) noexcept; // Move Assignment Operator

	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 diffuseColour, XMFLOAT3 lightDirection) override;

private:
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount) const;

	ID3D11InputLayout* m_inputLayout;
	ID3D11SamplerState* m_sampleState;
};