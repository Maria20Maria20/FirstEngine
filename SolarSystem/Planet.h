#ifndef SUN_H
#define SUN_H
#include <CPP/GameObject.h>
#include "SimpleMath.h"
#include <CPP/Camera.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

class Planet : public GameObject 
{
public:
	Planet(ID3D11Device* device, ID3D11VertexShader* vs,
		ID3D11PixelShader* ps, ID3D11RenderTargetView* rtv,
		ID3D11DepthStencilView* depthStencilView,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, XMFLOAT3 startPosition,
		Camera* camera,
		XMVECTOR rotateDirection, float rotateMove, ObjectType objectType,
		float changedScale = 1, Planet* parentPlanet = nullptr, float orbitRadius = 0.0f, LPCWSTR shaderFilePath = L"./Shaders/CubeShader.hlsl");
	void Update(float deltaTime);
	Vector3 GetCenterLocation();
	float orbitRadius = 1;
	Camera* camera;
	Planet* parentPlanet;
	float speedRotation = 20;
private:
	float moveSpeed = .7f;
	float orbitAngle;
	float changedScale = 1;
	XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMVECTOR rotationDirection;
	Matrix GetParentTransform();
};
#endif