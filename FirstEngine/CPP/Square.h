#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <vector>
#include <wrl.h>
#include <stdexcept>
#include <DirectXCollision.h>
#include "Game.h"

using namespace DirectX;

struct CBTransform { //constant buffer transform
	XMMATRIX translation = XMMatrixIdentity();
};

class Square
{
public:
	Square();
	Square(XMFLOAT4 vertexPositions[4],
		XMFLOAT4 colors[4],
		XMFLOAT4 startPosition,
		Microsoft::WRL::ComPtr<ID3D11Device> device,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
		ID3DBlob* vertexBC);
	XMFLOAT4 VertexPositions[4];
	XMFLOAT4 Colors[4];
	XMFLOAT4 StartPosition;
	void InitializeShape(int count);

	float MoveSpeed = 0.1f;
protected:
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	ID3D11RasterizerState* rastState;
	CBTransform transformData;
	void MoveShape(float dx, float dy, float dz);
	void RotateShape(FXMVECTOR Axis, FLOAT Angle);
	void ScalingShape(float scaleFactorX, float scaleFactorY, float scaleFactorZ);
	void SetupIAStage(UINT  strides[1], UINT  offsets[1]);	
	void SetupViewport();
private:
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	ID3D11Buffer* vb;  // vertex buffer
	ID3D11Buffer* ib;  // index buffer
	ID3D11Buffer* cbTransform = nullptr; //constant buffer for transform
	ID3D11InputLayout* layout;
	int vertexCount = 0;
	ID3DBlob* vertexBC = nullptr;
	float DirectionX = -1.0f;
	float DirectionY = 0.0f;
	void CreateInputLayout();
	void CreateConstantBuffer();
	void CreateCubeVertexBuffer(XMFLOAT4 points[], int count, const XMFLOAT4& translation);
	void CreateCubeIndexBuffer();
};