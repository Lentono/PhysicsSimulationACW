#include "TextureShader.h"

TextureShader::TextureShader(ID3D11Device* device, HWND hwnd) : Shader("TextureVertexShader", "TexturePixelShader", device, hwnd), m_inputLayout(nullptr), m_sampleState(nullptr)
{
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	D3D11_SAMPLER_DESC samplerDescription;

	unsigned int numberOfElements = 0;

	//Setup layout of buffer data in the shader
	//Setup of the layout needs to match the struct in our Model class and the struct in the shader

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	//Get count of elements in layout
	numberOfElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//Create vertex input layout
	auto result = device->CreateInputLayout(polygonLayout, numberOfElements, m_vertexShaderBuffer->GetBufferPointer(), m_vertexShaderBuffer->GetBufferSize(), &m_inputLayout);

	if (FAILED(result))
	{
		m_initializationFailed = true;
		return;
	}

	//Release buffer resources
	m_vertexShaderBuffer->Release();
	m_vertexShaderBuffer = nullptr;

	m_pixelShaderBuffer->Release();
	m_pixelShaderBuffer = nullptr;

	//Create our texture sampler state description
	//Filter determines which pixels will be used/combined for the final look of the texture on the polygon face
	//The filter used is costly in performance but best in visual looks
	samplerDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.MipLODBias = 0.0f;
	samplerDescription.MaxAnisotropy = 1;
	samplerDescription.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDescription.BorderColor[0] = 0.0f;
	samplerDescription.BorderColor[1] = 0.0f;
	samplerDescription.BorderColor[2] = 0.0f;
	samplerDescription.BorderColor[3] = 0.0f;
	samplerDescription.MinLOD = 0.0f;
	samplerDescription.MaxLOD = D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState(&samplerDescription, &m_sampleState);

	if (FAILED(result))
	{
		m_initializationFailed = true;
	}
}

TextureShader::TextureShader(const TextureShader& other) = default;

TextureShader::TextureShader(TextureShader&& other) noexcept = default;

TextureShader::~TextureShader()
{
	//Release resources
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = nullptr;
	}

	if (m_inputLayout)
	{
		m_inputLayout->Release();
		m_inputLayout = nullptr;
	}
}

TextureShader& TextureShader::operator=(const TextureShader& other) = default;

TextureShader& TextureShader::operator=(TextureShader&& other) noexcept = default;

bool TextureShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 diffuseColour, XMFLOAT3 lightDirection) {
	
	auto const result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture);

	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount);

	return true;
}

bool TextureShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture) {
	
	Shader::SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);

	//Set the texture resource to the pixel shader
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

void TextureShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount) const {
	//Set input layout
	deviceContext->IASetInputLayout(m_inputLayout);

	//Set our shaders
	Shader::RenderShader(deviceContext);

	//Set pixel shaders sampler state
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	//Render triangle
	deviceContext->DrawIndexed(indexCount, 0, 0);
}
