#include "TriangleComponent.h"


TriangleComponent::TriangleComponent(ID3D11Device* device, ID3D11DeviceContext* context)
{
    this->device = device;
    this->context = context;
}

void TriangleComponent::DrawShape(DirectX::XMFLOAT4 points[], int count, const DirectX::XMFLOAT2& offset)
{
    CreateVertexBuffer(points, count, offset);
    CreateIndexBuffer();
}


void TriangleComponent::CreateVertexBuffer(DirectX::XMFLOAT4 points[], int count, const DirectX::XMFLOAT2& offset)
{
    // Allocate a temporary array for transformed vertices
    std::vector<DirectX::XMFLOAT4> transformedPoints(count);

    // Apply the offset
    for (int i = 0; i < count; ++i)
    {
        transformedPoints[i] = points[i];
        transformedPoints[i].x += offset.x;
        transformedPoints[i].y += offset.y;
    }

    D3D11_BUFFER_DESC vertexBufDesc = {};
    vertexBufDesc.Usage = D3D11_USAGE_DEFAULT; //how often does writing and reading to the buffer occur (default = read/write from GPU)
    vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //way to bind a buffer to a pipeline
    vertexBufDesc.CPUAccessFlags = 0; //0 = CPU don't need, 1 = CPU need
    vertexBufDesc.MiscFlags = 0; //optional parameters
    vertexBufDesc.StructureByteStride = 0; //size per element in buffer structure
    vertexBufDesc.ByteWidth = sizeof(DirectX::XMFLOAT4) * count;

    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = transformedPoints.data();
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    device->CreateBuffer(&vertexBufDesc, &vertexData, &vb);
}

void TriangleComponent::CreateIndexBuffer()
{
    int indeces[] = { 0,1,2, 1,0,3 }; //for show square
    D3D11_BUFFER_DESC indexBufDesc = {};
    indexBufDesc.Usage = D3D11_USAGE_DEFAULT; //how often does writing and reading to the buffer occur (default = read/write from GPU)
    indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER; //way to bind a buffer to a pipeline
    indexBufDesc.CPUAccessFlags = 0; //0 = CPU don't need, 1 = CPU need
    indexBufDesc.MiscFlags = 0; //optional parameters
    indexBufDesc.StructureByteStride = 0; //size per element in buffer structure
    indexBufDesc.ByteWidth = sizeof(int) * std::size(indeces);

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = indeces;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    device->CreateBuffer(&indexBufDesc, &indexData, &ib);
}
