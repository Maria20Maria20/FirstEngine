#include "Cube.h"
#include <d3dcompiler.h>
#include <iostream>

using namespace DirectX;

Cube::Cube(ID3D11Device* device, ID3DBlob* vertexBC, ID3D11VertexShader* vs,
    ID3D11PixelShader* ps, ID3D11RenderTargetView* rtv)
    : mRotationAngle(0.0f), vsBlob(vertexBC), mVertexShader(vs), mPixelShader(ps), 
    renderTargetView(rtv)
{
    mWorldMatrix = XMMatrixIdentity();
    InitializeBuffers(device);
    InitializeShaders(device, vs, ps);
}

void Cube::InitializeBuffers(ID3D11Device* device)
{
    

    Vertex _vertices[8] = {
        XMFLOAT4(-0.1f, -0.1f, 0.4f, 1.0f), XMFLOAT4(+1.0f, +1.0f, +1.0f, 1.0f),
        XMFLOAT4(-0.1f, +0.1f, 0.4f, 1.0f), XMFLOAT4(+1.0f, +1.0f, +1.0f, 1.0f),
        XMFLOAT4(+0.1f, +0.1f, 0.4f, 1.0f), XMFLOAT4(+1.0f, +1.0f, +1.0f, 1.0f),
        XMFLOAT4(+0.1f, -0.1f, 0.4f, 1.0f), XMFLOAT4(+1.0f, +1.0f, +1.0f, 1.0f),
        XMFLOAT4(-0.1f, -0.1f, 0.6f, 1.0f), XMFLOAT4(+1.0f, +1.0f, +1.0f, 1.0f),
        XMFLOAT4(-0.1f, +0.1f, 0.6f, 1.0f), XMFLOAT4(+1.0f, +1.0f, +1.0f, 1.0f),
        XMFLOAT4(+0.1f, +0.1f, 0.6f, 1.0f), XMFLOAT4(+1.0f, +1.0f, +1.0f, 1.0f),
        XMFLOAT4(+0.1f, -0.1f, 0.6f, 1.0f), XMFLOAT4(+1.0f, +1.0f, +1.0f, 1.0f),
    };
    for (size_t i = 0; i < 8; i++)
    {
        vertices[i] = _vertices[i];
    }

    D3D11_BUFFER_DESC vbd = {};
    vbd.ByteWidth = sizeof(Vertex) * 8;
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

    // Индексный буфер (как в BoxDemo)
    UINT _indices[36] = {
        // Front Face
        0, 1, 2,
        0, 2, 3,

        // Back Face
        4, 6, 5,
        4, 7, 6,

        // Left Face
        4, 5, 1,
        4, 1, 0,

        // Right Face
        3, 2, 6,
        3, 6, 7,

        // Top Face
        1, 5, 6,
        1, 6, 2,

        // Bottom Face
        4, 0, 3,
        4, 3, 7
    };

    for (size_t i = 0; i < 36; i++)
    {
        indices[i] = _indices[i];
    }

    D3D11_BUFFER_DESC ibd = {};
    ibd.ByteWidth = sizeof(UINT) * 36;
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

    D3D11_BUFFER_DESC constantBufferDesc = {};
    constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC; //0 = CPU don't need, D3D11_USAGE_DYNAMIC = CPU need
    constantBufferDesc.ByteWidth = sizeof(ConstantBuffer);
    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    constantBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = &cb;
    device->CreateBuffer(&constantBufferDesc, &initData, &mConstantBuffer);

    D3D11_INPUT_ELEMENT_DESC inputElements[] = {
    D3D11_INPUT_ELEMENT_DESC{
        "POSITION", //parameter name from hlsl file
        0, //need if we have more one element with same semantic
        DXGI_FORMAT_R32G32B32A32_FLOAT, //parameter for create 3D object
        0, //vertex index (between 0 and 15)
        0, //translation from beginning vertex
        D3D11_INPUT_PER_VERTEX_DATA, //class input data for input slot (for each vertex or instance)
        0 },
        D3D11_INPUT_ELEMENT_DESC{
        "COLOR",
        0,
        DXGI_FORMAT_R32G32B32A32_FLOAT,
        0,
        D3D11_APPEND_ALIGNED_ELEMENT,
        D3D11_INPUT_PER_VERTEX_DATA,
        0 }
    };

    device->CreateInputLayout(
        inputElements,
        2,
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        &mInputLayout);
}

