#include "LightPass.h"

LightPass::LightPass(ID3D11Device* device, ID3D11DeviceContext* context,
	ID3D11Texture2D* backBuffer,
	UINT screenWidth, UINT screenHeight, GBuffer* pGBuffer, Camera* camera)
	:
	RenderPass(device, context)
{
	this->backBuffer = backBuffer;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	this->pGBuffer = pGBuffer;
	this->camera = camera;

	// rtv
	HRESULT hr = device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
	if (FAILED(hr))
		throw std::runtime_error("Failed to create Render Target View");

	// Viewport
	viewport = {};
	viewport.Width = static_cast<float>(screenWidth);
	viewport.Height = static_cast<float>(screenHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	pNormalMap = new Texture(device, pGBuffer->pNormalSRV.Get());
	pAlbedoMap = new Texture(device, pGBuffer->pAlbedoSRV.Get());
	pSpecularMap = new Texture(device, pGBuffer->pSpecularSRV.Get());
	pWorldPosMap = new Texture(device, pGBuffer->pWorldPosSRV.Get());


	// Usual sampler for all SRV
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&samplerDesc, &pSampler);


	{
		cameraData = { XMMatrixIdentity(), XMMatrixIdentity(),
			(XMFLOAT3)camera->GetPosition(), 0 };

		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(cameraData) + (16 - (sizeof(cameraData) % 16));  // aligned size
		cbd.StructureByteStride = 0u;

		D3D11_SUBRESOURCE_DATA InitData = {};
		InitData.pSysMem = &cameraData;
		device->CreateBuffer(&cbd, &InitData, &pCameraDataPixelConstantConstantBuffer);

	}

	/*
	{
		D3D11_DEPTH_STENCIL_DESC dsDesc = {};
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		dsDesc.DepthFunc = D3D11_COMPARISON_GREATER;
		device->CreateDepthStencilState(&dsDesc, &pDepthStencilState);
	}

	{
		D3D11_RASTERIZER_DESC rasterDesc = {};
		rasterDesc.CullMode = D3D11_CULL_NONE;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		device->CreateRasterizerState(&rasterDesc, &pRasterizer);
	}
	*/

	{
		D3D11_BLEND_DESC blendDesc = {};
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		device->CreateBlendState(&blendDesc, &pBlendState);
	}


}

void LightPass::StartFrame()
{
	context->OMSetRenderTargets(1, &renderTargetView, pGBuffer->pDepthDSV.Get());
	float colorBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	context->ClearRenderTargetView(renderTargetView, colorBlack);
	//context->ClearDepthStencilView(pGBuffer->pDepthDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
	context->RSSetViewports(1, &viewport);

	XMFLOAT3 camPos = camera->GetPosition();
	XMMATRIX vMatInverse = DirectX::XMMatrixTranspose(XMMatrixInverse(nullptr,
		camera->GetViewMatrix()));
	XMMATRIX pMatInverse = DirectX::XMMatrixTranspose(XMMatrixInverse(nullptr,
		camera->GetProjectionMatrix()));
	
	cameraData = { vMatInverse, pMatInverse, camPos, 0 };
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	context->Map(pCameraDataPixelConstantConstantBuffer, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource);
	memcpy(mappedResource.pData, &cameraData, sizeof(cameraData) + (16 - (sizeof(cameraData) % 16))); // aligned size
	context->Unmap(pCameraDataPixelConstantConstantBuffer, 0);
	context->PSSetConstantBuffers(0u, 1u, &pCameraDataPixelConstantConstantBuffer);

	context->PSSetShaderResources(0u, 1, pNormalMap->GetTextureResourceViewAddress());
	context->PSSetShaderResources(1u, 1, pAlbedoMap->GetTextureResourceViewAddress());
	context->PSSetShaderResources(2u, 1, pSpecularMap->GetTextureResourceViewAddress());
	context->PSSetShaderResources(3u, 1, pWorldPosMap->GetTextureResourceViewAddress());
	context->PSSetSamplers(0u, 1u, pSampler.GetAddressOf());
	/*
	context->RSSetState(pRasterizer.Get());
	context->OMSetDepthStencilState(pDepthStencilState.Get(), 1u);
	*/
	FLOAT blendFactor[4] = { 1, 1, 1, 1 };
	UINT sampleMask = 0xffffffff;
	context->OMSetBlendState(pBlendState.Get(), blendFactor, sampleMask);
}

void LightPass::Pass(const Scene& scene)
{

	for (GameObject* gameObject : scene.gameObjects) {
		gameObject->applyLightPass(context.Get());
	}

	ID3D11ShaderResourceView* nullSRVs[] = { nullptr, nullptr, nullptr, nullptr };
	context->PSSetShaderResources(0, 4, nullSRVs);

	for (auto ps : particleSystems) {
		ps->Render();
	}

}

void LightPass::EndFrame()
{

	context->RSSetState(nullptr);
	context->OMSetDepthStencilState(nullptr, 0);

	ID3D11RenderTargetView* nullRTVs[] = { nullptr };
	context->OMSetRenderTargets(1, nullRTVs, nullptr);
	context->ClearState();
}

Camera* LightPass::GetCamera()
{
	return camera;
}

void LightPass::SetCamera(Camera* camera)
{
	this->camera = camera;
}
