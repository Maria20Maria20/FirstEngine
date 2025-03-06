#include "Sun.h"

Sun::Sun(ID3D11Device* device, ID3DBlob* vertexBC, ID3D11VertexShader* vs, ID3D11PixelShader* ps,
	ID3D11RenderTargetView* rtv): Cube(device,vertexBC,vs,ps,rtv)
{
}

void Sun::Update(float deltaTime)
{
	RotateShape(DirectX::XMVectorSet(0, 1, 0, 1), .1, deltaTime);
	RotateShape(DirectX::XMVectorSet(1, 0, 0, 1), .1, deltaTime);
}
