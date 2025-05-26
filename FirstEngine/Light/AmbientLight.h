#pragma once
#include "LightObject.h"

class AmbientLight :
    public LightObject
{
public:
    AmbientLight(ID3D11Device* device, Vector4 ambient);

    struct AmbientLightPixelConstantBuffer {
        XMFLOAT4 Ambient;
    } ambientLightData;

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

    //Bind::PixelConstantBuffer<AmbientLightPCB>* ambientLightPBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> pAmbientLightPBuffer;

    // Rasterizer
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer;
    // BlendState
    Microsoft::WRL::ComPtr<ID3D11BlendState> pBlendState;
    // DepthStencilState
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDepthStencilState;


};


