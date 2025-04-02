#include "Player.h"


Player::Player(ID3D11Device* device, ID3D11VertexShader* vs, ID3D11PixelShader* ps,
	ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* depthStencilView,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, XMFLOAT3 startPosition, Camera* camera,
	ObjectType objectType,
	float changedScale, LPCWSTR shaderFilePath)
	// : GameObject(device, vs, ps, rtv, depthStencilView, context, objectType, shaderFilePath)
{

	mVertexShader = vs;
	mPixelShader = ps;
	renderTargetView = rtv;
	this->depthStencilView = depthStencilView;
	this->device = device;
	this->context = context;
	currentObject = objectType;
	this->shaderFilePath = L"./Shaders/KevinShader.hlsl";

	InitializeBuffers();
	InitializeShaders();

	{
		this->numInputElements = 4;
		this->IALayoutInputElements = (D3D11_INPUT_ELEMENT_DESC*)malloc(this->numInputElements * sizeof(D3D11_INPUT_ELEMENT_DESC));
		this->IALayoutInputElements[0] =
			D3D11_INPUT_ELEMENT_DESC{
				"POSITION",
				0,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				0,
				0,
				D3D11_INPUT_PER_VERTEX_DATA,
				0 };

		this->IALayoutInputElements[1] =
			D3D11_INPUT_ELEMENT_DESC{
				"COLOR",
				0,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0 };

		this->IALayoutInputElements[2] =
			D3D11_INPUT_ELEMENT_DESC{
				"TEXCOORD",
				0,
				DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0 };
		this->IALayoutInputElements[3] =
			D3D11_INPUT_ELEMENT_DESC{
				"NORMAL",
				0,
				DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0 };

		device->CreateInputLayout(
			this->IALayoutInputElements,
			this->numInputElements,
			vsBlob->GetBufferPointer(),
			vsBlob->GetBufferSize(),
			&mInputLayout);
	}

	initRandomRotation = Matrix::CreateFromYawPitchRoll(XM_PIDIV2,0,0);// GetRandomRotateTransform();
	position = startPosition;
	position.y = radius;
	this->scale = changedScale;
	this->camera = camera;

	std::string model_name = "Kevin";
	std::string texture_name = "..\\Textures\\" + model_name + "_Diffuse.dds";
	this->textures.push_back(Texture(device, texture_name, aiTextureType_DIFFUSE));
}

void Player::Update(float deltaTime)
{
	Move(deltaTime);
	//RotateShape(rotationDirection, speedRotation, deltaTime);
	Matrix spinTransform = Matrix::CreateFromYawPitchRoll(0.0f, currentSpin, 0.0f);
	Matrix rotTransform = Matrix::CreateFromYawPitchRoll(currentRotation, 0.0f, 0.0f);

	mWorldMatrix = 
		Matrix::CreateScale(radius)
		* initRandomRotation
		* spinTransform * rotTransform
		* Matrix::CreateTranslation(position);

	Matrix viewMat = camera->GetViewMatrix();
	Matrix projMat = camera->GetProjectionMatrix();

	// Update constant buffer
	cb.worldViewProj = mWorldMatrix * (XMMATRIX)(viewMat * projMat);
	cb.cameraPosition = camera->GetPosition();
	cb.worldMat = mWorldMatrix;
	DirectX::XMMATRIX A = mWorldMatrix;
	A.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	DirectX::XMVECTOR det = XMMatrixDeterminant(A);
	cb.worldMatInvTranspose = DirectX::XMMatrixTranspose(XMMatrixInverse(&det, A));
}

void Player::Move(float deltaTime)
{
	SlowDown(deltaTime);
	position.y = radius;
	position = position + deltaTime * velocity * GetMoveDir() * faceDirection;
	currentSpin += spinSpeed * deltaTime;
}

Vector3 Player::GetCenterLocation()
{
	return position;
}

Vector3 Player::GetMoveDir()
{
	return Vector3::Transform(Vector3(0.0f, 0.0f, faceDirection), Matrix::CreateFromYawPitchRoll(currentRotation, 0.0f, 0.0f));
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
	faceDirection = 1;
}
void Player::PushBackward(float deltaTime)
{
	velocity = min(maxVelocity, velocity + deltaTime * 2 * acceleration);
	spinSpeed = velocity / radius;
	faceDirection = -1;
}

void Player::SlowDown(float deltaTime)
{
	velocity = max(0.0, velocity - deltaTime * acceleration);
	spinSpeed = velocity / radius;
}
