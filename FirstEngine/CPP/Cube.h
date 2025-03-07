#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <memory>
#include <d3dcompiler.h>
#include "Game.h"


using namespace DirectX;
using namespace Microsoft::WRL;

class Cube
{
public:
    Cube(Microsoft::WRL::ComPtr<ID3D11Device> device, ID3DBlob* vertexBC, ID3D11VertexShader* vs,
        ID3D11PixelShader* ps, ID3D11RenderTargetView* rtv,
        ID3D11DepthStencilView* depthStencilView,
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
    void Update(float dt);
    void Draw(ID3D11DeviceContext* context, const XMMATRIX& viewProj);

    ID3D11RenderTargetView* renderTargetView;
    ID3D11RasterizerState* rastState;
    ID3D11DepthStencilView* depthStencilView;


    struct Vertex
    {
        XMFLOAT4 position;
        XMFLOAT4 color;
    };

    Vertex vertices[8];
    int indices[36];
    ID3D11VertexShader* mVertexShader;
    ID3D11PixelShader* mPixelShader;
    ID3DBlob* vsBlob;
    ID3DBlob* psBlob;
    ID3DBlob* errorVertexCode;
    ID3DBlob* errorPixelCode;
    ID3D11InputLayout* mInputLayout;
    void RotateShape(XMVECTOR Axis, FLOAT Angle, float deltaTime);
    void ScalingShape(float scaleFactorX, float scaleFactorY, float scaleFactorZ);
private:

    ID3D11Buffer* mVertexBuffer;
    ID3D11Buffer* mIndexBuffer;
    ID3D11Buffer* mConstantBuffer;
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

    struct ConstantBuffer
    {
        XMMATRIX worldViewProj = XMMatrixIdentity();
    };
    ConstantBuffer cb;

    XMMATRIX mWorldMatrix = XMMatrixIdentity();
    XMMATRIX mRotationMatrix = XMMatrixIdentity();
    XMMATRIX mScaleMatrix = XMMatrixIdentity();
    float mRotationAngle;

    void CreateConstantBuffer();
    void CreateVertexBuffer();
    void CreateIndexBuffer();
    void InitializeBuffers();  // Добавлено объявление
    void InitializeShaders(ID3D11VertexShader* vs, ID3D11PixelShader* ps);  // Добавлено объявление
    void CreateInputLayout();
protected:
    void SetupIAStage();
    void SetupViewport();
};