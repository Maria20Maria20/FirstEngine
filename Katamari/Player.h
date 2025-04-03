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
	Player();
	Player(ID3D11Device* device, ID3D11VertexShader* vs,
		ID3D11PixelShader* ps, ID3D11RenderTargetView* rtv,
		ID3D11DepthStencilView* depthStencilView,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, XMFLOAT3 startPosition,
		Camera* camera, ObjectType objectType,
		float changedScale = 1.0f, LPCWSTR shaderFilePath = L"./Shaders/CubeShader.hlsl");
	void Update(float deltaTime);
	Matrix GetRelativeTransform();
	void Move(float deltaTime);
	void AddTurn(float direction, float deltaTime);
	
	
	Camera* camera;
	XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f);

	Vector3 GetCenterLocation();
	Vector3 GetMoveDir();
	void SlowDown(float deltaTime);
	void PushForward(float deltaTime);
	void PushBackward(float deltaTime);

	// кручение м€чика (как колесо)
	float spinSpeed = 0.0f;
	float currentSpin = 0.0f;

	// передвижение м€чика вперЄд
	float velocity = 0.0f;
	float maxVelocity = 10.0f;
	float acceleration = 10.0f;


	// дл€ поворота м€чика влево-вправо
	float rotationSpeed = 0.5f;
	float currentRotation = 0.0f;

	float radius = 1.0f;

	// увеличение м€чика
	float scale = 1.0f;

	Vector3 moveDirection;
	float faceDirection = 1;

private:
	Matrix initRandomRotation;

public:
	// void CreateInputLayout() override;
};


#endif