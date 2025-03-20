#include "Planet.h"

Planet::Planet(ID3D11Device* device, ID3DBlob* vertexBC, ID3D11VertexShader* vs, ID3D11PixelShader* ps,
	ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* depthStencilView,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, XMFLOAT3 startPosition, Camera* camera,
	XMVECTOR rotationDirection, float rotateMove, ObjectType objectType,
	float changedScale, Planet* parentPlanet,
	float orbitRadius, LPCWSTR shaderFilePath
	) : GameObject(device,
		vertexBC, vs, ps, rtv, depthStencilView, context, objectType, shaderFilePath)
{
	position = startPosition;
	this->parentPlanet = parentPlanet;
	this->changedScale = changedScale;
	this->camera = camera;
	this->rotationDirection = rotationDirection;
	moveSpeed = rotateMove;
	if (parentPlanet)
	{
		//orbitRadius = parentPlanet->orbitRadius * 0.5;
		this->orbitRadius = orbitRadius;
		//position = 
	}
}

void Planet::Update(float deltaTime)
{
	RotateShape(rotationDirection, speedRotation, deltaTime);
	mWorldMatrix = mRotationMatrix * XMMatrixTranslation(position.x, position.y, position.z);

	if (parentPlanet) //if parentPlanet not nullptr
	{
		orbitAngle += moveSpeed * deltaTime;
		auto _attractredTransform = GetParentTransform();
		mWorldMatrix *= (XMMATRIX)_attractredTransform;
	}
	
	Matrix viewMat = camera->GetViewMatrix();
	Matrix projMat = camera->GetProjectionMatrix();


	// Update constant buffer
	cb.worldViewProj = XMMatrixScaling(changedScale, changedScale, changedScale) * mWorldMatrix * (XMMATRIX)(viewMat * projMat);

}

Matrix Planet::GetParentTransform() {
	Matrix res = Matrix::Identity;

	if (parentPlanet)
	{
		Matrix toOrbit = Matrix::CreateTranslation(Vector3(orbitRadius, 0.0f, 0.0f));

		Quaternion q_orbitRot = Quaternion::CreateFromAxisAngle(rotationDirection, orbitAngle);
		Matrix m_orbitRot = Matrix::CreateFromQuaternion(q_orbitRot);

		Matrix toAttractedCenter = Matrix::CreateTranslation(parentPlanet->GetCenterLocation());

		res = toOrbit * m_orbitRot * toAttractedCenter;
	}

	return res;
}

Vector3 Planet::GetCenterLocation()
{
	Vector3 center = Vector3(position);
	if (parentPlanet)
		center = Vector3::Transform(center, GetParentTransform());
	return center;
}