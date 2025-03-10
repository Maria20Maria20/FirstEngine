#ifndef SUN_H
#define SUN_H
#include <CPP/Sphere.h>
#include "SimpleMath.h"
#include "Camera.h"

using namespace DirectX::SimpleMath;


class Planet : public Sphere 
{
public:
	Planet(ID3D11Device* device, ID3DBlob* vertexBC, ID3D11VertexShader* vs,
		ID3D11PixelShader* ps, ID3D11RenderTargetView* rtv,
		ID3D11DepthStencilView* depthStencilView,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, XMFLOAT3 startPosition, float changedScale = 1, Planet* parentPlanet = nullptr);
	void Update(float deltaTime);
	Camera* camera;
private:
	Planet* parentPlanet;
	float orbitAngle;
	float moveSpeed = 1;
	float orbitRadius = 1.1f;
	float changedScale = 1;
	XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Matrix GetParentTransform();
	Vector3 GetCenterLocation();
};
#endif