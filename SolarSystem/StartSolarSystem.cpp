#include "StartSolarSystem.h"

StartSolarSystem::StartSolarSystem()
{
	Initialize();
}

int StartSolarSystem::InstanceObjects()
{
	camera = Camera();

	/*mCube = new Cube(device, vertexBC, vertexShader,
		pixelShader, rtv, depthStencilView, context);
	mCube2 = new Cube(device, vertexBC, vertexShader,
		pixelShader, rtv, depthStencilView, context);*/

	sun = new Planet(device, vertexBC, vertexShader,
		pixelShader, rtv, depthStencilView, context, XMFLOAT3(0.0f, 0.0f, .7f));
	sun->camera = &camera;
	satellite = new Planet(device, vertexBC, vertexShader,
		pixelShader, rtv, depthStencilView, context, XMFLOAT3(0.0f, 0.0f, 0.0f), .5f, sun);
	satellite->camera = &camera;
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
		float	deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - PrevTime).count() / 1000000.0f;
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


		/*mCube->RotateShape(DirectX::XMVectorSet(1, 0, 0, 1), .5, deltaTime);
		mCube->RotateShape(DirectX::XMVectorSet(0, 1, 0, 1), .5, deltaTime);
		mCube2->RotateShape(DirectX::XMVectorSet(0, 1, 0, 1), .5, deltaTime);
		mCube2->RotateShape(DirectX::XMVectorSet(0, 0, 1, 1), .5, deltaTime);*/

		//mCube->Update(deltaTime);
		//mCube2->Update(deltaTime);

		sun->Update(deltaTime);

		satellite->Update(deltaTime);

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
		//mCube->Draw(context, projection);

		//mCube2->Draw(context, projection);

		sun->Draw(context, projection);

		satellite->Draw(context, projection);

		swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
	}

}

