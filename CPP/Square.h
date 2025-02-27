#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <vector>
#include <wrl.h>
#include <stdexcept>
#include <DirectXCollision.h>

struct CBTransform { //constant buffer transform
	DirectX::XMMATRIX offset = DirectX::XMMatrixIdentity();
};

class Square
{
public:
	Square();
	Square(DirectX::XMFLOAT4 vertexPositions[4],
		DirectX::XMFLOAT4 colors[4],
		DirectX::XMFLOAT4 startPosition,
		Microsoft::WRL::ComPtr<ID3D11Device> device,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
		ID3DBlob* vertexBC);
	DirectX::XMFLOAT4 VertexPositions[4];
	DirectX::XMFLOAT4 Colors[4];
	DirectX::XMFLOAT4 StartPosition;
	void InitializeShape(int count);
	void MoveShape(float dx, float dy, float dz);

	float MoveSpeed = 0.1f;
	float DirectionX = -1.0f;
	float DirectionY = 0.0f;
	void SetupIAStage(UINT  strides[1], UINT  offsets[1]);	
protected:
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	ID3D11RasterizerState* rastState;
	CBTransform transformData;
private:
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	ID3D11Buffer* vb;  // vertex buffer
	ID3D11Buffer* ib;  // index buffer
	ID3D11Buffer* cbTransform = nullptr; //constant buffer for transform
	ID3D11InputLayout* layout;
	int vertexCount = 0;
	ID3DBlob* vertexBC = nullptr;
	std::vector<DirectX::XMFLOAT4> currentShape;
	void CreateInputLayout();
	void CreateVertexBuffer(DirectX::XMFLOAT4 points[], int count, const DirectX::XMFLOAT4& offset);
	void CreateIndexBuffer();
};