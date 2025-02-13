#include <d3d11.h>
#include <wrl.h>
#include <d3d11.h>
#pragma once
constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;
HWND hWnd;
DXGI_SWAP_CHAIN_DESC swapDesc;
IDXGISwapChain* swapChain;
Microsoft::WRL::ComPtr<ID3D11Device> device;
ID3D11DeviceContext* context;
ID3D11Texture2D* backTex;
ID3D11RenderTargetView* rtv;
ID3DBlob* vertexBC = nullptr;
ID3DBlob* errorVertexCode = nullptr;
ID3DBlob* pixelBC;
ID3DBlob* errorPixelCode;
ID3D11VertexShader* vertexShader;
ID3D11PixelShader* pixelShader;
ID3D11InputLayout* layout;
ID3D11Buffer* ib;
ID3D11Buffer* vb;
UINT strides[] = { 32 };
UINT offsets[] = { 0 };
CD3D11_RASTERIZER_DESC rastDesc = {};
ID3D11RasterizerState* rastState;
void SetupIAStage(UINT  strides[1], UINT  offsets[1]);
void SetVertexAndPixelShaders();
void SetupRasterizerStage(HRESULT& res);
void SetupViewport();
void CreateInputLayout();
HRESULT InitSwapChain();
void GetBackBufferAndCreateRTV(HRESULT& res);
int CompileShaders(HRESULT& res, bool& retFlag);
void InitializeWindow(HINSTANCE hInstance, LPCWSTR applicationName);
void SetBackBufferOutput(UINT NumViews, ID3D11RenderTargetView* const* ppRenderTargetViews, ID3D11DepthStencilView* pDepthStencilView);
void WindowLoop(std::chrono::steady_clock::time_point& PrevTime, float& totalTime, unsigned int& frameCount);
template <size_t N>
void CreateVertexAndIndexBuffers(DirectX::XMFLOAT4(&points)[N]);