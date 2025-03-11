#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <iostream>
#include <d3dcompiler.h>
#include "DisplayWin32.h"
#include <chrono>
#include "InputDevice.h"
#include "Delegates.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

class Game {
protected:
    // Приватные конструкторы
    Game() : display(new DisplayWin32()) {}
    Game(const Game&) = delete;             // Запрет копирования
    Game& operator=(const Game&) = delete;  // Запрет присваивания

    static Game* instance;  // Единственный экземпляр

    ID3D11DeviceContext* context;
    ID3D11Device* device;
    ID3DBlob* vertexBC = nullptr;
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    ID3D11RenderTargetView* rtv;
    ID3D11DepthStencilView* depthStencilView;
    IDXGISwapChain* swapChain;

    int Initialize();
    void WindowLoop(std::chrono::steady_clock::time_point& PrevTime, float& totalTime, unsigned int& frameCount);

public:
    DisplayWin32* display;

    // Метод доступа к экземпляру
    static Game& getInstance();

    // Метод для явного удаления экземпляра
    static void destroyInstance();

    ~Game() {
        delete display;  // Освобождаем ресурсы
    }
private:
    HRESULT res;
    int retVal;
    bool retFlag;
    DXGI_SWAP_CHAIN_DESC swapDesc;
    ID3D11Texture2D* backTex;
    ID3DBlob* errorVertexCode = nullptr;
    ID3DBlob* pixelBC;
    ID3DBlob* errorPixelCode;
    CD3D11_RASTERIZER_DESC rastDesc = {};
    ID3D11RasterizerState* rastState;

    ID3D11DepthStencilState* depthStencilState;
    ID3D11Texture2D* pDepthStencil;

    HRESULT InitSwapChain();
    void GetBackBufferAndCreateRTV();
    int CompileShaders();
    void SetupRasterizerStage();
    void SetBackBufferOutput(UINT NumViews, ID3D11RenderTargetView* const* ppRenderTargetViews, ID3D11DepthStencilView* pDepthStencilView);
};

