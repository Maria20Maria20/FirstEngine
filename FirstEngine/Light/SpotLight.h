#pragma once
#include "LightObject.h"

class SpotLight :
    public LightObject
{
public:
    SpotLight(ID3D11Device* device, Vector3 position,
        float range, Vector3 direction, float spot, Vector3 att,
        Vector4 ambient, Vector4 diffuse, Vector4 specular);

    struct SpotLightPCB {
        XMFLOAT4 Diffuse;
        XMFLOAT4 Specular;
        XMFLOAT3 Position;
        float Range;

        XMFLOAT3 Direction;
        float Spot;

        XMFLOAT3 Att;
        float pad;
    } spotLightData;

    float width;
    float depth;

    Vector4 ambient;

    D3D11_DEPTH_STENCIL_DESC GetDepthStencilDesc() override;
    D3D11_RASTERIZER_DESC GetRasterizerDesc() override;

    void Update(float deltaTime) override;

    void applyGBufferPass(ID3D11DeviceContext* context) override;
    void applyLightPass(ID3D11DeviceContext* context) override;

    void UpdateBuffers(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;

    // Topology
    D3D11_PRIMITIVE_TOPOLOGY topology;
    // IndexBuffer - mIndexBuffer;
    // VertexShader
    Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
    // InputLayout
    Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
    // TransformCBuffer
    Microsoft::WRL::ComPtr<ID3D11Buffer> pTransformConstantBuffer;
    // PixelShader
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;

    Microsoft::WRL::ComPtr<ID3D11Buffer> pSpotLightPBuffer;

    // Rasterizer
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer;
    // BlendState
    Microsoft::WRL::ComPtr<ID3D11BlendState> pBlendState;
    // DepthStencilState
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDepthStencilState;
};

