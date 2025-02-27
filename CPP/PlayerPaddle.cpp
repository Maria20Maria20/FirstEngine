#include "PlayerPaddle.h"

PlayerPaddle::PlayerPaddle()
{
}

PlayerPaddle::PlayerPaddle(DirectX::XMFLOAT4 vertexPositions[4],
    DirectX::XMFLOAT4 colors[4],
    Microsoft::WRL::ComPtr<ID3D11Device> device,
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
    ID3DBlob* vertexBC, ID3D11RenderTargetView* rtv,
    ID3D11VertexShader* vs, ID3D11PixelShader* ps, DisplayWin32 display,
    DirectX::XMFLOAT4 startPos)
    : Square(vertexPositions, colors, startPos, device, context, vertexBC),
    minClamp(minClamp), maxClamp(maxClamp), directionY(1.0f), directionX(0.0f), speed(0.5f), renderTargetView(rtv),
    vertexShader(vs), pixelShader(ps), Display(display), position(startPos)
{
}

void PlayerPaddle::Update(float deltaTime)
{
    UINT strides[] = { 32 };
    UINT offsets[] = { 0 };

    SetupIAStage(strides, offsets);
    context->RSSetState(rastState);
    SetupViewport();
    SetVertexAndPixelShaders();
    SetBackBufferOutput(1, &renderTargetView, nullptr);


    // Считываем ввод
    bool upPressed = GetAsyncKeyState(VK_UP) & 0x8000;
    bool downPressed = GetAsyncKeyState(VK_DOWN) & 0x8000;

    // Обновляем статус столкновения
    if (upPressed || downPressed)
    {
        CheckBorderCollision();
    }
    // Определяем направление движения
    if (upPressed && !hitYUp)
    {
        directionY = 1;
    }
    else if (downPressed && !hitYDown)
    {
        directionY = -1;
    }
    else
    {
        directionY = 0;
    }

    // Двигаем ракетку
    MoveShape(directionX * speed * deltaTime, directionY * speed * deltaTime, 0);
}
void PlayerPaddle::SetupViewport()
{
    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(Display.ScreenWidth);
    viewport.Height = static_cast<float>(Display.ScreenHeight);
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1.0f;

    context->RSSetViewports(1, &viewport);
}
void PlayerPaddle::SetVertexAndPixelShaders()
{
    context->VSSetShader(vertexShader, nullptr, 0);
    context->PSSetShader(pixelShader, nullptr, 0);
}
void PlayerPaddle::SetBackBufferOutput(UINT NumViews, ID3D11RenderTargetView* const* ppRenderTargetViews, ID3D11DepthStencilView* pDepthStencilView)
{
    context->OMSetRenderTargets(NumViews, ppRenderTargetViews, pDepthStencilView);
}
DirectX::BoundingBox PlayerPaddle::GetPlayerBoundingBox() const
{
    DirectX::BoundingBox bbox;

    DirectX::XMVECTOR center = DirectX::XMVectorSet(position.x,
        position.y,
        position.z, 1.0f);
    DirectX::XMFLOAT3 extents(width * 0.5f,
        height * 0.5f,
        0.0f);
    DirectX::XMStoreFloat3(&(bbox.Center), DirectX::XMVector4Transform(center, transformData.offset));

    bbox.Extents = extents;
    return bbox;
}
bool PlayerPaddle::CheckBorderCollision()
{
    DirectX::BoundingBox* screenBox = Display.GetScreenBorders();
    DirectX::BoundingBox paddleBox = GetPlayerBoundingBox();
    hitYUp = screenBox[0].Intersects(paddleBox);
    hitYDown = screenBox[1].Intersects(paddleBox);
    return hitYUp || hitYDown;
}


