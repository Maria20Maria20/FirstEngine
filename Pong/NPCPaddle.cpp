#include "NPCPaddle.h"

NPCPaddle::NPCPaddle()
{
}

NPCPaddle::NPCPaddle(DirectX::XMFLOAT4 vertexPositions[4],
    DirectX::XMFLOAT4 colors[4],
    Microsoft::WRL::ComPtr<ID3D11Device> device,
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
    ID3DBlob* vertexBC, ID3D11RenderTargetView* rtv,
    ID3D11VertexShader* vs, ID3D11PixelShader* ps, DisplayWin32* display,
    DirectX::XMFLOAT4 startPos)
    : Square(vertexPositions, colors, startPos, device, context, vertexBC),
    directionY(1.0f), directionX(0.0f), speed(0.5f), renderTargetView(rtv),
    vertexShader(vs), pixelShader(ps), Display(display), position(startPos)
{
}

void NPCPaddle::Update(float deltaTime)
{
    UINT strides[] = { 32 };
    UINT offsets[] = { 0 };

    SetupIAStage(strides, offsets);
    context->RSSetState(rastState);

    SetupViewport();

    //square->SetupIAStage(strides, offsets);
    SetVertexAndPixelShaders();

    SetBackBufferOutput(1, &renderTargetView, nullptr);

    // Двигаем ракетку
    MoveShape(directionX * speed * deltaTime, directionY * speed * deltaTime, 0);
    if (CheckBorderCollision())
    {
        if (hitX)
        {
            directionX *= -1;
        }if (hitY)
        {
            directionY *= -1;
        }
    }
}
void NPCPaddle::SetupViewport()
{
    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(Display->ScreenWidth);
    viewport.Height = static_cast<float>(Display->ScreenHeight);
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1.0f;

    context->RSSetViewports(1, &viewport);
}
void NPCPaddle::SetVertexAndPixelShaders()
{
    context->VSSetShader(vertexShader, nullptr, 0);
    context->PSSetShader(pixelShader, nullptr, 0);
}
void NPCPaddle::SetBackBufferOutput(UINT NumViews, ID3D11RenderTargetView* const* ppRenderTargetViews, ID3D11DepthStencilView* pDepthStencilView)
{
    context->OMSetRenderTargets(NumViews, ppRenderTargetViews, pDepthStencilView);
}
DirectX::BoundingBox NPCPaddle::GetNPCBoundingBox() const
{
    DirectX::BoundingBox bbox;

    DirectX::XMVECTOR center = DirectX::XMVectorSet(position.x,
        position.y,
        position.z, 1.0f);
    DirectX::XMFLOAT3 extents(width * 0.5f,
        height * 0.5,
        0.0f);
    DirectX::XMStoreFloat3(&(bbox.Center), DirectX::XMVector4Transform(center, transformData.translation));

    bbox.Extents = extents;
    return bbox;
}
bool NPCPaddle::CheckBorderCollision()
{
    DirectX::BoundingBox* screenBox = Display->GetScreenBorders();
    DirectX::BoundingBox paddleBox = GetNPCBoundingBox();
    hitY = screenBox[0].Intersects(paddleBox) || screenBox[1].Intersects(paddleBox);
    hitX = screenBox[2].Intersects(paddleBox) || screenBox[3].Intersects(paddleBox);
    return hitX || hitY;
}


