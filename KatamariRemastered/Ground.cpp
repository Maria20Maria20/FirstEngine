#include "Ground.h"


Ground::Ground(ID3D11Device* device, XMFLOAT3 startPosition, float changedScale)
{
    this->device = device;
    this->position = startPosition;
    this->changedScale = changedScale;

    {
        // Topology
        topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        
        CreateRandomHeightPlane(50.0f, 50.0f, 10, 10, 0.05f,
            DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), &vertices, &verticesNum, &indices, &indicesNum);

        // VertexBuffer
        D3D11_BUFFER_DESC vbd = {};
        vbd.ByteWidth = sizeof(Vertex) * verticesNum;
        vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbd.Usage = D3D11_USAGE_DEFAULT; //how often does writing and reading to the buffer occur (default = read/write from GPU)
        vbd.CPUAccessFlags = 0; //0 = CPU don't need, D3D11_CPU_ACCESS_WRITE = CPU need
        vbd.MiscFlags = 0; //optional parameters
        vbd.StructureByteStride = 0; //size per element in buffer structure

        D3D11_SUBRESOURCE_DATA vinitData = {};
        vinitData.pSysMem = vertices;
        vinitData.SysMemPitch = 0;
        vinitData.SysMemSlicePitch = 0;

        device->CreateBuffer(&vbd, &vinitData, &mVertexBuffer);

        // IndexBuffer
        D3D11_BUFFER_DESC ibd = {};
        ibd.ByteWidth = sizeof(UINT) * indicesNum;
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibd.Usage = D3D11_USAGE_DEFAULT; //how often does writing and reading to the buffer occur (default = read/write from GPU)
        ibd.CPUAccessFlags = 0; //0 = CPU don't need, 1 = CPU need
        ibd.MiscFlags = 0; //optional parameters
        ibd.StructureByteStride = 0; //size per element in buffer structure

        D3D11_SUBRESOURCE_DATA iinitData = {};
        iinitData.pSysMem = indices;
        iinitData.SysMemPitch = 0;
        iinitData.SysMemSlicePitch = 0;

        device->CreateBuffer(&ibd, &iinitData, &mIndexBuffer);

        // VertexShader
        auto res = D3DCompileFromFile(L"./Shaders/GroundGBufferShaderVS.hlsl", //create vertex shader from  hlsl file
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
                std::cout << L"./Shaders/GroundGBufferShaderVS.hlsl" << L" - Missing Shader File\n";
            }

        }
        device->CreateVertexShader(
            vsBlob->GetBufferPointer(),
            vsBlob->GetBufferSize(),
            nullptr, &pVertexShader);

        // InputLayout
        this->numInputElements = 4;
        this->IALayoutInputElements = (D3D11_INPUT_ELEMENT_DESC*)malloc(this->numInputElements * sizeof(D3D11_INPUT_ELEMENT_DESC));
        this->IALayoutInputElements[0] =
            D3D11_INPUT_ELEMENT_DESC{
                "POSITION",
                0,
                DXGI_FORMAT_R32G32B32A32_FLOAT,
                0,
                0,
                D3D11_INPUT_PER_VERTEX_DATA,
                0 };

        this->IALayoutInputElements[1] =
            D3D11_INPUT_ELEMENT_DESC{
                "COLOR",
                0,
                DXGI_FORMAT_R32G32B32A32_FLOAT,
                0,
                D3D11_APPEND_ALIGNED_ELEMENT,
                D3D11_INPUT_PER_VERTEX_DATA,
                0 };

        this->IALayoutInputElements[2] =
            D3D11_INPUT_ELEMENT_DESC{
                "TEXCOORD",
                0,
                DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT,
                0,
                D3D11_APPEND_ALIGNED_ELEMENT,
                D3D11_INPUT_PER_VERTEX_DATA,
                0 };
        this->IALayoutInputElements[3] =
            D3D11_INPUT_ELEMENT_DESC{
                "NORMAL",
                0,
                DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
                0,
                D3D11_APPEND_ALIGNED_ELEMENT,
                D3D11_INPUT_PER_VERTEX_DATA,
                0 };

        device->CreateInputLayout(
            this->IALayoutInputElements,
            this->numInputElements,
            vsBlob->GetBufferPointer(),
            vsBlob->GetBufferSize(),
            &pInputLayout);

        // TransformCBuffer
        D3D11_BUFFER_DESC cbd;
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.Usage = D3D11_USAGE_DYNAMIC;
        cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        cbd.MiscFlags = 0u;
        cbd.ByteWidth = sizeof(Transforms) + (16 - (sizeof(Transforms) % 16));  // aligned size
        cbd.StructureByteStride = 0u;

        device->CreateBuffer(&cbd, nullptr, &pTransformConstantBuffer);

        // PixelShader
        res = D3DCompileFromFile(L"./Shaders/GroundGBufferShaderPS.hlsl",
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
                std::cout << L"./Shaders/GroundGBufferShaderPS.hlsl" << L" - Missing Shader File\n";
            }

            return;
        }
        device->CreatePixelShader(
            psBlob->GetBufferPointer(),
            psBlob->GetBufferSize(),
            nullptr, &pPixelShader);

        // Texture
        std::string texture_name = "./Ground.dds";
        pTexture = new Texture(device, texture_name, aiTextureType_DIFFUSE);

        // Texture sampler
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
        device->CreateSamplerState(&samplerDesc, &pSampler);;


        // PixelConstantBuffer
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.Usage = D3D11_USAGE_DYNAMIC;
        cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        cbd.MiscFlags = 0u;
        cbd.ByteWidth = sizeof(GroundPixelConstantBuffer) + (16 - (sizeof(GroundPixelConstantBuffer) % 16));  // aligned size
        cbd.StructureByteStride = 0u;

        device->CreateBuffer(&cbd, nullptr, &pGroundPixelConstantBuffer);
        
        // Rasterizer
        D3D11_RASTERIZER_DESC rastDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
        rastDesc.CullMode = D3D11_CULL_BACK;
        rastDesc.FillMode = D3D11_FILL_SOLID;
        device->CreateRasterizerState(&rastDesc, &pRasterizer);

        // BlendState
        D3D11_BLEND_DESC blendDesc = CD3D11_BLEND_DESC(CD3D11_DEFAULT{});
        device->CreateBlendState(&blendDesc, &pBlendState);

        // DepthStencilState
        D3D11_DEPTH_STENCIL_DESC depthStencilDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT{});
        device->CreateDepthStencilState(&depthStencilDesc, &pDepthStencilState);
    }

}

