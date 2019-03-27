#include "Model.h"

Model::Model(ID3D11Device* device, ModelType modelType, ResourceManager* resourceManager) : m_initializationFailed(false), m_sizeOfVertexType(0), m_modelType(modelType), m_vertexBuffer(nullptr), m_indexBuffer(nullptr)
{
	const auto* modelFileName = "";

	switch (modelType)
	{
		case Sphere:
			modelFileName = "sphere.obj";
			break;
		case Cube:
			modelFileName = "cube.obj";
			break;
		case Plane:
			modelFileName = "plane.obj";
			break;
		case Cylinder:
			modelFileName = "cylinder.obj";
			break;
		default:
			m_initializationFailed = true;
			return;
	}

	const auto result = resourceManager->GetModel(device, modelFileName, m_vertexBuffer, m_indexBuffer);

	if (!result)
	{
		m_initializationFailed = true;
		return;
	}

	m_sizeOfVertexType = resourceManager->GetSizeOfVertexType();
	m_indexCount = resourceManager->GetIndexCount(modelFileName);
}

Model::Model(const Model& other) = default;

Model::Model(Model&& other) noexcept = default;

Model::~Model()
{
	//Release resources

	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}

	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}
}

Model& Model::operator=(const Model& other) = default;

Model& Model::operator=(Model&& other) noexcept = default;

void Model::Render(ID3D11DeviceContext* deviceContext) {
	
	//Render buffers

	//Set vertex buffer stride and offset
	unsigned int stride = m_sizeOfVertexType;
	unsigned int offset = 0;

	//Set the vertex buffer to active in the input assembler so it will render it
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	//Set the index buffer to active in the input assembler so it will render it
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set the type of primitive render style for the vertex buffer
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool Model::GetInitializationState() const {
	return m_initializationFailed;
}

int Model::GetIndexCount() const {
	return m_indexCount;
}

Model::ModelType Model::GetModelType() const
{
	return m_modelType;
}


