#include "StartGame.h"

StartGame::StartGame()
{
	Initialize();
	InputDevice::getInstance().OnKeyPressed.AddRaw(this, &StartGame::HandleMoveDown);
	InputDevice::getInstance().MouseMove.AddRaw(this, &StartGame::HandleMouseMove);

	plane = new Ground(device, vertexBC, vertexShader,
		pixelShader, rtv, depthStencilView, context, XMFLOAT3(0.0f, 0.0f, 0.0f),
		&camera, GameObject::ObjectType::PLANE, 1);
	player = new Player(device, vertexBC, vertexShader,
		pixelShader, rtv, depthStencilView, context, XMFLOAT3(0.0f, 0.0f, 0.0f), 
		&camera, GameObject::ObjectType::SPHERE, 1);

	for (int i = 0; i < 10; ++i)
	{
		float rad = 0.3f;
		float x = (rand() % 100) - 50.0f;
		float z = (rand() % 100) - 50.0f;
		items.emplace_back(device, vertexBC, vertexShader,
			pixelShader, rtv, depthStencilView, context, 
			&camera, GameObject::ObjectType::CUBE, XMFLOAT3(x, 0.0f, z), 1.0f, 1.0f
		);
	}

	camera.SwitchToFollowMode(player->position, player->GetMoveDir(), player->radius);

	std::chrono::time_point<std::chrono::steady_clock> PrevTime = std::chrono::steady_clock::now();
	float totalTime = 0;
	unsigned int frameCount = 0;
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

		for (Item item : items)
		{
			item.Update(deltaTime);
		}
		player->Update(deltaTime);
		plane->Update(deltaTime);
		camera.Update(deltaTime, Matrix::CreateScale(player->scale) * player->mWorldMatrix, player->GetMoveDir(), player->radius);

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

		player->Draw(context, projection);
		plane->Draw(context, projection);
		for (Item item : items)
		{
			//std::cout << item.initialPosition.x << "\n";
			item.Draw(context, projection);
		}
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
		// player->Move(true, -1, deltaTime * 50);
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
