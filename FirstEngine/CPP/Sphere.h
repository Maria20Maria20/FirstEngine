#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <memory>
#include <d3dcompiler.h>
#include <iostream>

using namespace DirectX;
using namespace Microsoft::WRL;

class Sphere
{
public:
    Sphere(Microsoft::WRL::ComPtr<ID3D11Device> device, ID3DBlob* vertexBC, ID3D11VertexShader* vs,
        ID3D11PixelShader* ps, ID3D11RenderTargetView* rtv,
        ID3D11DepthStencilView* depthStencilView,
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
    void Update(float dt);
    void Draw(ID3D11DeviceContext* context, const XMMATRIX& viewProj);

    struct Vertex
    {
        XMFLOAT4 position;
        XMFLOAT4 color;
    };
    Vertex* vertices;
    UINT* indices;
    ID3D11VertexShader* mVertexShader;
    ID3D11PixelShader* mPixelShader;
    ID3DBlob* vsBlob;
    ID3DBlob* psBlob;
    ID3DBlob* errorVertexCode;
    ID3DBlob* errorPixelCode;
    ID3D11InputLayout* mInputLayout;
    ID3D11RenderTargetView* renderTargetView;
    ID3D11DepthStencilView* depthStencilView;
private:
    int verticesNum = 5;
    int sliceCount = 20;
    int elevationCount = 7;
    int indicesNum;

    float radius = 0.3;
    XMFLOAT4 sphere_color_1 = XMFLOAT4(0.0f, 0.3f, 0.0f, 1.0f);
    XMFLOAT4 sphere_color_2 = XMFLOAT4(0.0f, 0.9f, 0.0f, 1.0f);

    ID3D11Buffer* mVertexBuffer;
    ID3D11Buffer* mIndexBuffer;
    ID3D11Buffer* mConstantBuffer;
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

    struct ConstantBuffer
    {
        XMMATRIX worldViewProj = XMMatrixIdentity();
    };


    void CreateConstantBuffer();
    void CreateVertexBuffer();
    void CreateIndexBuffer();
    void InitializeBuffers();
    void InitializeShaders();
    void CreateInputLayout();
protected:
    XMMATRIX mWorldMatrix = XMMatrixIdentity();
    XMMATRIX mRotationMatrix = XMMatrixIdentity();
    XMMATRIX mScaleMatrix = XMMatrixIdentity();
    float mRotationAngle;
    ConstantBuffer cb;
    void SetupIAStage();
    void RotateShape(XMVECTOR Axis, FLOAT Angle, float deltaTime);
    void ScalingShape(float scaleFactor);
};