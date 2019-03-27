#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>
#include "Shader.h"

using namespace std;
using namespace DirectX;

class ColourShader : public Shader
{
public:

	ColourShader(ID3D11Device* device, HWND hwnd); // Default Constructor
	ColourShader(const ColourShader& other); // Copy Constructor
	ColourShader(ColourShader && other) noexcept; // Move Constructor
	~ColourShader() override;

	ColourShader& operator = (const ColourShader& other); // Copy Assignment Operator
	ColourShader& operator = (ColourShader && other) noexcept; // Move Assignment Operator

	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 diffuseColour, XMFLOAT3 lightDirection) override;

private:
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount) const;

	ID3D11InputLayout* m_inputLayout;
};