void Cube::InitializeShaders(ID3D11Device* device, ID3D11VertexShader* vs, ID3D11PixelShader* ps)
{
    auto res = D3DCompileFromFile(L"./Shaders/CubeShader.hlsl", //create vertex shader from  hlsl file
        nullptr /*macros*/,
        nullptr /*include*/,
        "VSMain", //function name from hlsl file for run it
        "vs_5_0", //shader target - vertex shader (vs)
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, //options for compile shader
        0, //parameters for compile effects (if 0, then don't compile them)
        &vsBlob,
        &errorVertexCode);

    D3D_SHADER_MACRO Shader_Macros[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr };
    //"TEST" - use TEST define from hlsl file
    //float4(0.0f, 1.0f, 0.0f, 1.0f) - color for square right

    res = D3DCompileFromFile(L"./Shaders/CubeShader.hlsl", //create pixel shaders from hlsl file
        Shader_Macros /*macros*/, //macros shaders
        nullptr /*include*/,
        "PSMain", //function name from hlsl file for run it
        "ps_5_0", //shader target - pixel shader (ps)
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, //options for compile shader
        0, //parameters for compile effects (if 0, then don't compile them)
        &psBlob,
        &errorPixelCode);

    device->CreateVertexShader(
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        nullptr, &mVertexShader);

    device->CreatePixelShader(
        psBlob->GetBufferPointer(),
        psBlob->GetBufferSize(),
        nullptr, &mPixelShader);
}

void Cube::Update(float dt)
{
    //mRotationAngle += 1.0f * dt;
    //auto rotation = XMMatrixRotationX(1.); 
    //mWorldMatrix = rotation * XMMatrixRotationY(mRotationAngle);

    RotateShape(DirectX::XMVectorSet(0, 1, 0, 1), .1, dt);
    RotateShape(DirectX::XMVectorSet(1, 0, 0, 1), .1, dt);
    //mWorldMatrix = mWorldMatrix * DirectX::XMMatrixTranslation(0, 0, 0.3);
}

void Cube::Draw(ID3D11DeviceContext* context, const XMMATRIX& viewProj)
{
    for (size_t i = 0; i < 4; i++)
    {
        std::cout << vertices[2*i].position.x << " " << vertices[2*i].position.y << " " << vertices[2 * i].position.z << " ";
    }
    std::cout <<  "\n";

    // Установка вершинного буфера
    UINT stride[] = { 32 };
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &mVertexBuffer, stride, &offset);
    context->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->IASetInputLayout(mInputLayout);

    //context->RSSetState(rastState);


    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(800); // screenWidth
    viewport.Height = static_cast<float>(800); // screenHeight
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1.0f;

    context->RSSetViewports(1, &viewport);

    // Установка шейдеров и ресурсов
    context->VSSetShader(mVertexShader, nullptr, 0);
    context->PSSetShader(mPixelShader, nullptr, 0);
    
    context->OMSetRenderTargets(1, &renderTargetView, nullptr);

    // Обновление константного буфера
    //cb.worldViewProj = XMMatrixTranspose(mWorldMatrix * viewProj);
    cb.worldViewProj = mWorldMatrix * viewProj;
    //cb.worldViewProj = XMMatrixIdentity();

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    context->Map(mConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    memcpy(mappedResource.pData, &cb, sizeof(ConstantBuffer));
    context->Unmap(mConstantBuffer, 0);

    context->VSSetConstantBuffers(0, 1, &mConstantBuffer);

    // Отрисовка
    context->DrawIndexed(36, 0, 0);
}
void Cube::RotateShape(FXMVECTOR Axis, FLOAT Angle, float deltaTime)
{
    XMVECTOR normalizedAxis = XMVector3Normalize(Axis); // Используем новую переменную

    // Проверяем, что вектор не нулевой
    if (XMVector3Equal(normalizedAxis, XMVectorZero()))
    {
        return; // Если ось некорректна, не выполняем вращение
    }
    XMMATRIX rotateMatrix = XMMatrixRotationAxis(normalizedAxis, XMConvertToRadians(Angle));
    mWorldMatrix *= rotateMatrix;
}
void Cube::ScalingShape(float scaleFactorX, float scaleFactorY, float scaleFactorZ)
{
    XMMATRIX scalingMatrix = XMMatrixScaling(scaleFactorX, scaleFactorY, scaleFactorZ);
    mWorldMatrix *= scalingMatrix;
}
