// MySuper3DApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>
#include <WinUser.h>
#include <wrl.h>
#include <iostream>
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <chrono>
#include "MySuper3DApp.h"


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

int main()
{
	LPCWSTR applicationName = L"My3DApp";
	HINSTANCE hInstance = GetModuleHandle(nullptr);

#pragma region Window init
	InitializeWindow(hInstance, applicationName);

#pragma endregion Window init


	HRESULT res = InitSwapChain();


	GetBackBufferAndCreateRTV(res);

	bool retFlag;
	int retVal = CompileShaders(res, retFlag);
	if (retFlag) return retVal;

	CreateInputLayout();

	DirectX::XMFLOAT4 points[8] = { //set points for show it (right = color, left = vertex position) (four line because need square)
		DirectX::XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(0.7f, 0.08f, 0.9f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(0.3f, 0.06f, 0.9f, 0.5f),
		DirectX::XMFLOAT4(0.5f, -0.5f, 0.5f, 1.0f),  DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f),
		//DirectX::XMFLOAT4(-0.5f, 0.5f, 0.5f, 1.0f),  DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
	};


	CreateVertexAndIndexBuffers(points);		
	
	SetupRasterizerStage(res);

	std::chrono::time_point<std::chrono::steady_clock> PrevTime = std::chrono::steady_clock::now();
	float totalTime = 0;
	unsigned int frameCount = 0;


	WindowLoop(PrevTime, totalTime, frameCount);

	std::cout << "Hello World!\n";
}
void WindowLoop(std::chrono::steady_clock::time_point& PrevTime, float& totalTime, unsigned int& frameCount)
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

		context->ClearState();

		context->RSSetState(rastState);

		SetupViewport();

		SetupIAStage(strides, offsets);
		SetVertexAndPixelShaders();


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
			SetWindowText(hWnd, text);

			frameCount = 0;
		}

		SetBackBufferOutput(1, &rtv, nullptr);

		float color[] = { totalTime, 0.1f, 0.1f, 1.0f };
		context->ClearRenderTargetView(rtv, color);

		context->DrawIndexed(6, 0, 0);

		SetBackBufferOutput(0, nullptr, nullptr);

		swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
	}
}
void SetBackBufferOutput(UINT NumViews, ID3D11RenderTargetView* const* ppRenderTargetViews, ID3D11DepthStencilView* pDepthStencilView)
{
	context->OMSetRenderTargets(NumViews, ppRenderTargetViews, pDepthStencilView);
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	case WM_KEYDOWN:
	{
		// If a key is pressed send it to the input object so it can record that state.
		std::cout << "Key: " << static_cast<unsigned int>(wparam) << std::endl;

		if (static_cast<unsigned int>(wparam) == 27) PostQuitMessage(0);
		return 0;
	}
	default:
	{
		return DefWindowProc(hwnd, umessage, wparam, lparam);
	}
	}
}
void InitializeWindow(HINSTANCE hInstance, LPCWSTR applicationName)
{
	WNDCLASSEX wc;

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	RECT windowRect = { 0, 0, static_cast<LONG>(SCREEN_WIDTH), static_cast<LONG>(SCREEN_HEIGHT) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	auto dwStyle = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME;

	auto posX = (GetSystemMetrics(SM_CXSCREEN) - SCREEN_WIDTH) / 2;
	auto posY = (GetSystemMetrics(SM_CYSCREEN) - SCREEN_HEIGHT) / 2;

	hWnd = CreateWindowEx(WS_EX_APPWINDOW, applicationName, applicationName,
		dwStyle,
		posX, posY,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr, nullptr, hInstance, nullptr);

	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);

	ShowCursor(true);
}

void SetupViewport()
{
	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(SCREEN_WIDTH);
	viewport.Height = static_cast<float>(SCREEN_HEIGHT);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	context->RSSetViewports(1, &viewport);
}

