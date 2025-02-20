#include "Square.h"
#include "Triangle.h"

Triangle::Triangle()
{
}


Triangle::Triangle(DirectX::XMFLOAT4 vertexPositions[3],
    DirectX::XMFLOAT4 colors[3], DirectX::XMFLOAT2 startPosition,
    Microsoft::WRL::ComPtr<ID3D11Device> device,
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
    ID3DBlob* vertexBC)
{
    this->device = device;
    this->context = context;
    this->vertexBC = vertexBC;
    CreateInputLayout();
    for (int i = 0; i < 3; i++)
    {
        VertexPositions[i] = vertexPositions[i];
    }
    for (int i = 0; i < 3; i++)
    {
        Colors[i] = colors[i];
    }
    this->StartPosition = startPosition;
}
void Triangle::DrawShape(int count)
{
    std::vector<DirectX::XMFLOAT4> pointsAndColors(count * 2); //*2 = multiply on color for vertex positions

    for (int i = 0; i < count; i++)
    {
        pointsAndColors[i * 2] = DirectX::XMFLOAT4(
            VertexPositions[i].x + StartPosition.x,
            VertexPositions[i].y + StartPosition.y,
            VertexPositions[i].z,
            VertexPositions[i].w
        );

        pointsAndColors[i * 2 + 1] = Colors[i];
    }

    currentShape.assign(pointsAndColors.begin(), pointsAndColors.end());
    vertexCount = count * 2;

    CreateVertexBuffer(pointsAndColors.data(), vertexCount, StartPosition);
    CreateIndexBuffer();
}
void Triangle::MoveShape(float dx, float dy, float dz)
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

void Triangle::CreateVertexBuffer(DirectX::XMFLOAT4 points[], int count, const DirectX::XMFLOAT2& offset)
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
void Triangle::CreateIndexBuffer()
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
void Triangle::SetupIAStage(UINT strides[1], UINT offsets[1])
{
    context->IASetInputLayout(layout);
    context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);
    context->IASetVertexBuffers(0, 1, &vb, strides, offsets);
}
void Triangle::CreateInputLayout()
{
    D3D11_INPUT_ELEMENT_DESC inputElements[] = {
    D3D11_INPUT_ELEMENT_DESC{
        "POSITION", //parameter name from hlsl file
        0, //need if we have more one element with same semantic
        DXGI_FORMAT_R32G32B32A32_FLOAT,
        0, //vertex index (between 0 and 15)
        0, //offset from beginning vertex
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
        vertexBC->GetBufferPointer(),
        vertexBC->GetBufferSize(),
        &layout);
}

