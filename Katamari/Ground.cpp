#include "Ground.h"


Ground::Ground(ID3D11Device* device, ID3DBlob* vertexBC, ID3D11VertexShader* vs, ID3D11PixelShader* ps,
	ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* depthStencilView,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, XMFLOAT3 startPosition, Camera* camera,
	ObjectType objectType,
	float changedScale, LPCWSTR shaderFilePath
) : GameObject(device,
	vertexBC, vs, ps, rtv, depthStencilView, context, objectType, shaderFilePath)
{
	position = startPosition;
	this->changedScale = changedScale;
	this->camera = camera;
}

void Ground::Update(float deltaTime)
{
	//RotateShape(rotationDirection, speedRotation, deltaTime);
	mmWorldMatrixrix = XMMatrixTranslation(position.x, position.y, position.z);


	Matrix viewMat = camera->GetViewMatrix();
	Matrix projMat = camera->GetProjectionMatrix();


	// Update constant buffer
	cb.worldViewProj = XMMatrixScaling(changedScale, changedScale, changedScale) * mmWorldMatrixrix * (XMMATRIX)(viewMat * projMat);

}
