#include "Player.h"

Player::Player(ID3D11Device* device, ID3DBlob* vertexBC, ID3D11VertexShader* vs, ID3D11PixelShader* ps,
	ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* depthStencilView,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, XMFLOAT3 startPosition, Camera* camera,
	XMVECTOR rotationDirection, float rotateMove, ObjectType objectType,
	float changedScale, LPCWSTR shaderFilePath
) : GameObject(device,
	vertexBC, vs, ps, rtv, depthStencilView, context, objectType, shaderFilePath)
{
	position = startPosition;
	this->changedScale = changedScale;
	this->camera = camera;
	this->rotationDirection = rotationDirection;
	moveSpeed = rotateMove;
}

void Player::Update(float deltaTime)
{
	//RotateShape(rotationDirection, speedRotation, deltaTime);
	mWorldMatrix = mRotationMatrix * XMMatrixTranslation(position.x, position.y, position.z);


	Matrix viewMat = camera->GetViewMatrix();
	Matrix projMat = camera->GetProjectionMatrix();


	// Update constant buffer
	cb.worldViewProj = XMMatrixScaling(changedScale, changedScale, changedScale) * mWorldMatrix * (XMMATRIX)(viewMat * projMat);

}
