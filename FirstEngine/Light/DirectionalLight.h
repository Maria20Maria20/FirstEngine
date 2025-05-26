#pragma once
#include "LightObject.h"

class DirectionalLight :
    public LightObject
{
public:
    DirectionalLight(ID3D11Device* device, Vector3 position, Vector3 direction, Vector4 ambient, Vector4 diffuse, Vector4 specular);

    struct DirectionalLightPCB {
        XMFLOAT4 Diffuse;
        XMFLOAT4 Specular;
        XMFLOAT3 Position;
        float pad1;

        XMFLOAT3 Direction;
        float pad2;
    } directionalLightData;

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
    // PixelShader
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;

    Microsoft::WRL::ComPtr<ID3D11Buffer> pDirectionalLightPBuffer;

    // Rasterizer
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer;
    // BlendState
    Microsoft::WRL::ComPtr<ID3D11BlendState> pBlendState;
    // DepthStencilState
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDepthStencilState;


};

