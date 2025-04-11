#pragma once
#include "Player.h"
#include <CPP/Game.h>
#include "Ground.h"
#include "Item.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>


class StartGame : public Game 
{
public:
	StartGame();
private:
	Player* player = nullptr;
	Ground* plane;

	// --- Default BlendState (no transparency) ---
	ID3D11BlendState* defaultBlendState;
	FLOAT   defaultBlendFactor[4] = { 0.f };
	UINT    defaultSampleMask = 0;

	// --- Default RastState ---
	ID3D11RasterizerState* defaultRastState;

	// -- Flowers (items) BlendState ---
	ID3D11BlendState* flowersBlendState;

	// -- Kevin (items) BlendState ---
	ID3D11BlendState* KevinBlendState;
	// -- Kevin (items) RastState ---
	// CULL (CULL_BACK) other side of Kevin (to get rid of artifacts)
	ID3D11RasterizerState* KevinRastState;


	LightData lightData;
	ID3D11Buffer* lightConstPixelBuffer;
	int pointLightCount = 10;
	XMFLOAT3 pointLightInitPositions[10];

	std::vector<Item*> items;

	// Shadow
	Camera* lightViewCamera;
	UINT smSizeX = 512;
	UINT smSizeY = 512;
	Vector3 lightPos;
	ID3D11Texture2D* shadowTexture;
	ID3D11DepthStencilView* depthDSV[4];
	ID3D11ShaderResourceView* depthSRV;
	D3D11_VIEWPORT smViewport;
	
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler_PCF;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pShadowInputLayout;
	UINT currCascade = 0;

	Microsoft::WRL::ComPtr<ID3DBlob> pShadowShaderBytecodeBlob;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pShadowVertexShader;
	ID3D11Buffer* shadowTransformsConstantBuffer;
	ID3D11Buffer* cascadesConstantBuffer;
	std::vector<GameObject*> gameObjects;

	struct ShadowTransformData
	{
		DirectX::XMMATRIX lightView;
		DirectX::XMMATRIX lightProjection;
		DirectX::XMMATRIX shadowTransformFull;
	};
	struct CascadesData {
		ShadowTransformData cascades[4];
		DirectX::XMFLOAT4 distances[4];
	} cascadesData;

	//float cascadeBounds[5] = { 0.1f, 10.0f, 30.0f, 10.0f, 40.0f };
	float cascadeBounds[5] = { 0.1f, 10.0f, 30.0f, 80.0f, 200.0f };
	float frustrumBias = 8.0f;
	//float cascadeBounds[5] = { 10.0f, 40.0f, 100.0f, 400.0f, 500.0f };


	float deltaTime;
	void KatamariWindowLoop(std::chrono::steady_clock::time_point& PrevTime, float& totalTime, unsigned int& frameCount);
	void HandleMoveDown(Keys key);
	void HandleMouseMove(const InputDevice::MouseMoveEventArgs& args);
	void BindDsvAndSetNullRenderTarget();
	void DrawSceneToShadowMap();
};