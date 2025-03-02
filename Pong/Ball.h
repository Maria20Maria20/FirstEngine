#pragma once

#include <CPP/Square.h>
#include <CPP/DisplayWin32.h>
#include "PlayerPaddle.h"
#include "NPCPaddle.h"

class Ball : public Square
{
public:
    Ball();
    Ball(DirectX::XMFLOAT4 vertexPositions[4],
        DirectX::XMFLOAT4 colors[4],
        Microsoft::WRL::ComPtr<ID3D11Device> device,
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
        ID3DBlob* vertexBC, ID3D11RenderTargetView* rtv,
        ID3D11VertexShader* vs,
        ID3D11PixelShader* ps,
        DisplayWin32 display,
        DirectX::XMFLOAT4 startPos, PlayerPaddle* player,
        NPCPaddle* npc);

    void Update(float deltaTime); // move
    ID3D11RenderTargetView* renderTargetView;
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    DisplayWin32 Display;
    float Flicker;
    const DirectX::XMFLOAT4 position = DirectX::XMFLOAT4(-0.05f, -0.2f, 0.5f, 1.0f);
    float width = 0.1;
    float height = 0.1;
    float directionX; // 1 = right, -1 = left
    float directionY; // 1 = up, -1 = down
    void SetupViewport();
    void SetVertexAndPixelShaders();
    void SetBackBufferOutput(UINT NumViews, ID3D11RenderTargetView* const* ppRenderTargetViews, ID3D11DepthStencilView* pDepthStencilView);
    DirectX::BoundingBox GetBallBoundingBox() const;
    bool CheckBorderCollision();
private:
    PlayerPaddle* player;
    NPCPaddle* npc;
    float minClamp;
    float maxClamp;
    bool hitXPlayer, hitXNPC;
    bool hitY;
    bool hitPlayer, hitNPC;
    float defaultSpeed = 0.5f;
    float speed;
    float upgradeSpeed = 0.1f;
    float changedScaleX = 2.1f;
    float changedScaleY = 2.1f;
};

