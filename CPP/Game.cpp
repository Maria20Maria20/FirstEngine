#include "Game.h"

Game::Game()
{

}

int Game::Initialize()
{
	display = DisplayWin32();
	display.InitWindow();
	res = InitSwapChain();
	GetBackBufferAndCreateRTV();
	retVal = CompileShaders();
	if (retFlag) return retVal;
	//CreateInputLayout();
	//triangleComponent = new TriangleComponent(device, context);

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

	npcPaddle = new NPCPaddle(paddleVertices, paddleColors, startPosition,
		-0.8f, 0.8f, device, context, vertexBC, rtv, vertexShader, pixelShader,
		display);
	npcPaddle->InitializeShape(4);

	//square = new Square(VertexPositionsSquare, ColorsSquare, StartPositionSquare, device, context, layout);
	//squares.push_back(*square);
	//squares.push_back(*new Square(VertexPositionsSquare, ColorsSquare, StartPosition, device, context, vertexBC));
	//squares.push_back(*new Square(VertexPositionsSquare, ColorsSquare, StartPositionSquare, device, context, vertexBC));
	//squares.push_back(*npcPaddle);
	//triangles.push_back(*new Triangle(VertexPositions, Colors, StartPosition, device, context, vertexBC));
	//triangles.push_back(*new Triangle(VertexPositions, Colors, StartPositionSquare, device, context, vertexBC));
	//squares.push_back(*new Square(VertexPositionsSquare, ColorsSquare, StartPositionSquare, device, context, layout));

	//triangleComponent->InitializeShape(triangle.VertexPositions, triangle.Colors, std::size(triangle.VertexPositions), triangle.StartPosition);
	//square->InitializeShape(std::size(square->VertexPositions));
	for (auto& currentSquare : squares)
	{
		currentSquare.InitializeShape(std::size(currentSquare.VertexPositions));
	}
	//for (auto& currentTriangle : triangles)
	//{
	//	currentTriangle.InitializeShape(std::size(currentTriangle.VertexPositions));
	//}

	//triangleComponent->InitializeShape(square.VertexPositions, square.Colors, std::size(square.VertexPositions), square.StartPosition);

	SetupRasterizerStage();
	std::chrono::time_point<std::chrono::steady_clock> PrevTime = std::chrono::steady_clock::now();
	float totalTime = 0;
	unsigned int frameCount = 0;
	WindowLoop(PrevTime, totalTime, frameCount);
	return 0;
}
void Game::SetupRasterizerStage()
{
	rastDesc.CullMode = D3D11_CULL_NONE; //triangles in the specified direction do not need to be drawn
	rastDesc.FillMode = D3D11_FILL_SOLID; //fill mode (solid or wireframe)

	res = device->CreateRasterizerState(&rastDesc, &rastState);

	context->RSSetState(rastState);
}

void Game::WindowLoop(std::chrono::steady_clock::time_point& PrevTime, float& totalTime, unsigned int& frameCount)
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
			SetWindowText(display.hWnd, text);

			frameCount = 0;
		}


		context->ClearState();



		UINT strides[] = { 32 };
		UINT offsets[] = { 0 };

		float color[] = { totalTime, 0.1f, 0.1f, 1.0f };
		context->ClearRenderTargetView(rtv, color);
		for (auto& currentSquare : squares)
		{
			currentSquare.SetupIAStage(strides, offsets);
			context->RSSetState(rastState);

			SetupViewport();

			//square->SetupIAStage(strides, offsets);
			SetVertexAndPixelShaders();

			SetBackBufferOutput(1, &rtv, nullptr);


			currentSquare.MoveShape(currentSquare.MoveSpeed
				* deltaTime * currentSquare.DirectionX,
				currentSquare.MoveSpeed
				* deltaTime * currentSquare.DirectionY, 0);

			//context->DrawIndexed(6, 0, 0);
		}
		for (auto& currentTriangle : triangles)
		{
			currentTriangle.SetupIAStage(strides, offsets);
			context->RSSetState(rastState);

			SetupViewport();

			//square->SetupIAStage(strides, offsets);
			SetVertexAndPixelShaders();

			SetBackBufferOutput(1, &rtv, nullptr);


			currentTriangle.MoveShape(currentTriangle.MoveSpeed
				* deltaTime * currentTriangle.DirectionX,
				currentTriangle.MoveSpeed
				* deltaTime * currentTriangle.DirectionY, 0);

			context->DrawIndexed(6, 0, 0);
		}
		npcPaddle->Update(deltaTime);


		SetBackBufferOutput(0, nullptr, nullptr);

		swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
	}
}

void Game::SetupViewport()
{
	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(display.ScreenWidth);
	viewport.Height = static_cast<float>(display.ScreenHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	context->RSSetViewports(1, &viewport);
}

/*void Game::SetupIAStage(UINT strides[1], UINT offsets[1])
{
	context->IASetInputLayout(layout);
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetIndexBuffer(square->ib, DXGI_FORMAT_R32_UINT, 0);
	context->IASetVertexBuffers(0, 1, &square->vb, strides, offsets);
}*/

void Game::SetVertexAndPixelShaders()
{
	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);
}

