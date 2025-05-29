#include "GBufferMaps.h"

GBufferMaps::GBufferMaps(ID3D11Device* device, ID3D11Texture2D* depthBuffer)
{
    this->device = device;
    this->depthBuffer = depthBuffer;
    if (depthBuffer)
    {
        D3D11_TEXTURE2D_DESC depthDesc = {};
        depthDesc.Width = 800;
        depthDesc.Height = 800;
        depthDesc.MipLevels = 1;
        depthDesc.ArraySize = 1;
        depthDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
        depthDesc.SampleDesc.Count = 1;
        depthDesc.SampleDesc.Quality = 0;
        depthDesc.Usage = D3D11_USAGE_DEFAULT;
        depthDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
        depthDesc.MiscFlags = 0;
        device->CreateTexture2D(&depthDesc, nullptr, depthBufferCopy.GetAddressOf());
        // Depth SRV
        D3D11_SHADER_RESOURCE_VIEW_DESC descSRV = {};
        descSRV.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        descSRV.Texture2DArray.MostDetailedMip = 0;
        descSRV.Texture2DArray.MipLevels = 1;
        descSRV.Texture2DArray.FirstArraySlice = 0;
        descSRV.Texture2DArray.ArraySize = 1;
        device->CreateShaderResourceView(depthBufferCopy.Get(), &descSRV, depthBufferCopySRV.GetAddressOf());
    }


    // LightPass
    {

        // Topology
        topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

        // IndexBuffer
        indices = (int*)calloc(4, sizeof(int));
        indices[0] = 0;
        indices[1] = 1;
        indices[2] = 2;
        indices[3] = 3;

        indicesNum = 4;
        CreateIndexBuffer();

        // VertexShader
        auto res = D3DCompileFromFile(L"./Shaders/LightPass/GBufferMapsVShader.hlsl",
            nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
            "VSMain", "vs_5_0",
            D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
            &vsBlob, &errorVertexCode);
        if (FAILED(res)) {
            if (errorVertexCode) {
                char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());

                std::cout << compileErrors << std::endl;
            }
            else
            {
                std::cout << L"./Shaders/LightPass/GBufferMapsVShader.hlsl" << L" - Missing Shader File\n";
            }
        }

        device->CreateVertexShader(
            vsBlob->GetBufferPointer(),
            vsBlob->GetBufferSize(),
            nullptr, &pVertexShader);


        // PixelShader
        res = D3DCompileFromFile(L"./Shaders/LightPass/GBufferMapsPShader.hlsl",
            nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
            "PSMain", "ps_5_0",
            D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
            &psBlob, &errorPixelCode);
        if (FAILED(res)) {
            if (errorPixelCode) {
                char* compileErrors = (char*)(errorPixelCode->GetBufferPointer());
                std::cout << compileErrors << " - // -- " << std::endl;
            }
            else
            {
                std::cout << L"./Shaders/LightPass/GBufferMapsPShader.hlsl" << L" - Missing Shader File\n";
            }

            return;
        }
        device->CreatePixelShader(
            psBlob->GetBufferPointer(),
            psBlob->GetBufferSize(),
            nullptr, &pPixelShader);

        // Rasterizer
        D3D11_RASTERIZER_DESC rastDesc = GetRasterizerDesc();
        device->CreateRasterizerState(&rastDesc, &pRasterizer);

        // DepthStencilState
        D3D11_DEPTH_STENCIL_DESC depthStencilDesc = GetDepthStencilDesc();
        device->CreateDepthStencilState(&depthStencilDesc, &pDepthStencilState);

        // BlendState
        D3D11_BLEND_DESC blendDesc = CD3D11_BLEND_DESC(CD3D11_DEFAULT{});
        blendDesc.RenderTarget[0].BlendEnable = TRUE;
        blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        res = device->CreateBlendState(&blendDesc, pBlendState.GetAddressOf());


    }
}

D3D11_DEPTH_STENCIL_DESC GBufferMaps::GetDepthStencilDesc()
{
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    return dsDesc;
}

D3D11_RASTERIZER_DESC GBufferMaps::GetRasterizerDesc()
{
    D3D11_RASTERIZER_DESC rasterDesc = {};
    rasterDesc.CullMode = D3D11_CULL_NONE;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    return rasterDesc;
}

void GBufferMaps::Update(float deltaTime)
{
}

void GBufferMaps::UpdateBuffers(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
    if (depthBuffer)
    {
        /*
        D3D11_MAPPED_SUBRESOURCE mappedResourceRead;
        context->Map(depthBuffer.Get(), 0u, D3D11_MAP_READ, 0u, &mappedResourceRead);
        D3D11_MAPPED_SUBRESOURCE mappedResourceWrite;
        context->Map(depthBufferCopy.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResourceWrite);
        memcpy(&mappedResourceWrite, &mappedResourceRead, 8 * 800 * 800);
        context->Unmap(depthBuffer.Get(), 0u);
        context->Unmap(depthBufferCopy.Get(), 0u);
        */
        context->CopyResource(depthBufferCopy.Get(), depthBuffer.Get());

        //context->PSSetShaderResources(4u, 1u, depthBufferCopySRV.GetAddressOf());
    }
}

void GBufferMaps::applyGBufferPass(ID3D11DeviceContext* context)
{
}

void GBufferMaps::applyLightPass(ID3D11DeviceContext* context)
{
    context->IASetPrimitiveTopology(topology);
    context->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    context->VSSetShader(pVertexShader.Get(), nullptr, 0u);

    context->RSSetState(pRasterizer.Get());
    
    context->PSSetShader(pPixelShader.Get(), nullptr, 0u);
    
    if (depthBuffer)
    {
        context->PSSetShaderResources(4u, 1u, depthBufferCopySRV.GetAddressOf());
    }

    context->OMSetDepthStencilState(pDepthStencilState.Get(), 1u);

    FLOAT blendFactor[4] = { 1, 1, 1, 1 };
    UINT sampleMask = 0xffffffff;
    context->OMSetBlendState(pBlendState.Get(), blendFactor, sampleMask);

    context->DrawIndexed(indicesNum, 0, 0);
}
