#include "Sun.h"

Sun::Sun(ID3D11Device* device, ID3DBlob* vertexBC, ID3D11VertexShader* vs, ID3D11PixelShader* ps,
	ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* depthStencilView,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context): Cube(device,
		vertexBC,vs,ps,rtv, depthStencilView, context)
{
}

void Sun::Update(float deltaTime)
{
	RotateShape(DirectX::XMVectorSet(0, 1, 0, 1), .1, deltaTime);
	RotateShape(DirectX::XMVectorSet(1, 0, 0, 1), .1, deltaTime);
}
