#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <iostream>
#include <d3dcompiler.h>
#include "DisplayWin32.h"
#include <chrono>
#include <d3d.h>
#include <directxmath.h>
#include "TriangleComponent.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

class Game
{
public:
    Game();
    int Initialize();
private:
    DisplayWin32 display;
    TriangleComponent* triangle;
    HRESULT res;
    IDXGISwapChain* swapChain;
    DXGI_SWAP_CHAIN_DESC swapDesc;
    ID3D11DeviceContext* context;
    ID3D11Device* device;
    ID3D11RenderTargetView* rtv;
    ID3D11Texture2D* backTex;
    ID3DBlob* vertexBC = nullptr;
    ID3DBlob* errorVertexCode = nullptr;
    ID3DBlob* pixelBC;
    ID3DBlob* errorPixelCode;
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    ID3D11InputLayout* layout;
    bool retFlag;
    int retVal;
    ID3D11Buffer* ib;
    ID3D11Buffer* vb;
    CD3D11_RASTERIZER_DESC rastDesc = {};
    ID3D11RasterizerState* rastState;
    DirectX::XMFLOAT4 points[8] = { //set points for show it (right = color, left = vertex position) (four line because need square)
    DirectX::XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(0.7f, 0.08f, 0.9f, 1.0f),
    DirectX::XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(0.3f, 0.06f, 0.9f, 0.5f),
    DirectX::XMFLOAT4(0.5f, -0.5f, 0.5f, 1.0f),  DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f),
    //DirectX::XMFLOAT4(-0.5f, 0.5f, 0.5f, 1.0f),  DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
    };
    HRESULT InitSwapChain();
    void GetBackBufferAndCreateRTV();
    int CompileShaders();
    void CreateInputLayout();
    void CreateVertexBuffers();
    void CreateIndexBuffers();
    void SetupRasterizerStage();
    void WindowLoop(std::chrono::steady_clock::time_point& PrevTime, float& totalTime, unsigned int& frameCount);
    void SetupViewport();
    void SetupIAStage(UINT  strides[1], UINT  offsets[1]);
    void SetVertexAndPixelShaders();
    void SetBackBufferOutput(UINT NumViews, ID3D11RenderTargetView* const* ppRenderTargetViews, ID3D11DepthStencilView* pDepthStencilView);
};
