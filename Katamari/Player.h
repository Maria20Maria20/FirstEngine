#ifndef PLAYER_H
#define PLAYER_H
#include <CPP/GameObject.h>
#include "SimpleMath.h"
#include <CPP/Camera.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

class Player : public GameObject
{
public:
	Player(ID3D11Device* device, ID3DBlob* vertexBC, ID3D11VertexShader* vs,
		ID3D11PixelShader* ps, ID3D11RenderTargetView* rtv,
		ID3D11DepthStencilView* depthStencilView,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, XMFLOAT3 startPosition,
		Camera* camera,
		XMVECTOR rotateDirection, float rotateMove, ObjectType objectType,
		float changedScale = 1, LPCWSTR shaderFilePath = L"./Shaders/CubeShader.hlsl");
	void Update(float deltaTime);
	Camera* camera;
	float speedRotation = 20;
private:
	float moveSpeed = .7f;
	float changedScale = 1;
	XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMVECTOR rotationDirection;
};
#endif