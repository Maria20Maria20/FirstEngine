#include "SpotLight.h"

SpotLight::SpotLight(ID3D11Device* device, Vector3 position,
    float range, Vector3 direction, float spot, Vector3 att,
    Vector4 ambient, Vector4 diffuse, Vector4 specular)
{
    this->device = device;
    this->ambient;

    if (att.z < 0.0001) {
        float c = fmax(fmax(diffuse.x, diffuse.y), diffuse.z) / att.y;
        range = max(range, (256.0f * c)); // range = max(range, (8.0f * sqrtf(c) + 1.0f));
    }
    else {
        float c = fmax(fmax(diffuse.x, diffuse.y), diffuse.z) / att.z;
        range = max(range, (16.0f * sqrtf(c) + 1.0f)); // range = max(range, (8.0f * sqrtf(c) + 1.0f));
    }

    spotLightData = {
        diffuse, specular, position, range,
        direction, spot,
        att, 0.0f
    };

    //float coneAngle = acosf(powf(256.0f, -1.0f / spot));
    //width = 2 * range * sinf(coneAngle);
    float coneAngle = acosf(powf(128.0f, -1.0f / spot));
    width = range * sinf(coneAngle);
    depth = range;
    
    // LightPass
    {
        // Topology
        topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

        // VertexBuffer
        CreateCubeVertexBuffer();
        // IndexBuffer
        CreateCubeIndexBuffer();

        // VertexShader
        auto res = D3DCompileFromFile(L"./Shaders/LightPass/SpotLightVShader.hlsl", //create vertex shader from  hlsl file
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
                std::cout << L"./Shaders/LightPass/SpotLightVShader.hlsl" << L" - Missing Shader File\n";
            }
        }
        device->CreateVertexShader(
            vsBlob->GetBufferPointer(),
            vsBlob->GetBufferSize(),
            nullptr, &pVertexShader);

        numInputElements = 1;
        IALayoutInputElements = (D3D11_INPUT_ELEMENT_DESC*)malloc(numInputElements * sizeof(D3D11_INPUT_ELEMENT_DESC));

        IALayoutInputElements[0] =
            D3D11_INPUT_ELEMENT_DESC{
                "POSITION",
                0,
                DXGI_FORMAT_R32G32B32_FLOAT,
                0,
                0,
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
        res = D3DCompileFromFile(L"./Shaders/LightPass/SpotLightPShader.hlsl",
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
                std::cout << L"./Shaders/LightPass/SpotLightPShader.hlsl" << L" - Missing Shader File\n";
            }

            return;
        }
        device->CreatePixelShader(
            psBlob->GetBufferPointer(),
            psBlob->GetBufferSize(),
            nullptr, &pPixelShader);


        // PixelConstantBuffer
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.Usage = D3D11_USAGE_DYNAMIC;
        cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        cbd.MiscFlags = 0u;
        cbd.ByteWidth = sizeof(SpotLightPCB) + (16 - (sizeof(SpotLightPCB) % 16));  // aligned size
        cbd.StructureByteStride = 0u;

        D3D11_SUBRESOURCE_DATA InitData = {};
        InitData.pSysMem = &spotLightData;
        device->CreateBuffer(&cbd, &InitData, &pSpotLightPBuffer);

        // Rasterizer
        D3D11_RASTERIZER_DESC rastDesc = GetRasterizerDesc();
        device->CreateRasterizerState(&rastDesc, &pRasterizer);

        // DepthStencilState
        D3D11_DEPTH_STENCIL_DESC depthStencilDesc = GetDepthStencilDesc();
        device->CreateDepthStencilState(&depthStencilDesc, &pDepthStencilState);
    }
}

D3D11_DEPTH_STENCIL_DESC SpotLight::GetDepthStencilDesc()
{
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = D3D11_COMPARISON_GREATER;
    return dsDesc;
}

D3D11_RASTERIZER_DESC SpotLight::GetRasterizerDesc()
{
    D3D11_RASTERIZER_DESC rasterDesc = {};
    rasterDesc.CullMode = D3D11_CULL_NONE;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    return rasterDesc;
}

void SpotLight::Update(float deltaTime) {
    mWorldMatrix =  Matrix::CreateScale(0.5 * width, 0.5 * width, 0.5 * depth)
        * Matrix::CreateTranslation(0,0,depth * 0.5)
        * Matrix::CreateFromQuaternion(Quaternion::FromToRotation({0,0,1}, spotLightData.Direction))
        * Matrix::CreateTranslation(spotLightData.Position);
    return;
}

void SpotLight::UpdateBuffers(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
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

    context->Map(pSpotLightPBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource);
    memcpy(mappedResource.pData, &spotLightData, sizeof(spotLightData) + (16 - (sizeof(spotLightData) % 16))); // aligned size
    context->Unmap(pSpotLightPBuffer.Get(), 0);
}

void SpotLight::applyGBufferPass(ID3D11DeviceContext* context)
{
}

void SpotLight::applyLightPass(ID3D11DeviceContext* context)
{
    UpdateBuffers(context);

    context->IASetPrimitiveTopology(topology);
    UINT stride = sizeof(Vertex);
    const UINT offset = 0u;
    context->IASetVertexBuffers(0u, 1u, &mVertexBuffer, &stride, &offset);
    context->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    context->VSSetShader(pVertexShader.Get(), nullptr, 0u);
    context->IASetInputLayout(pInputLayout.Get());

    context->VSSetConstantBuffers(0u, 1u, pTransformConstantBuffer.GetAddressOf());

    context->PSSetShader(pPixelShader.Get(), nullptr, 0u);
    context->PSSetConstantBuffers(1u, 1u, pSpotLightPBuffer.GetAddressOf());

    context->RSSetState(pRasterizer.Get());

    context->OMSetDepthStencilState(pDepthStencilState.Get(), 1u);

    context->DrawIndexed(indicesNum, 0, 0);
}
