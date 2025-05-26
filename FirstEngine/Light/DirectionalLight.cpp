#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(ID3D11Device* device, Vector3 position,
    Vector3 direction, Vector4 ambient,
    Vector4 diffuse, Vector4 specular)
{
    this->device = device;
    direction.Normalize();
    this->ambient = ambient;
    directionalLightData = {
        diffuse, specular, position, 0.0f,
        direction, 0.0f
    };

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
        auto res = D3DCompileFromFile(L"./Shaders/LightPass/DirectionalLightVShader.hlsl", //create vertex shader from  hlsl file
            nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
            "VSMain", "vs_5_0",
            D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
            &vsBlob, &errorVertexCode);
        if (FAILED(res)) {
            // If the shader failed to compile it should have written something to the error message.
            if (errorVertexCode) {
                char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());

                std::cout << compileErrors << std::endl;
            }
            // If there was  nothing in the error message then it simply could not find the shader file itself.
            else
            {
                std::cout << L"./Shaders/LightPass/DirectionalLightVShader.hlsl" << L" - Missing Shader File\n";
            }
        }

        device->CreateVertexShader(
            vsBlob->GetBufferPointer(),
            vsBlob->GetBufferSize(),
            nullptr, &pVertexShader);


        // PixelShader
        res = D3DCompileFromFile(L"./Shaders/LightPass/DirectionalLightPShader.hlsl",
            nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
            "PSMain", "ps_5_0",
            D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
            &psBlob, &errorPixelCode);
        if (FAILED(res)) {
            // If the shader failed to compile it should have written something to the error message.
            if (errorPixelCode) {
                char* compileErrors = (char*)(errorPixelCode->GetBufferPointer());
                std::cout << compileErrors << " - // -- " << std::endl;
            }
            // If there was  nothing in the error message then it simply could not find the shader file itself.
            else
            {
                std::cout << L"./Shaders/LightPass/DirectionalLightPShader.hlsl" << L" - Missing Shader File\n";
            }

            return;
        }
        device->CreatePixelShader(
            psBlob->GetBufferPointer(),
            psBlob->GetBufferSize(),
            nullptr, &pPixelShader);


        // PixelConstantBuffer
        D3D11_BUFFER_DESC cbd;
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.Usage = D3D11_USAGE_DYNAMIC;
        cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        cbd.MiscFlags = 0u;
        cbd.ByteWidth = sizeof(directionalLightData) + (16 - (sizeof(directionalLightData) % 16));  // aligned size
        cbd.StructureByteStride = 0u;

        D3D11_SUBRESOURCE_DATA InitData = {};
        InitData.pSysMem = &directionalLightData;
        device->CreateBuffer(&cbd, &InitData, &pDirectionalLightPBuffer);

        // Rasterizer
        D3D11_RASTERIZER_DESC rastDesc = GetRasterizerDesc();
        device->CreateRasterizerState(&rastDesc, &pRasterizer);

        // DepthStencilState
        D3D11_DEPTH_STENCIL_DESC depthStencilDesc = GetDepthStencilDesc();
        device->CreateDepthStencilState(&depthStencilDesc, &pDepthStencilState);
    }

}

D3D11_DEPTH_STENCIL_DESC DirectionalLight::GetDepthStencilDesc()
{
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    return dsDesc;
}

D3D11_RASTERIZER_DESC DirectionalLight::GetRasterizerDesc()
{
    D3D11_RASTERIZER_DESC rasterDesc = {};
    rasterDesc.CullMode = D3D11_CULL_NONE;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    return rasterDesc;
}

void DirectionalLight::Update(float deltaTime)
{
}

void DirectionalLight::UpdateBuffers(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    context->Map(pDirectionalLightPBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource);
    memcpy(mappedResource.pData, &directionalLightData, sizeof(directionalLightData) + (16 - (sizeof(directionalLightData) % 16))); // aligned size
    context->Unmap(pDirectionalLightPBuffer.Get(), 0);
}

void DirectionalLight::applyGBufferPass(ID3D11DeviceContext* context)
{
}

void DirectionalLight::applyLightPass(ID3D11DeviceContext* context)
{
    context->IASetPrimitiveTopology(topology);
    context->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    context->VSSetShader(pVertexShader.Get(), nullptr, 0u);

    UpdateBuffers(context);
    context->PSSetConstantBuffers(1u, 1u, pDirectionalLightPBuffer.GetAddressOf());

    context->PSSetShader(pPixelShader.Get(), nullptr, 0u);
    context->RSSetState(pRasterizer.Get());

    context->OMSetDepthStencilState(pDepthStencilState.Get(), 1u);

    context->DrawIndexed(indicesNum, 0, 0);
}
