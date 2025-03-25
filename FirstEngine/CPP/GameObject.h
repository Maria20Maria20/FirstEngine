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
#include "Camera.h"
#include <random>
#include <assimp/scene.h>
#include "Texture.h"


using namespace DirectX;

class GameObject
{
private:
    D3D11_INPUT_ELEMENT_DESC defaultIALayoutInputElements[2] = {
        D3D11_INPUT_ELEMENT_DESC{
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            0,
            0,
            D3D11_INPUT_PER_VERTEX_DATA,
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

public:
    enum class ObjectType {
        CUBE = 0, 
        SPHERE = 1,
        GRID = 2,
        SKYBOX = 3,
        PLANE = 4
    };
    GameObject();
    GameObject(Microsoft::WRL::ComPtr<ID3D11Device> device, ID3D11VertexShader* vs,
        ID3D11PixelShader* ps, ID3D11RenderTargetView* rtv,
        ID3D11DepthStencilView* depthStencilView,
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, ObjectType planetType,
        LPCWSTR shaderFilePath = L"./Shaders/CubeShader.hlsl", UINT numInputElements = 2, 
        D3D11_INPUT_ELEMENT_DESC* IALayoutInputElements = nullptr);
    ObjectType currentObject;
    void Update(float dt);
    void Draw(ID3D11DeviceContext* context, const DirectX::XMMATRIX& viewProj);

    struct Vertex
    {
        DirectX::XMFLOAT4 position;
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT2 texCoord;
        DirectX::XMFLOAT3 normal;
    };
    struct ConstantBuffer
    {
        //DirectX::XMMATRIX world; 
        DirectX::XMMATRIX worldViewProj = DirectX::XMMatrixIdentity();
    };

    Vertex* vertices;
    int* indices;
    ID3DBlob* psBlob;
    ID3DBlob* errorVertexCode;
    ID3DBlob* errorPixelCode;
    ID3D11InputLayout* mInputLayout;
    LPCWSTR shaderFilePath = L"./Shaders/CubeShader.hlsl";
    DirectX::XMMATRIX mWorldMatrix = DirectX::XMMatrixIdentity();

    D3D11_INPUT_ELEMENT_DESC* IALayoutInputElements;
    UINT numInputElements = 2;

    UINT verticesNum = 5;
    UINT indicesNum = 0;
    
    std::string directoryPath;
    bool hasTexture = false;
    std::vector<Texture> textures;

    Camera camera = Camera();
    ID3DBlob* vsBlob = nullptr;
    ID3D11VertexShader* mVertexShader;
    ID3D11PixelShader* mPixelShader;
    ID3D11RenderTargetView* renderTargetView;
    ID3D11DepthStencilView* depthStencilView;
    DirectX::XMMATRIX mRotationMatrix = DirectX::XMMatrixIdentity();
    DirectX::XMMATRIX mScaleMatrix = DirectX::XMMatrixIdentity();
    float mRotationAngle;
    ConstantBuffer cb;
    void SetupIAStage();
    void RotateShape(DirectX::XMVECTOR Axis, FLOAT Angle, float deltaTime);
    void ScalingShape(float scaleFactor);
    XMFLOAT4 gridColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); // gray color
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
private:
    float radius = 1.0f;
    int sliceCount = 20;
    int elevationCount = 7;

    DirectX::XMFLOAT4 sphere_color_1 = DirectX::XMFLOAT4(0.0f, 0.3f, 0.0f, 1.0f);
    DirectX::XMFLOAT4 sphere_color_2 = DirectX::XMFLOAT4(0.0f, 0.9f, 0.0f, 1.0f);

    ID3D11Buffer* mVertexBuffer;
    ID3D11Buffer* mIndexBuffer = nullptr;
    ID3D11Buffer* mConstantBuffer;


    void CreateSphereVertexBuffer();
    void CreateSphereIndexBuffer();

    void CreateCubeVertexBuffer();
    void CreateCubeIndexBuffer();


    void CreateGridVertexBuffer();
    void CreateGridIndexBuffer();
    
    void CreateSkyVertexBuffer();

    void CreateRandomHeightPlane(float width, float depth, UINT widthSegments, UINT depthSegments, float maxHeight, DirectX::XMFLOAT4 col,
        Vertex** vertices, UINT* verticesNum, int** indices, UINT* indicesNum);

public:
    void CreateInputLayout(UINT numInputElements, D3D11_INPUT_ELEMENT_DESC* IALayoutInputElements);
protected:
    void InitializeBuffers();
    void InitializeShaders();
    void CreateVertexBuffer();
    void CreateIndexBuffer();
    void CreateConstantBuffer();
};

Matrix GetRandomRotateTransform();