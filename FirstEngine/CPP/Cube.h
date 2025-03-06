#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <memory>

using namespace DirectX;
using namespace Microsoft::WRL;

class Cube
{
public:
    Cube(ID3D11Device* device, ID3DBlob* vertexBC, ID3D11VertexShader* vs,
        ID3D11PixelShader* ps, ID3D11RenderTargetView* rtv,
        ID3D11DepthStencilView* depthStencilView);
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
    void InitializeBuffers(ID3D11Device* device);  // Добавлено объявление
    void InitializeShaders(ID3D11Device* device, ID3D11VertexShader* vs, ID3D11PixelShader* ps);  // Добавлено объявление

    ID3D11Buffer* mVertexBuffer;
    ID3D11Buffer* mIndexBuffer;
    ID3D11Buffer* mConstantBuffer;

    struct ConstantBuffer
    {
        XMMATRIX worldViewProj;
    };
    ConstantBuffer cb;

    XMMATRIX mWorldMatrix;
    XMMATRIX mRotationMatrix;
    XMMATRIX mScaleMatrix;
    float mRotationAngle;
};