#ifndef NPC_PADDLE_H
#define NPC_PADDLE_H

#include "../CPP/Square.h"
#include "../CPP/DisplayWin32.h"

class NPCPaddle : public Square
{
public:
    NPCPaddle();
    NPCPaddle(DirectX::XMFLOAT4 vertexPositions[4],
        DirectX::XMFLOAT4 colors[4], DirectX::XMFLOAT2 startPosition,
        float minClamp, float maxClamp,
        Microsoft::WRL::ComPtr<ID3D11Device> device,
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
        ID3DBlob* vertexBC, ID3D11RenderTargetView* rtv,
        ID3D11VertexShader* vs,
        ID3D11PixelShader* ps,
        DisplayWin32 display);

    void Update(float deltaTime); // Метод обновления движения

private:
    float minClamp;
    float maxClamp;
    float direction; // 1 = вверх, -1 = вниз
    float speed;
    ID3D11RenderTargetView* renderTargetView;
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    DisplayWin32 Display;
    void SetupViewport();
    void SetVertexAndPixelShaders();
    void SetBackBufferOutput(UINT NumViews, ID3D11RenderTargetView* const* ppRenderTargetViews, ID3D11DepthStencilView* pDepthStencilView);
};

#endif
