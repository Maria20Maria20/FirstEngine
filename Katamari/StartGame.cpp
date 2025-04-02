#include "StartGame.h"

int main()
{
	Assimp::Importer imp;
	auto testModel = imp.ReadFile("Models\\little_grass_1.fbx",
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices
	);
	StartGame* startGame = new StartGame();
	return 0;
}

StartGame::StartGame()
{
	Initialize();
	InputDevice::getInstance().OnKeyPressed.AddRaw(this, &StartGame::HandleMoveDown);
	InputDevice::getInstance().MouseMove.AddRaw(this, &StartGame::HandleMouseMove);

	plane = new Ground(device, vertexShader,
		pixelShader, rtv, depthStencilView, context, XMFLOAT3(0.0f, 0.0f, 0.0f),
		&camera, GameObject::ObjectType::PLANE, 3);
	player = new Player(device, vertexShader,
		pixelShader, rtv, depthStencilView, context, XMFLOAT3(0.0f, 0.0f, 0.0f), 
		&camera, GameObject::ObjectType::SPHERE);

	for (int i = 0; i < 10; ++i)
	{
		float rad = 1.0f;
		float x = (rand() % 100) - 50.0f;
		float z = (rand() % 100) - 50.0f;
		items.push_back(new Item(device, vertexBC, vertexShader,
			pixelShader, rtv, depthStencilView, context, 
			&camera, GameObject::ObjectType::CUBE, XMFLOAT3(x, 0.0f, z), rad, 1.0f)
		);
	}

	camera.SwitchToFollowMode(player->position, player->GetMoveDir(), player->radius);

	std::chrono::time_point<std::chrono::steady_clock> PrevTime = std::chrono::steady_clock::now();
	float totalTime = 0;
	unsigned int frameCount = 0;

	std::cout << player->scale << " " << player->radius << ";\n";

	KatamariWindowLoop(PrevTime, totalTime, frameCount);
}

void StartGame::KatamariWindowLoop(std::chrono::steady_clock::time_point& PrevTime, float& totalTime, unsigned int& frameCount)
{
	MSG msg = {};
	bool isExitRequested = false;
	while (!isExitRequested) {
		// Handle the windows messages.
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if (msg.message == WM_QUIT) {
			isExitRequested = true;
		}

		auto	curTime = std::chrono::steady_clock::now();
		deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - PrevTime).count() / 1000000.0f;
		PrevTime = curTime;

		totalTime += deltaTime;
		frameCount++;

		if (totalTime > 1.0f) {
			float fps = frameCount / totalTime;

			totalTime -= 1.0f;

			WCHAR text[256];
			swprintf_s(text, TEXT("FPS: %f"), fps);
			SetWindowText(display->hWnd, text);

			frameCount = 0;
		}


		for (Item* item : items)
		{
			item->Update(deltaTime);
		}
		player->Update(deltaTime);
		plane->Update(deltaTime);

		for (Item* item : items)
		{
			if (item->CheckCollision(*player))
			{
				std::cout << "COLLISION!\n";
				item->AttachToBall(player);
				// play.Grow(obj.radius / deltaTime);
			}
		}

		camera.Update(deltaTime, player->mWorldMatrix, player->GetMoveDir(), CameraFOV * player->radius);


		float color[] = { 0.0f, 0.1f, 0.5f, 1.0f };
		context->ClearRenderTargetView(rtv, color);
		context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1, 0);

		D3D11_VIEWPORT viewport = {};
		viewport.Width = static_cast<float>(Game::getInstance().display->ScreenWidth);
		viewport.Height = static_cast<float>(Game::getInstance().display->ScreenHeight);
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0;
		viewport.MaxDepth = 1.0f;
		context->RSSetViewports(1, &viewport);

		XMMATRIX projection = XMMatrixIdentity();

		plane->Draw(context, projection);






		ID3D11BlendState* old_blend_state;
		FLOAT   old_blend_factor[4] = { 0.f };
		UINT    old_sample_mask = 0;

		context->OMGetBlendState(&old_blend_state, old_blend_factor, &old_sample_mask);

		ID3D11BlendState* _blend_state = nullptr;

		D3D11_BLEND_DESC blendDesc;
		ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));

		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; //D3D11_BLEND_SRC_ALPHA
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; //D3D11_BLEND_INV_SRC_ALPHA
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		device->CreateBlendState(&blendDesc, &_blend_state);
		float blend_factor[4] = { 1.f, 1.f, 1.f, 1.f };
		context->OMSetBlendState(_blend_state, blend_factor, 0xffffffff);


		for (Item* item : items)
		{
			//std::cout << item.initialPosition.x << "\n";
			item->Draw(context, projection);
		}
		_blend_state->Release();

		ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));

		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		device->CreateBlendState(&blendDesc, &_blend_state);
		context->OMSetBlendState(_blend_state, blend_factor, 0xffffffff);

		CD3D11_RASTERIZER_DESC newRastDesc = {};
		ID3D11RasterizerState* newRastState;
		newRastDesc.CullMode = D3D11_CULL_BACK; //triangles in the specified direction do not need to be drawn
		newRastDesc.FillMode = D3D11_FILL_SOLID; //fill mode (solid or wireframe)
		device->CreateRasterizerState(&newRastDesc, &newRastState);
		context->RSSetState(newRastState);

		player->Draw(context, projection);

		_blend_state->Release();
		newRastState->Release();
		CD3D11_RASTERIZER_DESC oldRastDesc = {};
		ID3D11RasterizerState* oldRastState;
		oldRastDesc.CullMode = D3D11_CULL_NONE; //triangles in the specified direction do not need to be drawn
		oldRastDesc.FillMode = D3D11_FILL_SOLID; //fill mode (solid or wireframe)
		device->CreateRasterizerState(&oldRastDesc, &oldRastState);
		context->RSSetState(oldRastState);

		context->OMSetBlendState(old_blend_state, old_blend_factor, old_sample_mask);

		//if (focusedBody)
		//{
		//	//follow to planet
		//	camera.Update(deltaTime, focusedBody->mWorldMatrix);
		//}
		swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
	}

}


void StartGame::HandleMoveDown(Keys key)
{
	if (key == Keys::W) {
		player->PushForward(deltaTime * 30);
	}
	if (key == Keys::S) {
		player->PushBackward(deltaTime * 30);
	}
	if (key == Keys::A)
	{
		player->AddTurn(-1.0f, deltaTime);
	}
	if (key == Keys::D)
	{
		player->AddTurn(1.0f, deltaTime);
	}
}

void StartGame::HandleMouseMove(const InputDevice::MouseMoveEventArgs& args)
{
	player->AddTurn(args.Offset.x * 1.0f, deltaTime);
}
