#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <fstream>
#include "Shader.h"

using namespace DirectX;
using namespace std;

class LightShader : public Shader
{
public:
	LightShader(ID3D11Device* device, HWND hwnd); // Default Constructor
	LightShader(const LightShader& other); // Copy Constructor
	LightShader(LightShader&& other) noexcept; // Move Constructor
	~LightShader() override; // Destructor

	LightShader& operator = (const LightShader& other); // Copy Assignment Operator
	LightShader& operator = (LightShader&& other) noexcept; // Move Assignment Operator

	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 diffuseColour, XMFLOAT3 lightDirection) override;

private: 
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 diffuseColour, XMFLOAT3 lightDirection);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount) const;

	struct LightBufferType {
		XMFLOAT4 diffuseColour;
		XMFLOAT3 lightDirection;
		float padding; //Used to pad the structure to a multiple of 16 (CreateBuffer function requirement?)
	};

	ID3D11InputLayout* m_inputLayout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_lightBuffer;
};