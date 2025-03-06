#include "Game.h"
// Инициализация статического члена
Game* Game::instance = nullptr;
int Game::Initialize()
{
	display->InitWindow();
	res = InitSwapChain();
	GetBackBufferAndCreateRTV();
	retVal = CompileShaders();
	if (retFlag) return retVal;
	SetupRasterizerStage();
	//std::chrono::time_point<std::chrono::steady_clock> PrevTime = std::chrono::steady_clock::now();
	//float totalTime = 0;
	//unsigned int frameCount = 0;
	//WindowLoop(PrevTime, totalTime, frameCount);
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

		//cube.MoveShape(0.0f, 0.0f, 0.0f);


		//SetBackBufferOutput(0, nullptr, nullptr);

		swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
	}
}
Game& Game::getInstance()
{
    if (!instance) {
        instance = new Game();
    }
    return *instance;
}

void Game::destroyInstance()
{
    if (instance) {
        delete instance;
        instance = nullptr;
    }
}

HRESULT Game::InitSwapChain()
{
	D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_1 };

	swapDesc = {}; //description for creating swap chain
	swapDesc.BufferCount = 2; //windows count (back and front window)
	swapDesc.BufferDesc.Width = display->ScreenWidth;
	swapDesc.BufferDesc.Height = display->ScreenHeight;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60; //update rate 
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; //sweep drawing mode
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; //scaling mode
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //parameter buffer usage (here create back and front buffer)
	swapDesc.OutputWindow = display->hWnd; //show window
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

void Game::GetBackBufferAndCreateRTV()
{
	res = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backTex);	// __uuidof(ID3D11Texture2D)
	res = device->CreateRenderTargetView(backTex, nullptr, &rtv);

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	ID3D11DepthStencilState* depthStencilState;
	device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
	context->OMSetDepthStencilState(depthStencilState, 1u);
	ID3D11Texture2D* pDepthStencil;
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = 800;
	descDepth.Height = 800;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1u;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	device->CreateTexture2D(&descDepth, nullptr, &pDepthStencil);
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;
	device->CreateDepthStencilView(pDepthStencil, &descDSV, &depthStencilView);
	SetBackBufferOutput(1u, &rtv, depthStencilView);
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
			MessageBox(display->hWnd, L"MyVeryFirstShader.hlsl", L"Missing Shader File", MB_OK);
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

void Game::SetupRasterizerStage()
{
	rastDesc.CullMode = D3D11_CULL_NONE; //triangles in the specified direction do not need to be drawn
	rastDesc.FillMode = D3D11_FILL_SOLID; //fill mode (solid or wireframe)

	res = device->CreateRasterizerState(&rastDesc, &rastState);

	context->RSSetState(rastState);
}

void Game::SetBackBufferOutput(UINT NumViews, ID3D11RenderTargetView* const* ppRenderTargetViews, ID3D11DepthStencilView* pDepthStencilView)
{
	context->OMSetRenderTargets(NumViews, ppRenderTargetViews, pDepthStencilView);
}
