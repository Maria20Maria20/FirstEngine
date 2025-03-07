#include "Square.h"

Square::Square()
{
}

Square::Square(DirectX::XMFLOAT4 vertexPositions[4],
	DirectX::XMFLOAT4 colors[4], DirectX::XMFLOAT4 startPosition,
    Microsoft::WRL::ComPtr<ID3D11Device> device,
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
    ID3DBlob* vertexBC) : device(device), context(context),
    vertexBC(vertexBC)
{
	for (int i = 0; i < 4; i++)
	{
		VertexPositions[i] = vertexPositions[i];
	}
	for (int i = 0; i < 4; i++)
	{
		Colors[i] = colors[i];
	}
	this->StartPosition = startPosition;
    CreateConstantBuffer();
    CreateInputLayout();
}
void Square::CreateConstantBuffer()
{
    D3D11_BUFFER_DESC constantBufferDesc = {};
    constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC; //0 = CPU don't need, D3D11_USAGE_DYNAMIC = CPU need
    constantBufferDesc.ByteWidth = sizeof(CBTransform);
    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    constantBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = &transformData;
    device->CreateBuffer(&constantBufferDesc, &initData, &cbTransform);
}
void Square::InitializeShape(int count)
{
    std::vector<DirectX::XMFLOAT4> pointsAndColors(count * 2); //*2 = multiply on color for vertex positions

    for (int i = 0; i < count; i++)
    {
        pointsAndColors[i * 2] = DirectX::XMFLOAT4(
            VertexPositions[i].x,
            VertexPositions[i].y,
            VertexPositions[i].z,
            VertexPositions[i].w
        );

        pointsAndColors[i * 2 + 1] = Colors[i];
    }

    vertexCount = count * 2;


    CreateVertexBuffer(pointsAndColors.data(), vertexCount, StartPosition);
    CreateIndexBuffer();
}
void Square::MoveShape(float dx, float dy, float dz)
{
    if (!vb) {
        throw std::runtime_error("Vertex buffer is not initialized.");
        return;
    }
    UINT strides[] = { 32 };
    UINT offsets[] = { 0 };

    SetupIAStage(strides, offsets);

    DirectX::XMMATRIX moveMatrix = DirectX::XMMatrixTranslation(dx, dy, 0);
    transformData.translation *= moveMatrix;

    // update data in GPU
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = context->Map(cbTransform, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource); //move with constant buffer
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to map vertex buffer for updating.");
    }
    memcpy(mappedResource.pData, &transformData, sizeof(CBTransform));
    context->Unmap(cbTransform, 0);

    context->VSSetConstantBuffers(0, 1, &cbTransform);
    context->DrawIndexed(6, 0, 0);
}
void Square::SetupViewport()
{
    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(Game::getInstance().display->ScreenWidth);
    viewport.Height = static_cast<float>(Game::getInstance().display->ScreenHeight);
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1.0f;

    context->RSSetViewports(1, &viewport);
}

void Square::RotateShape(FXMVECTOR Axis, FLOAT Angle)
{
    XMVECTOR normalizedAxis = XMVector3Normalize(Axis); // Используем новую переменную

    // Проверяем, что вектор не нулевой
    if (XMVector3Equal(normalizedAxis, XMVectorZero()))
    {
        return; // Если ось некорректна, не выполняем вращение
    }

    XMMATRIX rotateMatrix = XMMatrixRotationAxis(normalizedAxis, XMConvertToRadians(Angle));
    transformData.translation *= rotateMatrix;
}

void Square::ScalingShape(float scaleFactorX, float scaleFactorY, float scaleFactorZ)
{
    XMMATRIX scalingMatrix = XMMatrixScaling(scaleFactorX, scaleFactorY, scaleFactorZ);
    transformData.translation *= scalingMatrix;
}

void Square::CreateVertexBuffer(DirectX::XMFLOAT4 points[], int count, const DirectX::XMFLOAT4& translation)
{
    // Allocate a temporary array for transformed vertices
    std::vector<DirectX::XMFLOAT4> transformedPoints(count);

    // Apply the translation
    for (int i = 0; i < count; ++i)
    {
        transformedPoints[i] = points[i];
        transformedPoints[i].x += translation.x;
        transformedPoints[i].y += translation.y;
    }

    D3D11_BUFFER_DESC vertexBufDesc = {};
    vertexBufDesc.Usage = D3D11_USAGE_DEFAULT; //how often does writing and reading to the buffer occur (default = read/write from GPU)
    vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //way to bind a buffer to a pipeline
    vertexBufDesc.CPUAccessFlags = 0; //0 = CPU don't need, D3D11_CPU_ACCESS_WRITE = CPU need
    vertexBufDesc.MiscFlags = 0; //optional parameters
    vertexBufDesc.StructureByteStride = 0; //size per element in buffer structure
    vertexBufDesc.ByteWidth = sizeof(DirectX::XMFLOAT4) * count;

    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = transformedPoints.data();
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    device->CreateBuffer(&vertexBufDesc, &vertexData, &vb);
}
void Square::CreateIndexBuffer()
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
void Square::SetupIAStage(UINT strides[1], UINT offsets[1])
{
    context->IASetInputLayout(layout);
    context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);
    context->IASetVertexBuffers(0, 1, &vb, strides, offsets);
}
void Square::CreateInputLayout()
{
    D3D11_INPUT_ELEMENT_DESC inputElements[] = {
    D3D11_INPUT_ELEMENT_DESC{
        "POSITION", //parameter name from hlsl file
        0, //need if we have more one element with same semantic
        DXGI_FORMAT_R32G32B32A32_FLOAT,
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
        vertexBC->GetBufferPointer(),
        vertexBC->GetBufferSize(),
        &layout);
}


