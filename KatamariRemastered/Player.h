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
	Player() {};
	Player(ID3D11Device* device, XMFLOAT3 startPosition, float changedScale = 1.0f);
	void Update(float deltaTime) override;
	Matrix GetRelativeTransform();
	void Move(float deltaTime);
	void AddTurn(float direction, float deltaTime);
	
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

	//float radius = 1.0f;

	// увеличение м€чика
	float scale = 1.0f;

	Vector3 moveDirection;
	float faceDirection = 1;

private:
	Matrix initRandomRotation;

public:
	// GBuffer pass stuff

// Topology
	D3D11_PRIMITIVE_TOPOLOGY topology;
	// Vertex Buffer - mVertexBuffer;
	// IndexBuffer - mIndexBuffer;
	// VertexShader
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	// TransformCBuffer
	Microsoft::WRL::ComPtr<ID3D11Buffer> pTransformConstantBuffer;
	// InputLayout
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	// PixelShader
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	// PixelConstantBuffer
	struct PlayerPixelConstantBuffer {
		XMFLOAT3 cam_pos;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> pPlayerPixelConstantBuffer;

	// TextureB for pic
	Texture* pTexture;
	// Sampler for pic
	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
	// Rasterizer
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer;
	// BlendState
	Microsoft::WRL::ComPtr<ID3D11BlendState> pBlendState;
	// DepthStencilState
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDepthStencilState;


	void applyGBufferPass(ID3D11DeviceContext* context) override;
	void applyLightPass(ID3D11DeviceContext* context) override;
};


#endif