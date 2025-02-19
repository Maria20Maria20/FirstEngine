#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <directxmath.h>
#include <d3dcompiler.h>
#include <stdexcept>
#include <vector>

class TriangleComponent
{
public:
    TriangleComponent(ID3D11Device* device, ID3D11DeviceContext* context);
    void DrawShape(DirectX::XMFLOAT4 points[], int count, const DirectX::XMFLOAT2& offset);

    ID3D11Buffer* vb;  // Вершинный буфер
    ID3D11Buffer* ib;  // Индексный буфер
private:
    DirectX::XMFLOAT4 points[8] = { //set points for show it (right = color, left = vertex position) (four line because need square)
    DirectX::XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(0.7f, 0.08f, 0.9f, 1.0f),
    DirectX::XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(0.3f, 0.06f, 0.9f, 0.5f),
    DirectX::XMFLOAT4(0.5f, -0.5f, 0.5f, 1.0f),  DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f),
    //DirectX::XMFLOAT4(-0.5f, 0.5f, 0.5f, 1.0f),  DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
    };

    ID3D11Device* device;
    ID3D11DeviceContext* context;

    void CreateVertexBuffer(DirectX::XMFLOAT4 points[], int count, const DirectX::XMFLOAT2& offset);
    void CreateIndexBuffer();
};
