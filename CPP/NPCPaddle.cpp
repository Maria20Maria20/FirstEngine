#include "NPCPaddle.h"

NPCPaddle::NPCPaddle()
{
}

NPCPaddle::NPCPaddle(DirectX::XMFLOAT4 vertexPositions[4],
    DirectX::XMFLOAT4 colors[4], DirectX::XMFLOAT2 startPosition,
    float minClamp, float maxClamp,
    Microsoft::WRL::ComPtr<ID3D11Device> device,
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
    ID3DBlob* vertexBC, ID3D11RenderTargetView* rtv,
    ID3D11VertexShader* vs, ID3D11PixelShader* ps, DisplayWin32 display)
    : Square(vertexPositions, colors, startPosition, device, context, vertexBC),
    minClamp(minClamp), maxClamp(maxClamp), direction(1.0f), speed(0.5f), renderTargetView(rtv),
    vertexShader(vs), pixelShader(ps), Display(display)
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
    MoveShape(0, direction * speed * deltaTime, 0);

    // Проверяем границы и отражаем направление
    float currentY = GetPosition().y;
    if (currentY > maxClamp)
    {
        direction = -1.0f;
    }
    else if (currentY < minClamp)
    {
        direction = 1.0f;
    }
}
void NPCPaddle::SetupViewport()
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
void NPCPaddle::SetVertexAndPixelShaders()
{
    context->VSSetShader(vertexShader, nullptr, 0);
    context->PSSetShader(pixelShader, nullptr, 0);
}
void NPCPaddle::SetBackBufferOutput(UINT NumViews, ID3D11RenderTargetView* const* ppRenderTargetViews, ID3D11DepthStencilView* pDepthStencilView)
{
    context->OMSetRenderTargets(NumViews, ppRenderTargetViews, pDepthStencilView);
}

