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

	// --- Setup default BlendState ---

	context->OMGetBlendState(&defaultBlendState, defaultBlendFactor, &defaultSampleMask);

	// --- Setup default RastState ---

	CD3D11_RASTERIZER_DESC defaultRastDesc = {};
	defaultRastDesc.CullMode = D3D11_CULL_NONE; //triangles in the specified direction do not need to be drawn
	defaultRastDesc.FillMode = D3D11_FILL_SOLID; //fill mode (solid or wireframe)
	device->CreateRasterizerState(&defaultRastDesc, &defaultRastState);
	context->RSSetState(defaultRastState);

	// --- Setup flowers BlendState ---

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

	device->CreateBlendState(&blendDesc, &flowersBlendState);

	// --- Setup Kevin BlendState ---

	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));

	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	device->CreateBlendState(&blendDesc, &KevinBlendState);

	// --- Setup Kevin RastState ---

	CD3D11_RASTERIZER_DESC newRastDesc = {};
	newRastDesc.CullMode = D3D11_CULL_BACK; //triangles in the specified direction do not need to be drawn
	newRastDesc.FillMode = D3D11_FILL_SOLID; //fill mode (solid or wireframe)
	device->CreateRasterizerState(&newRastDesc, &KevinRastState);

	// --- Buffer for lighting (SLOT 1) ---

	// Directional light
	lightData.dLight = {
		Vector4(0.1f, 0.1f, 0.1f, 0.0f),
		Vector4(0.4f, 0.4f, 0.4f, 0.0f),
		Vector4(0.6f, 0.6f, 0.6f, 0.0f),
		Vector3(-2.0f, -1.0f, -1.0f) / Vector3(-2.0f, -1.0f, -1.0f).Length(),
		0
	};


	// Point lights
	for (int i = 0; i < 6; i++)
	{
		lightData.pointLights[i].Ambient = { 0, 0, 0, 1 };
		lightData.pointLights[i].Diffuse = { (i + 1) % 2 * 1.0f, i / 3 * 1.0f, (i + 2) % 6 / 3 * 1.0f, 1 };
		lightData.pointLights[i].Specular = { (i + 1) % 2 * 1.0f, i / 3 * 1.0f, (i + 2) % 6 / 3 * 1.0f, 1 };
		
		float _pLightDist = 2.0f + (rand() % 30) / 10.0f; // 2.0 + rand(0, 3)
		pointLightInitPositions[i] = { _pLightDist, 0.0f, 0.0f };
		pointLightInitPositions[i] = 
			Vector3::Transform(
				pointLightInitPositions[i],
				GetRandomRotateTransform()
			);

		lightData.pointLights[i].Range = 15.0f; // max distance for light from point light
		lightData.pointLights[i].Att = { 0.01f, 1.0f, 0.0f }; // attenuation coefficients
	}



	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(lightData) + (16 - (sizeof(lightData) % 16));  // aligned size
	cbd.StructureByteStride = 0u;

	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = &lightData;
	device->CreateBuffer(&cbd, &InitData, &lightConstPixelBuffer);

	// ------------------

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

		// --- Physics update ---

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

		// --- Update lighting ---
		for (size_t i = 0; i < 6; i++)
		{
			lightData.pointLights[i].Position =
				Vector3::Transform(
					pointLightInitPositions[i],
					//Matrix::CreateRotationY(deltaTime * 1.0f)
					player->GetRelativeTransform()
				);
		}

		// --- Draw ---

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

		// --- Setup lighting ---

		// Transfer light data to GPU
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		context->Map(lightConstPixelBuffer, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource);
		memcpy(mappedResource.pData, &lightData, sizeof(lightData) + (16 - (sizeof(lightData) % 16))); // aligned size
		context->Unmap(lightConstPixelBuffer, 0);

		// (SLOT, num_of_buffers, data_for_constant buffer)
		context->PSSetConstantBuffers(1u, 1u, &lightConstPixelBuffer);

		// --- Draw plane ---

		plane->Draw(context, projection);

		// --- Draw flowers (items) ---
		// --- Set BlendState for transparent flowers ---

		float blend_factor[4] = { 1.f, 1.f, 1.f, 1.f };
		context->OMSetBlendState(flowersBlendState, blend_factor, 0xffffffff);

		for (Item* item : items)
		{
			//std::cout << item.initialPosition.x << "\n";
			item->Draw(context, projection);
		}

		// --- Draw Kevin ---
		// --- BlendState for transparent Kevin ---

		context->OMSetBlendState(KevinBlendState, blend_factor, 0xffffffff);

		// --- RastState for Kevin ---

		context->RSSetState(KevinRastState);
		player->Draw(context, projection);


		// --- Return to default settings (RastState, BlendState) ---

		context->RSSetState(defaultRastState);
		context->OMSetBlendState(defaultBlendState, defaultBlendFactor, defaultSampleMask);

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
