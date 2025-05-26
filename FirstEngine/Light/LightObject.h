#pragma once
#include "../CPP/GameObject.h"
#include "../CPP/Camera.h"

class LightObject :
    public GameObject
{
public:
    virtual D3D11_DEPTH_STENCIL_DESC GetDepthStencilDesc()
    {
        D3D11_DEPTH_STENCIL_DESC dsDesc = {};
        dsDesc.DepthEnable = TRUE;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
        return dsDesc;
    };
    virtual D3D11_RASTERIZER_DESC GetRasterizerDesc()
    {
        D3D11_RASTERIZER_DESC rasterDesc = {};
        rasterDesc.CullMode = D3D11_CULL_NONE;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
        return rasterDesc;
    };

    virtual void UpdateBuffers(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) = 0;
};

