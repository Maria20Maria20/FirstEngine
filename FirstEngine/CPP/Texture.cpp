#include "Texture.h"

Texture::Texture(ID3D11Device* device, ID3D11ShaderResourceView* pTextureView)
{
	this->pTexture = nullptr;
	this->pTextureView = pTextureView;
}

Texture::Texture(ID3D11Device* device, const ColorHelper& color, aiTextureType type)
{
	this->Initialize1x1ColorTexture(device, color, type);
}

Texture::Texture(ID3D11Device* device, const ColorHelper* colorData, UINT width, UINT height, aiTextureType type)
{
	this->InitializeColorTexture(device, colorData, width, height, type);
}

Texture::Texture(ID3D11Device* device, const std::string& filePath, aiTextureType type)
{
	this->type = type;
	if (StringHelper::GetFileExtension(filePath) == "dds")
	{
		HRESULT hr = DirectX::CreateDDSTextureFromFile(device, StringHelper::StringToWide(filePath).c_str(), &pTexture, GetTextureResourceViewAddress());
		if (FAILED(hr))
		{
			std::cout << "DDS loading error!!! " << filePath << " :: " << StringHelper::GetFileExtension(filePath) << "\n";
			this->Initialize1x1ColorTexture(device, ColorHelpers::UnloadedTextureColor, type);
		}
		return;
	}
	else
	{
		std::cout << "Wrong texture file extension: " << StringHelper::GetFileExtension(filePath) << "\n";
		this->Initialize1x1ColorTexture(device, ColorHelpers::UnloadedTextureColor, type);
	}
}

aiTextureType Texture::GetType()
{
	return this->type;
}

ID3D11ShaderResourceView* Texture::GetTextureResourceView()
{
	return this->pTextureView;
}

ID3D11ShaderResourceView** Texture::GetTextureResourceViewAddress()
{
	return &(this->pTextureView);
}

void Texture::Initialize1x1ColorTexture(ID3D11Device* device, const ColorHelper& colorData, aiTextureType type)
{
	InitializeColorTexture(device, &colorData, 1, 1, type);
}

void Texture::InitializeColorTexture(ID3D11Device* device, const ColorHelper* colorData, UINT width, UINT height, aiTextureType type)
{
	this->type = type;

	CD3D11_TEXTURE2D_DESC textureDesc(DXGI_FORMAT_R8G8B8A8_UNORM, width, height);

	D3D11_SUBRESOURCE_DATA initialData{};
	initialData.pSysMem = colorData;
	initialData.SysMemPitch = width * sizeof(ColorHelper);

	ID3D11Texture2D* p2DTexture = nullptr;
	HRESULT hr = device->CreateTexture2D(&textureDesc, &initialData, &p2DTexture);
	if (FAILED(hr)) {
		throw std::runtime_error("Failed to initialize texture from color data.");
	}

	pTexture = static_cast<ID3D11Texture2D*>(p2DTexture);

	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, textureDesc.Format);

	hr = device->CreateShaderResourceView(pTexture, &srvDesc, &pTextureView);
	if (FAILED(hr)) {
		throw std::runtime_error("Failed to create shader resource view from texture generated from color data.");
	}
}