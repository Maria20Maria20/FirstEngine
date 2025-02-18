#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <directxmath.h>
#include <d3dcompiler.h>
#include <stdexcept>

class TriangleComponent
{
public:
    TriangleComponent(ID3D11Device* device, ID3D11DeviceContext* context);
    void Draw();

private:
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

    Microsoft::WRL::ComPtr<ID3D11Buffer> vb;  // Вершинный буфер
    Microsoft::WRL::ComPtr<ID3D11Buffer> ib;  // Индексный буфер
    Microsoft::WRL::ComPtr<ID3D11InputLayout> layout; // Описание входных данных

    DirectX::XMFLOAT4 vertices[3]; // Вершины треугольника
    DirectX::XMFLOAT4 colors[3];   // Цвета вершин

    void CreateVertexBuffer();
    void CreateIndexBuffer();
};
