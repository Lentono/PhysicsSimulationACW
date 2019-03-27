#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <fstream>
#include <atlbase.h>
#include <string>

using namespace DirectX;
using namespace std;

class Shader
{
public:
	Shader(string vertexShaderFileName, string pixelShaderFileName, ID3D11Device* device, HWND hwnd);
	Shader(const Shader& other); // Copy Constructor
	Shader(Shader&& other) noexcept; // Move Constructor
	virtual ~Shader();

	Shader& operator = (const Shader& other); // Copy assignment operator
	Shader& operator = (Shader&& other) noexcept; // Move assignment operator

	bool GetInitializationState() const;

	virtual bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 diffuseColour, XMFLOAT3 lightDirection) = 0;

protected:

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);
	void RenderShader(ID3D11DeviceContext* deviceContext) const;

	struct MatrixBufferType {
		XMMATRIX worldMatrix;
		XMMATRIX viewMatrix;
		XMMATRIX projectionMatrix;
	};

	bool m_initializationFailed;

	ID3D10Blob* m_vertexShaderBuffer;
	ID3D10Blob* m_pixelShaderBuffer;

	D3D11_MAPPED_SUBRESOURCE m_mappedResource;;

private:
	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, LPCSTR shaderFileName);

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11Buffer* m_matrixBuffer;
};

