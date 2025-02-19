#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <directxmath.h>
#include <d3dcompiler.h>
#include <stdexcept>
#include <vector>
#include "ShapeType.h"


class TriangleComponent
{
public:
    TriangleComponent(ID3D11Device* device, ID3D11DeviceContext* context);
    void SetupShape(DirectX::XMFLOAT4* shape, int& vertexCount, ShapeType shapeType, float size, DirectX::XMFLOAT2 offset);
    void DrawShape(DirectX::XMFLOAT4 points[], int count, const DirectX::XMFLOAT2& offset);
    void MoveShape(float dx, float dy, float dz);

    ID3D11Buffer* vb;  // vertex buffer
    ID3D11Buffer* ib;  // index buffer
    float MoveSpeed = 0.1f;
    float DirectionX = -1.0f;
    float DirectionY = 0.0f;
    ShapeType shapeType;
private:
    std::vector<DirectX::XMFLOAT4> currentShape; 
    int vertexCount = 0; 

    ID3D11Device* device;
    ID3D11DeviceContext* context;

    void CreateVertexBuffer(DirectX::XMFLOAT4 points[], int count, const DirectX::XMFLOAT2& offset);
    void CreateIndexBuffer();
};
