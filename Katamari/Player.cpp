#include "Player.h"


Player::Player(ID3D11Device* device, ID3DBlob* vertexBC, ID3D11VertexShader* vs, ID3D11PixelShader* ps,
	ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* depthStencilView,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, XMFLOAT3 startPosition, Camera* camera,
	ObjectType objectType,
	float changedScale, LPCWSTR shaderFilePath
) : GameObject(device,
	vertexBC, vs, ps, rtv, depthStencilView, context, objectType, shaderFilePath)
{
	initRandomRotation = GetRandomRotateTransform();
	position = startPosition;
	position.y = radius;
	this->scale = changedScale;
	this->camera = camera;
}

void Player::Update(float deltaTime)
{
	Move(deltaTime);
	//RotateShape(rotationDirection, speedRotation, deltaTime);
	Matrix rotTransform = Matrix::CreateFromYawPitchRoll(currentRotation, 0.0f, 0.0f);
	Matrix spinTransform = Matrix::CreateFromYawPitchRoll(0.0f, currentSpin, 0.0f);

	mWorldMatrix = Matrix::CreateScale(scale)
		* initRandomRotation
		* spinTransform * rotTransform
		* Matrix::CreateTranslation(position);

	Matrix viewMat = camera->GetViewMatrix();
	Matrix projMat = camera->GetProjectionMatrix();

	// Update constant buffer
	cb.worldViewProj = mWorldMatrix * (XMMATRIX)(viewMat * projMat);

}

void Player::Move(float deltaTime)
{
	SlowDown(deltaTime);
	position.y = radius;
	position = position + deltaTime * velocity * GetMoveDir();
	currentSpin += spinSpeed * deltaTime;
}

Vector3 Player::GetCenterLocation()
{
	Vector3 center = Vector3(position);
	return center;
}

Vector3 Player::GetMoveDir()
{
	return Vector3::Transform(Vector3(0.0f, 0.0f, 1.0f), Matrix::CreateFromYawPitchRoll(currentRotation, 0.0f, 0.0f));
}

void Player::AddTurn(float direction, float deltaTime)
{
	currentRotation += direction * deltaTime * rotationSpeed;
	//currentRotation = max(-XM_PI, min(currentRotation, XM_PI));
	if (currentRotation > XM_PI) {
		currentRotation -= XM_2PI;
	}
	else if (currentRotation < -XM_PI) {
		currentRotation += XM_2PI;
	}
}

void Player::PushForward(float deltaTime)
{
	velocity = min(maxVelocity, velocity + deltaTime * 2 * acceleration);
	spinSpeed = velocity / radius;
}

void Player::SlowDown(float deltaTime)
{
	velocity = max(0.0, velocity - deltaTime * acceleration);
	spinSpeed = velocity / radius;
}
