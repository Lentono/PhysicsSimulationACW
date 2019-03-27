#include "ResourceManager.h"

ResourceManager::ResourceManager(ID3D11Device* device)
{
	LoadTexture(device, L"texture1.dds");
	LoadTexture(device, L"texture2.dds");
	LoadTexture(device, L"texture3.dds");
	LoadTexture(device, L"texture4.dds");
	LoadTexture(device, L"texture5.dds");
	LoadTexture(device, L"texture6.dds");
	LoadTexture(device, L"texture7.dds");
	LoadTexture(device, L"texture8.dds");
	LoadTexture(device, L"texture9.dds");
	LoadTexture(device, L"texture10.dds");
}

ResourceManager::ResourceManager(const ResourceManager& other) = default;

//ResourceManager::ResourceManager(ResourceManager&& other) noexcept = default;

ResourceManager::~ResourceManager()
{
	
}

ResourceManager& ResourceManager::operator=(const ResourceManager& other) = default;

ResourceManager& ResourceManager::operator=(ResourceManager&& other) noexcept = default;

bool ResourceManager::GetModel(ID3D11Device* device, const char* modelFileName, ID3D11Buffer* &vertexBuffer, ID3D11Buffer* &indexBuffer)
{
	if (0 == m_vertexBuffers.count(modelFileName))
	{
		auto const result = LoadModel(device, modelFileName);

		if (!result)
		{
			return false;
		}
	}

	vertexBuffer = m_vertexBuffers.at(modelFileName);
	indexBuffer = m_indexBuffers.at(modelFileName);

	return true;
}

bool ResourceManager::GetTexture(ID3D11Device* device, const WCHAR* textureFileName, ID3D11ShaderResourceView* &texture)
{
	if (0 == m_textures.count(textureFileName))
	{
		auto const result = LoadTexture(device, textureFileName);

		if (!result)
		{
			return false;
		}
	}

	texture = m_textures.at(textureFileName);

	return true;
}

int ResourceManager::GetSizeOfVertexType() const {
	return sizeof(VertexType);
}

int ResourceManager::GetIndexCount(const char* modelFileName) const {
	return m_indexCount.at(modelFileName);
}

void ResourceManager::ReturnRandomTexture(ID3D11ShaderResourceView*& texture) {
	random_device rd;
	std::mt19937 rng(rd());
	uniform_int_distribution<int> uni(0, m_textures.size());

	const auto randomInt = uni(rng);

	auto count = 0;

	for (auto it = m_textures.begin(); it != m_textures.end(); ++it)
	{
		if (count == randomInt)
		{
			texture = it->second;
			return;
		}

		count++;
	}
}


bool ResourceManager::LoadModel(ID3D11Device* device, const char* modelFileName)
{
	//Load Model
	ifstream fin;
	char input;
	int i;

	//Open obj file
	fin.open(modelFileName);

	if (fin.fail())
	{
		return false;
	}

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	vector<XMFLOAT3> positions;
	vector<XMFLOAT2> textures;
	vector<XMFLOAT3> normals;

	char cmd[256] = { 0 };

	auto faceCount = 0;
	auto vertexCount = 0;
	auto indexCount = 0;

	VertexType* vertices = nullptr;
	unsigned long* indices = nullptr;

	while (!fin.eof())
	{
		float x, y, z;

		fin >> cmd;

		if (0 == strcmp(cmd, "faces"))
		{
			fin >> faceCount;
			vertices = new VertexType[faceCount * 3];

			if (!vertices)
			{
				return false;
			}

			vertexCount = faceCount * 3;

			indices = new unsigned long[faceCount * 3];

			if (!indices)
			{
				return false;
			}

			indexCount = faceCount * 3;
		}

		if (0 == strcmp(cmd, "v"))
		{
			fin >> x >> y >> z;
			positions.emplace_back(XMFLOAT3(x, y, z));
		}
		else if (0 == strcmp(cmd, "vn"))
		{
			fin >> x >> y >> z;
			normals.emplace_back(XMFLOAT3(x, y, z));
		}
		else if (0 == strcmp(cmd, "vt"))
		{
			fin >> x >> y >> z;
			textures.emplace_back(XMFLOAT2(x, y));
		}
		else if (0 == strcmp(cmd, "f"))
		{
			int value;
			auto count = 0;

			while (0 == strcmp(cmd, "f"))
			{

				for (auto i = 0; i < 3; i++)
				{
					fin >> value;
					vertices[count].position = (positions[value - 1]);
					fin.ignore();

					fin >> value;
					vertices[count].texture = (textures[value - 1]);
					fin.ignore();

					fin >> value;
					vertices[count].normal = (normals[value - 1]);
					fin.ignore();

					indices[count] = count;

					count++;
				}

				fin >> cmd;
			}
		}
	}

	//Initialize buffers
	D3D11_BUFFER_DESC vertexBufferDescription;
	D3D11_BUFFER_DESC indexBufferDescription;

	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_SUBRESOURCE_DATA indexData;

	//Initialize vertex and index descriptions and then create buffers
	vertexBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDescription.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDescription.CPUAccessFlags = 0;
	vertexBufferDescription.MiscFlags = 0;
	vertexBufferDescription.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	auto result = device->CreateBuffer(&vertexBufferDescription, &vertexData, &vertexBuffer);

	if (FAILED(result))
	{
		return false;
	}

	indexBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDescription.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDescription.CPUAccessFlags = 0;
	indexBufferDescription.MiscFlags = 0;
	indexBufferDescription.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexBufferDescription, &indexData, &indexBuffer);

	if (FAILED(result))
	{
		return false;
	}

	m_indexCount.insert(pair<const char*, int>(modelFileName, indexCount));

	m_vertexBuffers.insert(pair<const char*, ID3D11Buffer*>(modelFileName, vertexBuffer));
	m_indexBuffers.insert(pair<const char*, ID3D11Buffer*>(modelFileName, indexBuffer));

	//Release resources
	vertexBuffer = nullptr;
	indexBuffer = nullptr;

	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	return true;
}

bool ResourceManager::LoadTexture(ID3D11Device* device, const WCHAR* textureFileName)
{
	ID3D11ShaderResourceView* texture;

	const auto result = CreateDDSTextureFromFile(device, textureFileName, nullptr, &texture);

	if (SUCCEEDED(result))
	{
		m_textures.insert(pair<const WCHAR*, ID3D11ShaderResourceView*>(textureFileName, texture));
		texture = nullptr;

		return true;
	}
	else
	{
		texture = nullptr;
		return false;
	}
}