void SetupRasterizerStage(HRESULT& res)
{
	rastDesc.CullMode = D3D11_CULL_NONE; //triangles in the specified direction do not need to be drawn
	rastDesc.FillMode = D3D11_FILL_SOLID; //fill mode (solid or wireframe)

	res = device->CreateRasterizerState(&rastDesc, &rastState);

	context->RSSetState(rastState);
}

void SetVertexAndPixelShaders()
{
	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);
}

void SetupIAStage(UINT  strides[1], UINT  offsets[1])
{
	context->IASetInputLayout(layout);
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);
	context->IASetVertexBuffers(0, 1, &vb, strides, offsets);
}

HRESULT InitSwapChain() //swap between back and front buffers
{
	D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_1 };

	swapDesc = {}; //description for creating swap chain
	swapDesc.BufferCount = 2; //windows count (back and front window)
	swapDesc.BufferDesc.Width = SCREEN_WIDTH;
	swapDesc.BufferDesc.Height = SCREEN_HEIGHT;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60; //update rate 
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; //sweep drawing mode
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; //scaling mode
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //parameter buffer usage (here create back and front buffer)
	swapDesc.OutputWindow = hWnd; //show window
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
void GetBackBufferAndCreateRTV(HRESULT& res) //with help back buffer create render target view (RTV) (usually texture) for show anything
{
	res = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backTex);	// __uuidof(ID3D11Texture2D)
	res = device->CreateRenderTargetView(backTex, nullptr, &rtv);
}
int CompileShaders(HRESULT& res, bool& retFlag)
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
			MessageBox(hWnd, L"MyVeryFirstShader.hlsl", L"Missing Shader File", MB_OK);
		}

		return 0;
	}

	D3D_SHADER_MACRO Shader_Macros[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr };

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
void CreateInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		D3D11_INPUT_ELEMENT_DESC{
			"POSITION", //parameter name from hlsl file
			0, //need if we have more one element with same semantic
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0, //vertex index (between 0 and 15)
			0, //offset from beginning vertex
			D3D11_INPUT_PER_VERTEX_DATA, //class input data for input slot (for each vertex or instance)
			0 },
			D3D11_INPUT_ELEMENT_DESC{
			"COLOR",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0 }
	};

	device->CreateInputLayout(
		inputElements,
		2,
		vertexBC->GetBufferPointer(),
		vertexBC->GetBufferSize(),
		&layout);
}
template <size_t N>
void CreateVertexAndIndexBuffers(DirectX::XMFLOAT4(&points)[N])
{
	D3D11_BUFFER_DESC vertexBufDesc = {}; 
	vertexBufDesc.Usage = D3D11_USAGE_DEFAULT; //how often does writing and reading to the buffer occur (default = read/write from GPU)
	vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //way to bind a buffer to a pipeline
	vertexBufDesc.CPUAccessFlags = 0; //0 = CPU don't need, 1 = CPU need
	vertexBufDesc.MiscFlags = 0; //optional parameters
	vertexBufDesc.StructureByteStride = 0; //size per element in buffer structure
	vertexBufDesc.ByteWidth = sizeof(DirectX::XMFLOAT4) * std::size(points);

	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = points;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	device->CreateBuffer(&vertexBufDesc, &vertexData, &vb);

	int indeces[] = { 0,1,2, 1,0,3 };
	D3D11_BUFFER_DESC indexBufDesc = {};
	indexBufDesc.Usage = D3D11_USAGE_DEFAULT; //how often does writing and reading to the buffer occur (default = read/write from GPU)
	indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER; //way to bind a buffer to a pipeline
	indexBufDesc.CPUAccessFlags = 0; //0 = CPU don't need, 1 = CPU need
	indexBufDesc.MiscFlags = 0; //optional parameters
	indexBufDesc.StructureByteStride = 0; //size per element in buffer structure
	indexBufDesc.ByteWidth = sizeof(int) * std::size(indeces);

	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = indeces;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	device->CreateBuffer(&indexBufDesc, &indexData, &ib);
}
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file


