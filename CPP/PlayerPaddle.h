#ifndef PLAYER_PADDLE_H
#define PLAYER_PADDLE_H

#include "../CPP/Square.h"
#include "../CPP/DisplayWin32.h"

class PlayerPaddle : public Square
{
public:
    PlayerPaddle();
    PlayerPaddle(DirectX::XMFLOAT4 vertexPositions[4],
        DirectX::XMFLOAT4 colors[4],
        Microsoft::WRL::ComPtr<ID3D11Device> device,
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
        ID3DBlob* vertexBC, ID3D11RenderTargetView* rtv,
        ID3D11VertexShader* vs,
        ID3D11PixelShader* ps,
        DisplayWin32 display,
        DirectX::XMFLOAT4 startPos);

    void Update(float deltaTime); // Метод обновления движения
    float speed;
    float DefaultSpeed = 0.5f;
    ID3D11RenderTargetView* renderTargetView;
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    DisplayWin32 Display;
    const DirectX::XMFLOAT4 position = DirectX::XMFLOAT4(-0.05f, -0.2f, 0.5f, 1.0f);
    float width = 0.1;
    float height = 0.4;
    float directionX; // 1 = right, -1 = left
    float directionY; // 1 = up, -1 = down
    void SetupViewport();
    void SetVertexAndPixelShaders();
    void SetBackBufferOutput(UINT NumViews, ID3D11RenderTargetView* const* ppRenderTargetViews, ID3D11DepthStencilView* pDepthStencilView);
    DirectX::BoundingBox GetPlayerBoundingBox() const;
    bool CheckBorderCollision();
    int ScoreCount;
private:
    float minClamp;
    float maxClamp;
    bool hitYUp, hitYDown;
};

#endif
