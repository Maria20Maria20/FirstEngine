#include "PongGame.h"
#include <CPP/Cube.h>
PongGame::PongGame()
{
}

int PongGame::InstanceObjects()
{
	Initialize();

	DirectX::XMFLOAT4 VertexPositions[3] = {
	DirectX::XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f),
	DirectX::XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f),
	DirectX::XMFLOAT4(0.5f, -0.5f, 0.5f, 1.0f),
	};
	DirectX::XMFLOAT4 VertexPositionsSquare[4] = {
	DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
	DirectX::XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f),
	DirectX::XMFLOAT4(0.5f, -0.5f, 0.5f, 1.0f),
	DirectX::XMFLOAT4(-0.5f, 0.5f, 0.5f, 1.0f),
	};

	DirectX::XMFLOAT4 Colors[3] = {
	DirectX::XMFLOAT4(0.7f, 0.08f, 0.9f, 1.0f),
	DirectX::XMFLOAT4(0.3f, 0.06f, 0.9f, 0.5f),
	DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f),
	};
	DirectX::XMFLOAT4 ColorsSquare[4] = {
	DirectX::XMFLOAT4(0.7f, 0.08f, 0.9f, 1.0f),
	DirectX::XMFLOAT4(0.3f, 0.06f, 0.9f, 0.5f),
	DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f),
	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
	};

	DirectX::XMFLOAT2 StartPosition = { 0.0f, 0.0f };
	DirectX::XMFLOAT2 StartPositionSquare = { 0.5f, 0.5f };

	DirectX::XMFLOAT4 ballVertices[4] = {
		{-0.05f, 0.05f, 0.5f, 1.0f}, //0 point
		{0.05f, -0.05f, 0.5f, 1.0f}, //1 point
		{-0.05f, -0.05f, 0.5f, 1.0f}, //2 point
		{0.05f, 0.05f, 0.5f, 1.0f}, //3 point
	};

	DirectX::XMFLOAT4 paddleVertices[4] = {
	{-0.05f, 0.2f, 0.5f, 1.0f}, //0 point
	{0.05f, -0.2f, 0.5f, 1.0f}, //1 point
	{-0.05f, -0.2f, 0.5f, 1.0f}, //2 point
	{0.05f, 0.2f, 0.5f, 1.0f}, //3 point
	};


	DirectX::XMFLOAT4 paddleColors[4] = {
		{1.0f, 1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f, 1.0f}
	};

	DirectX::XMFLOAT2 startPosition = { 0.9f, 0.0f }; // Левый край экрана
	DirectX::XMFLOAT4 startPosition2 = { 0.9f, 0.0f, 0, 1 }; // right screen side
	DirectX::XMFLOAT4 startPosition3 = { -0.9f, 0.0f, 0, 1 }; // left screen side
	DirectX::XMFLOAT4 startPosition4 = { 0.0f, 0.0f, 0, 1 }; // middle screen side

	npcPaddle = new NPCPaddle(paddleVertices, paddleColors, device, context, vertexBC, rtv, vertexShader, pixelShader,
		display, startPosition2);
	npcPaddle->InitializeShape(4);
	playerPaddle = new PlayerPaddle(paddleVertices, paddleColors, device, context, vertexBC, rtv, vertexShader, pixelShader,
		display, startPosition3);
	playerPaddle->InitializeShape(4);
	ball = new Ball(ballVertices, paddleColors, device, context, vertexBC, rtv, vertexShader, pixelShader,
		display, startPosition4, playerPaddle, npcPaddle);
	ball->InitializeShape(4);

	// Начальная позиция куба
	DirectX::XMFLOAT4 startPos = { 0.0f, 0.0f, 0.0f, 1.0f };
	mCube = std::make_unique<Cube>(device, vertexBC, vertexShader, pixelShader, rtv, depthStencilView);


	//triangles.push_back(*new Triangle(VertexPositions, Colors, StartPosition, device, context, vertexBC));
	//triangles.push_back(*new Triangle(VertexPositions, Colors, StartPositionSquare, device, context, vertexBC));

	//triangleComponent->InitializeShape(triangle.VertexPositions, triangle.Colors, std::size(triangle.VertexPositions), triangle.StartPosition);
	//square->InitializeShape(std::size(square->VertexPositions));
	//for (auto& currentTriangle : triangles)
	//{
	//	currentTriangle.InitializeShape(std::size(currentTriangle.VertexPositions));
	//}

	//triangleComponent->InitializeShape(square.VertexPositions, square.Colors, std::size(square.VertexPositions), square.StartPosition);

	//SetupRasterizerStage();
	std::chrono::time_point<std::chrono::steady_clock> PrevTime = std::chrono::steady_clock::now();
	float totalTime = 0;
	unsigned int frameCount = 0;
	PongWindowLoop(PrevTime, totalTime, frameCount);
	return 0;
}

void PongGame::PongWindowLoop(std::chrono::steady_clock::time_point& PrevTime, float& totalTime, unsigned int& frameCount)
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

			totalTime -= 1/ball->Flicker;
			//totalTime -= 1.0f;

			WCHAR text[256];
			swprintf_s(text, TEXT("FPS: %f"), fps);
			SetWindowText(display->hWnd, text);

			frameCount = 0;
		}


		context->ClearState();



		UINT strides[] = { 32 };
		UINT offsets[] = { 0 };

		float color[] = { 0.0f, 0.1f, totalTime, 1.0f };
		context->ClearRenderTargetView(rtv, color);
		context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1, 0);
		//for (auto& currentTriangle : triangles)
		//{
		//	currentTriangle.SetupIAStage(strides, offsets);
		//	context->RSSetState(rastState);

		//	SetupViewport();

		//	//square->SetupIAStage(strides, offsets);
		//	SetVertexAndPixelShaders();

		//	SetBackBufferOutput(1, &rtv, nullptr);


		//	currentTriangle.MoveShape(currentTriangle.MoveSpeed
		//		* deltaTime * currentTriangle.DirectionX,
		//		currentTriangle.MoveSpeed
		//		* deltaTime * currentTriangle.DirectionY, 0);

		//	context->DrawIndexed(6, 0, 0);
		//}
		npcPaddle->Update(deltaTime);
		playerPaddle->Update(deltaTime);
		ball->Update(deltaTime);
		mCube->Update(deltaTime);

		XMMATRIX projection = XMMatrixIdentity();
		mCube->Draw(context, projection);
		
		swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
	}
}
