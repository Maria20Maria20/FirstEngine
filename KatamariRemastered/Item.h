#pragma once
#include <filesystem>
#include <random>
#include <CPP/GameObject.h>
#include <CPP/LoadModel.h>
#include "Player.h"

class Item :
    public GameObject
{
public:
    // Item(ID3D11Device* device, const std::string& modelsFolder, const DirectX::XMFLOAT3& position);
    Item(ID3D11Device* device,
        float radius, float changedScale);

    void Update(float deltaTime) override;
    void LoadRandomModel(const std::string& folder);
    std::vector<std::string> GetModelList(const std::string& modelsFolder);
    Vector3 GetCenterLocation();
    bool CheckCollision(Player& ball);
    void AttachToBall(Player* ball);

    //float radius;
    float modelRadius = 0.5f;
    bool isAttached = false;
    float appliedScale = 1.0f;

	Player* player = nullptr;

private:

    Matrix attachTransform;

    Matrix initRandomRotation;

public:
    DirectX::XMFLOAT3 initialPosition;

    float scale = 1.0f;

public:
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
	struct ItemPixelConstantBuffer {
		XMFLOAT3 cam_pos;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> pItemPixelConstantBuffer;

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


	void applyGBufferPass(ID3D11DeviceContext* context) override;
	void applyLightPass(ID3D11DeviceContext* context) override;

};
