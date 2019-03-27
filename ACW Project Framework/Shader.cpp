#include "Shader.h"

Shader::Shader(string vertexShaderFileName, string pixelShaderFileName, ID3D11Device* device, HWND hwnd) : m_initializationFailed(false), m_vertexShaderBuffer(nullptr), m_pixelShaderBuffer(nullptr), m_vertexShader(nullptr), m_pixelShader(nullptr), m_matrixBuffer(nullptr)
{
	ID3D10Blob* errorMessage = nullptr;
	D3D11_BUFFER_DESC matrixBufferDescription;

	unsigned int numberOfElements = 0;

	//Load our Vertex and Pixel Shader

	const auto hlslVertexFileName = vertexShaderFileName + ".hlsl";
	
	auto result = D3DCompileFromFile(CA2W(hlslVertexFileName.c_str()), nullptr, nullptr, vertexShaderFileName.c_str(), "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &m_vertexShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		m_initializationFailed = true;

		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, hlslVertexFileName.c_str());
		}
		else
		{
			MessageBox(hwnd, hlslVertexFileName.c_str(), "Missing Vertex Shader File", MB_OK);
		}

		return;
	}

	const auto hlslPixelFileName = pixelShaderFileName + ".hlsl";

	result = D3DCompileFromFile(CA2W(hlslPixelFileName.c_str()), nullptr, nullptr, pixelShaderFileName.c_str(), "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &m_pixelShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		m_initializationFailed = true;

		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, hlslPixelFileName.c_str());
		}
		else
		{
			MessageBox(hwnd, hlslPixelFileName.c_str(), "Missing Pixel Shader File", MB_OK);
		}

		return;
	}

	//Create vertex and pixel shader from buffer
	result = device->CreateVertexShader(m_vertexShaderBuffer->GetBufferPointer(), m_vertexShaderBuffer->GetBufferSize(), nullptr, &m_vertexShader);

	if (FAILED(result))
	{
		m_initializationFailed = true;
		return;
	}

	result = device->CreatePixelShader(m_pixelShaderBuffer->GetBufferPointer(), m_pixelShaderBuffer->GetBufferSize(), nullptr, &m_pixelShader);

	if (FAILED(result))
	{
		m_initializationFailed = true;
		return;
	}

	//Setup the description of the dynamic matrix constant buffer that is in the vertex shader
	matrixBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDescription.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDescription.MiscFlags = 0;
	matrixBufferDescription.StructureByteStride = 0;

	//Create constant buffer pointer so we can access the vertex shaders constant buffer
	result = device->CreateBuffer(&matrixBufferDescription, nullptr, &m_matrixBuffer);

	if (FAILED(result))
	{
		m_initializationFailed = true;
		return;
	}
}

Shader::Shader(const Shader& other) = default;

Shader::Shader(Shader&& other) noexcept = default;

Shader::~Shader() {
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = nullptr;
	}

	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = nullptr;
	}

	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = nullptr;
	}

	if (m_pixelShaderBuffer)
	{
		m_pixelShaderBuffer->Release();
		m_pixelShaderBuffer = nullptr;
	}

	if (m_vertexShaderBuffer)
	{
		m_vertexShaderBuffer->Release();
		m_vertexShaderBuffer = nullptr;
	}
}

Shader& Shader::operator=(const Shader& other) = default;

Shader& Shader::operator=(Shader&& other) noexcept = default;

bool Shader::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix) {
	
	//Lock matrix constant buffer and set the transposed matrices to it
	const auto result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &m_mappedResource);

	if (FAILED(result))
	{
		return false;
	}

	auto* matrixBufferDataPointer = static_cast<MatrixBufferType*>(m_mappedResource.pData);

	matrixBufferDataPointer->worldMatrix = XMMatrixTranspose(worldMatrix);
	matrixBufferDataPointer->viewMatrix = XMMatrixTranspose(viewMatrix);
	matrixBufferDataPointer->projectionMatrix = XMMatrixTranspose(projectionMatrix);

	matrixBufferDataPointer = nullptr;

	//Unlock constant buffer
	deviceContext->Unmap(m_matrixBuffer, 0);

	//Set the updated constant buffer
	const unsigned int bufferNumber = 0;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}

void Shader::RenderShader(ID3D11DeviceContext* deviceContext) const {
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader, nullptr, 0);
}

void Shader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, LPCSTR shaderFileName) {

	ofstream out;

	//pointer to the error message
	auto* compileErrors = static_cast<char*>(errorMessage->GetBufferPointer());

	//length of error message
	const auto bufferSize = errorMessage->GetBufferSize();

	//Write error to text file
	out.open("shader-error.txt");

	for (unsigned long i = 0; i < bufferSize; i++)
	{
		out << compileErrors[i];
	}

	out.close();

	//Release resources
	errorMessage->Release();
	errorMessage = nullptr;

	MessageBox(hwnd, "Error Compiling Shader. Check shader-error.txt for message.", shaderFileName, MB_OK);
}

bool Shader::GetInitializationState() const {
	return m_initializationFailed;
}
