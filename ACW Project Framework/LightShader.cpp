#include "LightShader.h"

LightShader::LightShader(ID3D11Device* device, HWND hwnd) : Shader("LightVertexShader", "LightPixelShader", device, hwnd), m_inputLayout(nullptr), m_sampleState(nullptr), m_lightBuffer(nullptr)
{
	if (m_initializationFailed)
	{
		return;
	}

	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	D3D11_SAMPLER_DESC samplerDescription;
	D3D11_BUFFER_DESC lightBufferDescription;

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

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

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
		return;
	}

	lightBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDescription.ByteWidth = sizeof(LightBufferType); // Is a multiple of 16 because our extra float is inside
	lightBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDescription.MiscFlags = 0;
	lightBufferDescription.StructureByteStride = 0;

	result = device->CreateBuffer(&lightBufferDescription, nullptr, &m_lightBuffer);

	if (FAILED(result))
	{
		m_initializationFailed = true;
	}
}

LightShader::LightShader(const LightShader& other) = default;

LightShader::LightShader(LightShader&& other) noexcept = default;

LightShader::~LightShader()
{
	//Release resources
	if (m_lightBuffer)
	{
		m_lightBuffer->Release();
		m_lightBuffer = nullptr;
	}

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

LightShader& LightShader::operator=(const LightShader& other) = default;

LightShader& LightShader::operator=(LightShader&& other) noexcept = default;

bool LightShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 diffuseColour, XMFLOAT3 lightDirection) {
	
	auto const result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, diffuseColour, lightDirection);

	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount);

	return true;
}

bool LightShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 diffuseColour, XMFLOAT3 lightDirection) {

	auto result = Shader::SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);

	if (!result)
	{
		return false;
	}

	//Set the texture resource to the pixel shader
	deviceContext->PSSetShaderResources(0, 1, &texture);

	//Lock light constant buffer
	result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &m_mappedResource);

	if (FAILED(result))
	{
		return false;
	}

	auto* lightBufferDataPointer = static_cast<LightBufferType*>(m_mappedResource.pData);

	//Copy light variables to constant buffer
	lightBufferDataPointer->diffuseColour = diffuseColour;
	lightBufferDataPointer->lightDirection = lightDirection;
	lightBufferDataPointer->padding = 0.0f;

	lightBufferDataPointer = nullptr;

	//Unlock constant buffer
	deviceContext->Unmap(m_lightBuffer, 0);

	const unsigned int bufferNumber = 0;

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

	return true;
}

void LightShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount) const {
	//Set input layout
	deviceContext->IASetInputLayout(m_inputLayout);

	//Set our shaders
	Shader::RenderShader(deviceContext);

	//Set pixel shaders sampler state
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	//Render model
	deviceContext->DrawIndexed(indexCount, 0, 0);
}