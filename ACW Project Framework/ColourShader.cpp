#include "ColourShader.h"

ColourShader::ColourShader(ID3D11Device* device, HWND hwnd) : Shader("ColourVertexShader", "ColourPixelShader", device, hwnd), m_inputLayout(nullptr) {
	
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];

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

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	//Get count of elements in layout
	numberOfElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//Create vertex input layout
	const auto result = device->CreateInputLayout(polygonLayout, numberOfElements, m_vertexShaderBuffer->GetBufferPointer(), m_vertexShaderBuffer->GetBufferSize(), &m_inputLayout);

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
}

ColourShader::ColourShader(const ColourShader& other) = default;

ColourShader::ColourShader(ColourShader&& other) noexcept = default;

ColourShader::~ColourShader()
{
	//Release resources
	if (m_inputLayout)
	{
		m_inputLayout->Release();
		m_inputLayout = nullptr;
	}
}

ColourShader& ColourShader::operator=(const ColourShader& other) = default;

ColourShader& ColourShader::operator=(ColourShader&& other) noexcept = default;

bool ColourShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 diffuseColour, XMFLOAT3 lightDirection) {
	
	auto const result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);

	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount);

	return true;
}

bool ColourShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix) {
	
	const auto result = Shader::SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);

	return result;
}

void ColourShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount) const {
	
	//Set input layout
	deviceContext->IASetInputLayout(m_inputLayout);

	//Set our shaders
	Shader::RenderShader(deviceContext);

	//Render model
	deviceContext->DrawIndexed(indexCount, 0, 0);
}
