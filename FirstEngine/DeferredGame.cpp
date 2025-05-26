#include "DeferredGame.h"

DeferredGame::DeferredGame()
{
	renderingSystem = new DeferredRenderer(displayWindow);
}

DeferredGame::DeferredGame(LPCWSTR appName)
{
	displayWindow = new DisplayWin32();
	displayWindow->applicationName = appName;
	renderingSystem = new DeferredRenderer(displayWindow);
}

void DeferredGame::Run()
{
	unsigned int frameCount = 0;
	float totalTime = 0;

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
		timer.Tick();
		deltaTime = timer.GetDeltaTime();
		totalTime += deltaTime;
		frameCount++;

		if (totalTime > 1.0f) {
			float fps = frameCount * 1.0f / totalTime;

			totalTime -= 1.0f;

			WCHAR text[256];
			swprintf_s(text, TEXT("%s. FPS: %f"), displayWindow->applicationName, fps);
			SetWindowText(displayWindow->hWnd, text);

			frameCount = 0;
		}

		Update(deltaTime);
		Render();
	}
}

void DeferredGame::Render()
{
	renderingSystem->RenderScene(scene);
}

DeferredGame::~DeferredGame()
{
	// Освобождение ресурсов
}
