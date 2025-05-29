#pragma once
#include "LightObject.h"

class GBufferMaps :
    public LightObject
{
public:
    GBufferMaps(ID3D11Device* device, ID3D11Texture2D* depthBuffer = nullptr);

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

    //Microsoft::WRL::ComPtr<ID3D11Buffer> pGBufferMapsPBuffer;

    // Rasterizer
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer;
    // BlendState
    Microsoft::WRL::ComPtr<ID3D11BlendState> pBlendState;
    // DepthStencilState
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDepthStencilState;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthBuffer;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthBufferCopy;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> depthBufferCopySRV;
};


