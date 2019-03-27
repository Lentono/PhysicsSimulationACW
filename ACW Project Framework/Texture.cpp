#include "Texture.h"

Texture::Texture(ID3D11Device* device, const WCHAR* fileName, ResourceManager* resourceManager) : m_texture(nullptr), m_initializationFailed(false)
{
	m_resourceManager = resourceManager;
	const auto result = resourceManager->GetTexture(device, fileName, m_texture);

	if (FAILED(result))
	{
		m_initializationFailed = true;
	}
}

Texture::Texture(const Texture& other) = default;

Texture::Texture(Texture&& other) = default;

Texture::~Texture()
{
	//Release resources

	if (m_texture)
	{
		m_texture->Release();
		m_texture = nullptr;
	}
}

Texture& Texture::operator=(const Texture& other) = default;

Texture& Texture::operator=(Texture&& other) noexcept = default;

ID3D11ShaderResourceView* Texture::GetTexture() const {
	return m_texture;
}

void Texture::ChangeRandomTexture() {
	m_resourceManager->ReturnRandomTexture(m_texture);
}


bool Texture::GetInitializationState() const {
	return m_initializationFailed;
}