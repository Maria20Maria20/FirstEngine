#include "TriangleComponent.h"


TriangleComponent::TriangleComponent(ID3D11Device* device, ID3D11DeviceContext* context)
{
    this->device = device;
    this->context = context;

    vertices[0] = DirectX::XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f);
    vertices[1] = DirectX::XMFLOAT4(-0.5f, -0.5f, 0.0f, 1.0f);
    vertices[2] = DirectX::XMFLOAT4(0.5f, -0.5f, 0.0f, 1.0f);

    colors[0] = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
    colors[1] = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
    colors[2] = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

    CreateVertexBuffer();
    CreateIndexBuffer();
}

void TriangleComponent::CreateVertexBuffer()
{
    struct Vertex
    {
        DirectX::XMFLOAT4 position;
        DirectX::XMFLOAT4 color;
    };

    Vertex vertexData[3] = {
        {vertices[0], colors[0]},
        {vertices[1], colors[1]},
        {vertices[2], colors[2]}
    };

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(vertexData);
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertexData;

    /*HRESULT res = device->CreateBuffer(&bufferDesc, &initData, &vb);
    if (FAILED(res))
    {
        throw std::runtime_error("Failed to create vertex buffer");
    }*/
}

void TriangleComponent::CreateIndexBuffer()
{
    uint16_t indices[3] = { 0, 1, 2 };

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(indices);
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = indices;

    /*HRESULT res = device->CreateBuffer(&bufferDesc, &initData, &ib);
    if (FAILED(res))
    {
        throw std::runtime_error("Failed to create index buffer");
    }*/
}

void TriangleComponent::Draw()
{
    UINT stride = sizeof(DirectX::XMFLOAT4) * 2;
    UINT offset = 0;

    context->IASetVertexBuffers(0, 1, vb.GetAddressOf(), &stride, &offset);
    context->IASetIndexBuffer(ib.Get(), DXGI_FORMAT_R16_UINT, 0);
    context->DrawIndexed(3, 0, 0);
}