void Game::SetBackBufferOutput(UINT NumViews, ID3D11RenderTargetView* const* ppRenderTargetViews, ID3D11DepthStencilView* pDepthStencilView)
{
	context->OMSetRenderTargets(NumViews, ppRenderTargetViews, pDepthStencilView);
}

HRESULT Game::InitSwapChain() //swap between back and front buffers
{
	D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_1 };

	swapDesc = {}; //description for creating swap chain
	swapDesc.BufferCount = 2; //windows count (back and front window)
	swapDesc.BufferDesc.Width = display.ScreenWidth;
	swapDesc.BufferDesc.Height = display.ScreenHeight;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60; //update rate 
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; //sweep drawing mode
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; //scaling mode
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //parameter buffer usage (here create back and front buffer)
	swapDesc.OutputWindow = display.hWnd; //show window
	swapDesc.Windowed = true; //show mode (window or full screen)
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; //options for working with the buffer after its output
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; //switch between window and full screen mode
	swapDesc.SampleDesc.Count = 1; //multi samples count on per pixel
	swapDesc.SampleDesc.Quality = 0; //image quality (between 0-1)



	HRESULT res = D3D11CreateDeviceAndSwapChain(
		nullptr, //use default adapter
		D3D_DRIVER_TYPE_HARDWARE, //use DirectX3D functions
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG, //device with debug mode
		featureLevel,
		1, //feature level order
		D3D11_SDK_VERSION,
		&swapDesc, //parameters for init buffers chain
		&swapChain,
		&device,
		nullptr,
		&context);
	if (FAILED(res))
	{
		std::cout << "Init error SwapChain!" << std::endl;
	}
	return res;
}

void Game::GetBackBufferAndCreateRTV() //with help back buffer create render target view (RTV) (usually texture) for show anything
{
	res = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backTex);	// __uuidof(ID3D11Texture2D)
	res = device->CreateRenderTargetView(backTex, nullptr, &rtv);
}
int Game::CompileShaders()
{
	retFlag = true;
	res = D3DCompileFromFile(L"./Shaders/MyVeryFirstShader.hlsl", //create vertex shader from  hlsl file
		nullptr /*macros*/,
		nullptr /*include*/,
		"VSMain", //function name from hlsl file for run it
		"vs_5_0", //shader target - vertex shader (vs)
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, //options for compile shader
		0, //parameters for compile effects (if 0, then don't compile them)
		&vertexBC,
		&errorVertexCode);

	if (FAILED(res)) {
		// If the shader failed to compile it should have written something to the error message.
		if (errorVertexCode) {
			char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());

			std::cout << compileErrors << std::endl;
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(display.hWnd, L"MyVeryFirstShader.hlsl", L"Missing Shader File", MB_OK);
		}

		return 0;
	}

	D3D_SHADER_MACRO Shader_Macros[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr };
	//"TEST" - use TEST define from hlsl file
	//float4(0.0f, 1.0f, 0.0f, 1.0f) - color for square right

	res = D3DCompileFromFile(L"./Shaders/MyVeryFirstShader.hlsl", //create pixel shaders from hlsl file
		Shader_Macros /*macros*/, //macros shaders
		nullptr /*include*/,
		"PSMain", //function name from hlsl file for run it
		"ps_5_0", //shader target - pixel shader (ps)
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, //options for compile shader
		0, //parameters for compile effects (if 0, then don't compile them)
		&pixelBC,
		&errorPixelCode);

	device->CreateVertexShader(
		vertexBC->GetBufferPointer(),
		vertexBC->GetBufferSize(),
		nullptr, &vertexShader);

	device->CreatePixelShader(
		pixelBC->GetBufferPointer(),
		pixelBC->GetBufferSize(),
		nullptr, &pixelShader);
	retFlag = false;
	return {};
}
//void Game::CreateInputLayout()
//{
//	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
//	D3D11_INPUT_ELEMENT_DESC{
//		"POSITION", //parameter name from hlsl file
//		0, //need if we have more one element with same semantic
//		DXGI_FORMAT_R32G32B32A32_FLOAT,
//		0, //vertex index (between 0 and 15)
//		0, //offset from beginning vertex
//		D3D11_INPUT_PER_VERTEX_DATA, //class input data for input slot (for each vertex or instance)
//		0 },
//		D3D11_INPUT_ELEMENT_DESC{
//		"COLOR",
//		0,
//		DXGI_FORMAT_R32G32B32A32_FLOAT,
//		0,
//		D3D11_APPEND_ALIGNED_ELEMENT,
//		D3D11_INPUT_PER_VERTEX_DATA,
//		0 }
//	};
//
//	device->CreateInputLayout(
//		inputElements,
//		2,
//		vertexBC->GetBufferPointer(),
//		vertexBC->GetBufferSize(),
//		&layout);
//}
