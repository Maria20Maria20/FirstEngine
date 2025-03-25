#include "Texture.h"

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
		else
			std::cout << "DDS loaded!!! " << filePath << " :: " << StringHelper::GetFileExtension(filePath) << "\n";
		return;
	}
	else
	{
		std::cout << "Wrong texture file extension: " << StringHelper::GetFileExtension(filePath) << "\n";
		this->Initialize1x1ColorTexture(device, ColorHelpers::UnloadedTextureColor, type);
		/*
		HRESULT hr = DirectX::CreateWICTextureFromFile(device, StringHelper::StringToWide(filePath).c_str(), *pTexture, GetTextureResourceViewAddress());
		if (FAILED(hr))
		{
			this->Initialize1x1ColorTexture(device, Colors::UnloadedTextureColor, type);
		}
		return;
		*/
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

	/*D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	*/
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
	/*D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//srvDesc.Texture2D.MipLevels = 1;*/

	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, textureDesc.Format);

	hr = device->CreateShaderResourceView(pTexture, &srvDesc, &pTextureView);
	if (FAILED(hr)) {
		throw std::runtime_error("Failed to create shader resource view from texture generated from color data.");
	}
}

/*
Texture::Texture(ID3D11Device* device)
{
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = 100; // s.GetWidth();
	textureDesc.Height = 100; // s.GetHeight();
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA sd = {};
	//sd.pSysMem = s.GetBufferPtr();
	//sd.SysMemPitch = s.GetWidth() * sizeof(XMFLOAT4(0,0,0,1)); // * sizeof(Surface::Color);
	ID3D11Texture2D* p2DTexture;
	device->CreateTexture2D(&textureDesc, &sd, &p2DTexture);

	pTexture = static_cast<ID3D11Texture2D*>(p2DTexture);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	device->CreateShaderResourceView(pTexture, &srvDesc, &pTextureView);
}

void Texture::Bind(ID3D11DeviceContext* context)
{
	context->PSGetShaderResources(0u, 1u, &pTextureView);
}

*/