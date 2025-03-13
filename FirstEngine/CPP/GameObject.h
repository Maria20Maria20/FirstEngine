#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <memory>
#include <d3dcompiler.h>
#include <iostream>
#include <wrl.h>
#include "DisplayWin32.h"
#include <chrono>
#include <SimpleMath.h>

using namespace DirectX;

class GameObject
{
public:
    enum class ObjectType {
        CUBE = 0, 
        SPHERE = 1,
        GRID = 2
    };
    GameObject(Microsoft::WRL::ComPtr<ID3D11Device> device, ID3DBlob* vertexBC, ID3D11VertexShader* vs,
        ID3D11PixelShader* ps, ID3D11RenderTargetView* rtv,
        ID3D11DepthStencilView* depthStencilView,
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, ObjectType planetType, LPCWSTR shaderFilePath = L"./Shaders/CubeShader.hlsl");
    ObjectType currentObject;
    void Update(float dt);
    void Draw(ID3D11DeviceContext* context, const DirectX::XMMATRIX& viewProj);

    struct Vertex
    {
        DirectX::XMFLOAT4 position;
        DirectX::XMFLOAT4 color;
    };
    struct ConstantBuffer
    {
        DirectX::XMMATRIX worldViewProj = DirectX::XMMatrixIdentity();
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
    LPCWSTR shaderFilePath = L"./Shaders/CubeShader.hlsl";
    DirectX::XMMATRIX mWorldMatrix = DirectX::XMMatrixIdentity();
protected:
    DirectX::XMMATRIX mRotationMatrix = DirectX::XMMatrixIdentity();
    DirectX::XMMATRIX mScaleMatrix = DirectX::XMMatrixIdentity();
    float mRotationAngle;
    ConstantBuffer cb;
    void SetupIAStage();
    void RotateShape(DirectX::XMVECTOR Axis, FLOAT Angle, float deltaTime);
    void ScalingShape(float scaleFactor);
    XMFLOAT4 gridColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); // gray color
private:
    int verticesNum = 5;
    int sliceCount = 20;
    int elevationCount = 7;
    int indicesNum = 0;

    float radius = 0.3;
    DirectX::XMFLOAT4 sphere_color_1 = DirectX::XMFLOAT4(0.0f, 0.3f, 0.0f, 1.0f);
    DirectX::XMFLOAT4 sphere_color_2 = DirectX::XMFLOAT4(0.0f, 0.9f, 0.0f, 1.0f);

    ID3D11Buffer* mVertexBuffer;
    ID3D11Buffer* mIndexBuffer = nullptr;
    ID3D11Buffer* mConstantBuffer;
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

    void CreateConstantBuffer();

    void CreateSphereVertexBuffer();
    void CreateSphereIndexBuffer();

    void CreateCubeVertexBuffer();
    void CreateCubeIndexBuffer();

    void CreateGridVertexBuffer();
    void CreateGridIndexBuffer();

    void InitializeBuffers();
    void InitializeShaders();
    void CreateInputLayout();
};