#include "TriangleComponent.h"


/*TriangleComponent::TriangleComponent(ID3D11Device* device, ID3D11DeviceContext* context)
{
    this->device = device;
    this->context = context;
}
void TriangleComponent::DrawShape(DirectX::XMFLOAT4 points[], DirectX::XMFLOAT4 colors[], int count, const DirectX::XMFLOAT2& offset)
{
    //Shape newShape;
    //newShape.vertices = vertices;
    std::vector<DirectX::XMFLOAT4> pointsAndColors(count * 2); //*2 = multiply on color for vertex positions

    for (int i = 0; i < count; i++)
    {
        pointsAndColors[i * 2] = DirectX::XMFLOAT4(
            points[i].x + offset.x,
            points[i].y + offset.y,
            points[i].z,
            points[i].w
        );

        pointsAndColors[i * 2 + 1] = colors[i];
    }

    currentShape.assign(pointsAndColors.begin(), pointsAndColors.end());
    //shapes.push_back(pointsAndColors);
    vertexCount = count * 2;

    CreateVertexBuffer(pointsAndColors.data(), vertexCount, offset);
    CreateIndexBuffer();
}

void TriangleComponent::MoveShape(float dx, float dy, float dz)
{
    if (currentShape.empty()) return; 
    if (!vb) {
        throw std::runtime_error("Vertex buffer is not initialized.");
        return;
    }

    // offset vertices
    for (auto& vertex : currentShape) //all vertices one shape
    {
        vertex.x += dx;
        vertex.y += dy;
        vertex.z += dz;
    }

    // update data in GPU
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = context->Map(vb, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to map vertex buffer for updating.");
    }
    memcpy(mappedResource.pData, currentShape.data(), sizeof(DirectX::XMFLOAT4) * vertexCount);
    context->Unmap(vb, 0);
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
    //vertexBufDesc.Usage = D3D11_USAGE_DEFAULT; //how often does writing and reading to the buffer occur (default = read/write from GPU)
    vertexBufDesc.Usage = D3D11_USAGE_DYNAMIC; //how often does writing and reading to the buffer occur (default = read/write from GPU)
    vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //way to bind a buffer to a pipeline
    //vertexBufDesc.CPUAccessFlags = 0; //0 = CPU don't need, 1 = CPU need
    vertexBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //0 = CPU don't need, 1 = CPU need
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
*/