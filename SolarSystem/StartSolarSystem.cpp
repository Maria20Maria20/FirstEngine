#include "StartSolarSystem.h"

StartSolarSystem::StartSolarSystem()
{
	Initialize();
	InputDevice::getInstance().OnKeyPressed.AddRaw(this, &StartSolarSystem::HandleMoveDown);
}

int StartSolarSystem::InstanceObjects()
{
	camera = Camera();

	sun = new Planet(device, vertexBC, vertexShader,
		pixelShader, rtv, depthStencilView, context, XMFLOAT3(0.0f, 0.0f, 0.0f), &camera, DirectX::XMVectorSet(0, 1, 0, 1), 1, GameObject::ObjectType::SPHERE, 3);
	satellite = new Planet(device, vertexBC, vertexShader,
		pixelShader, rtv, depthStencilView, context, XMFLOAT3(0.0f, 0.0f, 0.0f), &camera, DirectX::XMVectorSet(0, 1, 0, 1), 3,
		GameObject::ObjectType::SPHERE, .5f, sun, 1.3f);
	satellite2 = new Planet(device, vertexBC, vertexShader,
		pixelShader, rtv, depthStencilView, context, XMFLOAT3(0.0f, 0.0f, 0.0f), &camera, DirectX::XMVectorSet(0, 1, 0, 1), -3,
		GameObject::ObjectType::SPHERE, .5f, sun, 2.3f);
	satellite3 = new Planet(device, vertexBC, vertexShader,
		pixelShader, rtv, depthStencilView, context, XMFLOAT3(0.0f, 0.0f, 0.0f), &camera, DirectX::XMVectorSet(0, 1, 0, 1), 3,
		GameObject::ObjectType::SPHERE, .5f, sun, 3.3f);
	satellite4 = new Planet(device, vertexBC, vertexShader,
		pixelShader, rtv, depthStencilView, context, XMFLOAT3(0.0f, 0.0f, 0.0f), &camera, DirectX::XMVectorSet(0, 1, 0, 1), -3,
		GameObject::ObjectType::SPHERE, .5f, sun, 4.3f);
	satellite5 = new Planet(device, vertexBC, vertexShader,
		pixelShader, rtv, depthStencilView, context, XMFLOAT3(0.0f, 0.0f, 0.0f), &camera, DirectX::XMVectorSet(0, 1, 0, 1), 3,
		GameObject::ObjectType::SPHERE, .5f, sun, 5.3f);
	earth = new Planet(device, vertexBC, vertexShader,
		pixelShader, rtv, depthStencilView, context, XMFLOAT3(0.0f, 0.0f, 0.0f), &camera, DirectX::XMVectorSet(0, 1, 0, 1), 2, 
		GameObject::ObjectType::CUBE, 2, sun, 2.5f);
	moon = new Planet(device, vertexBC, vertexShader,
		pixelShader, rtv, depthStencilView, context, XMFLOAT3(0.0f, 0.0f, 0.0f), &camera, DirectX::XMVectorSet(0, 1, 0, 1), -4,
		GameObject::ObjectType::CUBE, .7f, earth, 0.5f);
	cubeEarth = new Planet(device, vertexBC, vertexShader,
		pixelShader, rtv, depthStencilView, context, XMFLOAT3(0.0f, 0.0f, 0.0f), &camera, DirectX::XMVectorSet(0, 1, 0, 1), -2, 
		GameObject::ObjectType::CUBE, 2, sun, 4.5f);
	cubeMoon = new Planet(device, vertexBC, vertexShader,
		pixelShader, rtv, depthStencilView, context, XMFLOAT3(0.0f, 0.0f, 0.0f), &camera, DirectX::XMVectorSet(0, 1, 0, 1), 4,
		GameObject::ObjectType::CUBE, .7f, cubeEarth, 0.5f);

	//planetSystem.Initialize(device, vertexBC, vertexShader, pixelShader, rtv, depthStencilView, context, &camera);
	//planetSystem.GenerateRandom(10);

	std::chrono::time_point<std::chrono::steady_clock> PrevTime = std::chrono::steady_clock::now();
	float totalTime = 0;
	unsigned int frameCount = 0;
	SolarSystemWindowLoop(PrevTime, totalTime, frameCount);
	return 0;
}

void StartSolarSystem::SolarSystemWindowLoop(std::chrono::steady_clock::time_point& PrevTime, float& totalTime, unsigned int& frameCount)
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


		sun->Update(deltaTime);
		satellite->Update(deltaTime);
		satellite2->Update(deltaTime);
		satellite3->Update(deltaTime);
		satellite4->Update(deltaTime);
		satellite5->Update(deltaTime);
		earth->Update(deltaTime);
		moon->Update(deltaTime);
		cubeEarth->Update(deltaTime);
		cubeMoon->Update(deltaTime);
		//planetSystem.Update(deltaTime);

		float color[] = { 0.0f, 0.1f, totalTime, 1.0f };
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

		sun->Draw(context, projection);
		satellite->Draw(context, projection);
		satellite2->Draw(context, projection);
		satellite3->Draw(context, projection);
		satellite4->Draw(context, projection);
		satellite5->Draw(context, projection);
		earth->Draw(context, projection);
		moon->Draw(context, projection);
		cubeEarth->Draw(context, projection);
		cubeMoon->Draw(context, projection);
		//planetSystem.Draw(context, projection);

		if (focusedBody)
		{
			//follow to planet
			camera.SwitchToOrbitalMode(focusedBody->GetCenterLocation(), Vector3(0.0f, 1.0f, 0.0f), focusedBody->orbitRadius * 5);
		}
		swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
	}

}


void StartSolarSystem::HandleMoveDown(Keys key)
{
	if (key == Keys::W) {
		camera.MoveForward(deltaTime * 5);
	}
	if (key == Keys::S) {
		camera.MoveBackward(deltaTime * 5);
	}
	if (key == Keys::A) {
		camera.MoveLeft(deltaTime * 5);
	}
	if (key == Keys::D) {
		camera.MoveRight(deltaTime * 5);
	}
	if (key == Keys::Q) {
		camera.RotatePitch(deltaTime * 5);
	}
	if (key == Keys::E) {
		camera.RotatePitch(-deltaTime * 5);
	}
	if (key == Keys::D1) {
		focusedBody = sun;
	}
	if (key == Keys::D2)
	{
		focusedBody = satellite;
	}
	if (key == Keys::D3)
	{
		camera.SwitchProjection();
	}

}

