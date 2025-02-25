#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <vector>
#include <wrl.h>

class Triangle 
{
public:
	Triangle();
	Triangle(DirectX::XMFLOAT4 vertexPositions[3],
		DirectX::XMFLOAT4 colors[3], DirectX::XMFLOAT2 startPosition,
		Microsoft::WRL::ComPtr<ID3D11Device> device,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
		ID3DBlob* vertexBC);
	DirectX::XMFLOAT4 VertexPositions[3];
	DirectX::XMFLOAT4 Colors[3];
	DirectX::XMFLOAT2 StartPosition;
	void InitializeShape(int count);
	void MoveShape(float dx, float dy, float dz);

	float MoveSpeed = 0.1f;
	float DirectionX = 1.0f;
	float DirectionY = 0.5f;
	void SetupIAStage(UINT  strides[1], UINT  offsets[1]);

private:
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	ID3D11Buffer* vb;  // vertex buffer
	ID3D11Buffer* ib;  // index buffer
	ID3D11InputLayout* layout;
	int vertexCount = 0;
	ID3DBlob* vertexBC = nullptr;
	std::vector<DirectX::XMFLOAT4> currentShape;
	void CreateInputLayout();
	void CreateVertexBuffer(DirectX::XMFLOAT4 points[], int count, const DirectX::XMFLOAT2& offset);
	void CreateIndexBuffer();
};