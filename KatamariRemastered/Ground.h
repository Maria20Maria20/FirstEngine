#ifndef GROUND_H
#define GROUND_H
#include <CPP/GameObject.h>
#include "SimpleMath.h"
#include <CPP/Camera.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

class Ground : public GameObject
{
public:
	Ground() {};
	Ground(ID3D11Device* device, XMFLOAT3 startPosition, float changedScale = 1);
	void Update(float deltaTime) override;

	void applyGBufferPass(ID3D11DeviceContext* context) override;
	void applyLightPass(ID3D11DeviceContext* context) override;

private:
	float changedScale = 1;
	XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f);


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
	struct GroundPixelConstantBuffer {
		XMFLOAT3 cam_pos;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> pGroundPixelConstantBuffer;

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

};
#endif