#include "Planet.h"

Planet::Planet(ID3D11Device* device, ID3DBlob* vertexBC, ID3D11VertexShader* vs, ID3D11PixelShader* ps,
	ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* depthStencilView,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, XMFLOAT3 startPosition, float changedScale, Planet* parentPlanet) : Sphere(device,
		vertexBC, vs, ps, rtv, depthStencilView, context)
{
	position = startPosition;
	this->parentPlanet = parentPlanet;
	this->changedScale = changedScale;

}

void Planet::Update(float deltaTime)
{
	if (parentPlanet) //if parentPlanet not nullptr
	{
		orbitAngle += moveSpeed * deltaTime;
		auto _attractredTransform = GetParentTransform() * XMMatrixScaling(changedScale, changedScale, changedScale);
		cb.worldViewProj = (XMMATRIX)_attractredTransform;
	}
	else
	{
		RotateShape(DirectX::XMVectorSet(0, 1, 0, 1), 20, deltaTime);
		mWorldMatrix = mRotationMatrix * XMMatrixTranslation(position.x, position.y, position.z)
			* XMMatrixScaling(changedScale, changedScale, changedScale);
		// Update constant buffer
		cb.worldViewProj = mWorldMatrix; //*viewProj;
	}


	Matrix viewMat = camera->GetViewMatrix();
	Matrix projMat = camera->GetProjectionMatrix();


	cb.worldViewProj *= (XMMATRIX)(viewMat * projMat);

}

Matrix Planet::GetParentTransform() {
	Matrix res = Matrix::Identity;

	if (parentPlanet)
	{
		Matrix toOrbit = Matrix::CreateTranslation(Vector3(orbitRadius, 0.0f, 0.0f));

		Quaternion q_orbitRot = Quaternion::CreateFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), orbitAngle);
		Matrix m_orbitRot = Matrix::CreateFromQuaternion(q_orbitRot);

		Matrix toAttractedCenter = Matrix::CreateTranslation(parentPlanet->GetCenterLocation());

		res = toOrbit * m_orbitRot * toAttractedCenter;
	}

	return res;
}

Vector3 Planet::GetCenterLocation()
{
	Vector3 center = Vector3(position);
	center = Vector3::Transform(center, GetParentTransform());
	return center;
}
