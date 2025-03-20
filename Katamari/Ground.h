#ifndef GROUND_H
#define GROUND_H
#include <CPP/GameObject.h>
#include "SimpleMath.h"
#include <CPP/Camera.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

class Ground : public GameObject
{
public:
	Ground();
	Ground(ID3D11Device* device, ID3DBlob* vertexBC, ID3D11VertexShader* vs,
		ID3D11PixelShader* ps, ID3D11RenderTargetView* rtv,
		ID3D11DepthStencilView* depthStencilView,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, XMFLOAT3 startPosition,
		Camera* camera, ObjectType objectType,
		float changedScale = 1, LPCWSTR shaderFilePath = L"./Shaders/CubeShader.hlsl");
	void Update(float deltaTime);
	Camera* camera;
private:
	float changedScale = 1;
	XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f);
};
#endif