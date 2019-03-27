#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <fstream>

#include "Texture.h"
#include "ResourceManager.h"

using namespace DirectX;
using namespace std;

class Model
{
public:

	enum ModelType
	{
		Sphere,
		Cube,
		Plane,
		Cylinder
	};

	Model(ID3D11Device* device, ModelType modelType, ResourceManager* resourceManager);

	Model(const Model& other); // Copy Constructor
	Model(Model && other) noexcept; // Move Constructor
	~Model(); // Destructor

	Model& operator = (const Model& other); // Copy Assignment Operator
	Model& operator = (Model&& other) noexcept; // Move Assignment Operator

	void Render(ID3D11DeviceContext* deviceContext);

	bool GetInitializationState() const;
	int GetIndexCount() const;
	ModelType GetModelType() const;

private:

	bool m_initializationFailed;

	int m_sizeOfVertexType;

	int m_indexCount;

	ModelType m_modelType;

	ID3D11Buffer *m_vertexBuffer;
	ID3D11Buffer *m_indexBuffer;
};