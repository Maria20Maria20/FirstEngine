#ifndef SUN_H
#define SUN_H
#include <CPP/Cube.h>
class Sun : public Cube 
{
public:
	Sun(ID3D11Device* device, ID3DBlob* vertexBC, ID3D11VertexShader* vs,
		ID3D11PixelShader* ps, ID3D11RenderTargetView* rtv,
		ID3D11DepthStencilView* depthStencilView,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
	void Update(float deltaTime);
};
#endif