void Ground::Update(float deltaTime)
{
	//RotateShape(rotationDirection, speedRotation, deltaTime);
	mWorldMatrix = XMMatrixScaling(changedScale, changedScale, changedScale) * XMMatrixTranslation(position.x, position.y, position.z);
}

void Ground::applyGBufferPass(ID3D11DeviceContext* context)
{
    context->IASetPrimitiveTopology(topology);
    UINT stride = sizeof(Vertex);
    const UINT offset = 0u;
    context->IASetVertexBuffers(0u, 1u, &mVertexBuffer, &stride, &offset);
    context->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    context->VSSetShader(pVertexShader.Get(), nullptr, 0u);
    context->IASetInputLayout(pInputLayout.Get());

    DirectX::XMMATRIX A = mWorldMatrix;
    A.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
    DirectX::XMVECTOR det = XMMatrixDeterminant(A);
    const auto wMatInvTranspose = DirectX::XMMatrixTranspose(XMMatrixInverse(&det, A));
    const Matrix vpMat = Matrix(camera->GetViewMatrix()) * Matrix(camera->GetProjectionMatrix());
    const Transforms tf = {
            mWorldMatrix, wMatInvTranspose,
            vpMat
    };
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    context->Map(pTransformConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource);
    memcpy(mappedResource.pData, &tf, sizeof(tf) + (16 - (sizeof(tf) % 16))); // aligned size
    context->Unmap(pTransformConstantBuffer.Get(), 0);
    context->VSSetConstantBuffers(0u, 1u, pTransformConstantBuffer.GetAddressOf());

    const GroundPixelConstantBuffer gpcb = { camera->GetPosition() };
    context->Map(pGroundPixelConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource);
    memcpy(mappedResource.pData, &gpcb, sizeof(gpcb) + (16 - (sizeof(gpcb) % 16))); // aligned size
    context->Unmap(pGroundPixelConstantBuffer.Get(), 0);
    context->VSSetConstantBuffers(1u, 1u, pGroundPixelConstantBuffer.GetAddressOf());

    context->PSSetShader(pPixelShader.Get(), nullptr, 0u);
    context->PSSetShaderResources(0u, 1, pTexture->GetTextureResourceViewAddress());
    context->PSSetSamplers(0u, 1u, pSampler.GetAddressOf());
    context->RSSetState(pRasterizer.Get());

    FLOAT blendFactor[4] = { 1, 1, 1, 1 };
    UINT sampleMask = 0xffffffff;
    context->OMSetBlendState(pBlendState.Get(), blendFactor, sampleMask);

    context->OMSetDepthStencilState(pDepthStencilState.Get(), 1u);

    context->DrawIndexed(indicesNum, 0, 0);
}

void Ground::applyLightPass(ID3D11DeviceContext* context)
{

}


