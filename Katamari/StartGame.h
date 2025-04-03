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
	XMFLOAT3 pointLightInitPositions[6];

	std::vector<Item*> items;

	float deltaTime;
	void KatamariWindowLoop(std::chrono::steady_clock::time_point& PrevTime, float& totalTime, unsigned int& frameCount);
	void HandleMoveDown(Keys key);
	void HandleMouseMove(const InputDevice::MouseMoveEventArgs& args);
};