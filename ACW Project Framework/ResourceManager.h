#pragma once
#include <map>
#include <memory>
#include <fstream>
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include <random>

#include "DDSTextureLoader.h"

using namespace std;
using namespace DirectX;

class ResourceManager
{
public:
	ResourceManager(ID3D11Device* device);
	ResourceManager(const ResourceManager& other); // Copy Constructor
	//ResourceManager(ResourceManager&& other) noexcept; // Move Constructor
	~ResourceManager();

	ResourceManager& operator = (const ResourceManager& other); // Copy Assignment Operator
	ResourceManager& operator = (ResourceManager&& other) noexcept; // Move Assignment Operator

	bool GetModel(ID3D11Device* device, const char* modelFileName, ID3D11Buffer* &vertexBuffer, ID3D11Buffer* &indexBuffer);
	bool GetTexture(ID3D11Device* device, const WCHAR* textureFileName, ID3D11ShaderResourceView* &texture);

	int GetSizeOfVertexType() const;
	int GetIndexCount(const char* modelFileName) const;

	void ReturnRandomTexture(ID3D11ShaderResourceView* &texture);

private:
	bool LoadModel(ID3D11Device* device, const char* modelFileName);
	bool LoadTexture(ID3D11Device* device, const WCHAR* textureFileName);

	struct VertexType {
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

	map<const char*, int> m_indexCount;

	map<const char*, ID3D11Buffer*> m_vertexBuffers;
	map<const char*, ID3D11Buffer*> m_indexBuffers;

	map<const WCHAR*, ID3D11ShaderResourceView*> m_textures;
};

