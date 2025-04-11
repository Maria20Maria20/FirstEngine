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

	gameObjects.push_back(plane);
	gameObjects.push_back(player);

	for (int i = 0; i < 20; ++i)
	{
		float rad = 1.0f;
		int spawnDist = 100;
		float z = (rand() % spawnDist) - spawnDist / 2;
		float x = (rand() % spawnDist) - spawnDist / 2;
		items.push_back(new Item(device, vertexBC, vertexShader,
			pixelShader, rtv, depthStencilView, context, 
			&camera, GameObject::ObjectType::CUBE, XMFLOAT3(x, 0.0f, z), rad, 1.0f)
		);
		gameObjects.push_back(items.back());
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

	Vector3 lightDir = { 0, -0.3, 1 };
	lightDir.Normalize();
	// Directional light
	lightData.dLight = {
		Vector4(0.1f, 0.1f, 0.1f, 0.0f),
		Vector4(0.4f, 0.4f, 0.4f, 0.0f),
		Vector4(0.6f, 0.6f, 0.6f, 0.0f),
		lightDir,
		0
	};

	//light_pcb = new Bind::PixelConstantBuffer<LightData>(renderer.GetDevice(), lightData, 0u);
	//renderer.AddPerFrameBind(light_pcb);

	//cam_pcb = new Bind::PixelConstantBuffer<Camera_PCB>(renderer.GetDevice(), { renderer.camera.GetPosition() }, 1u);
	//renderer.AddPerFrameBind(cam_pcb);


	// --- Shadow Map stuff ---

	// Camera from light
	lightPos = { 0, 30, -60 };
	lightViewCamera = new Camera(smSizeX / smSizeY);
	lightViewCamera->SetPosition(lightPos);
	lightViewCamera->SetTarget(lightPos + lightDir);
	Vector3 cameraUp = { 0, 1, 0 };
	cameraUp.Normalize();
	lightViewCamera->SetUp(cameraUp);
	lightViewCamera->SwitchProjection();
	lightViewCamera->SetViewWidth(200.0f);
	lightViewCamera->SetViewHeight(100.0f);
	lightViewCamera->SetNearZ(cascadeBounds[2]);
	lightViewCamera->SetFarZ(cascadeBounds[3]);

	// Viewport for rendering z-buffer from light
	smViewport.TopLeftX = 0.0f;
	smViewport.TopLeftY = 0.0f;
	smViewport.Width = static_cast<float>(512);
	smViewport.Height = static_cast<float>(512);
	smViewport.MinDepth = 0.0f;
	smViewport.MaxDepth = 1.0f;

	// Texture for depth values (for shadowing)
	D3D11_TEXTURE2D_DESC depthDesc = {};
	depthDesc.Width = smSizeX;
	depthDesc.Height = smSizeY;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 4;
	depthDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	depthDesc.MiscFlags = 0;

	device->CreateTexture2D(&depthDesc, nullptr, &shadowTexture);

	// View texture as Depth buffer while rendering from light camera
	D3D11_DEPTH_STENCIL_VIEW_DESC dViewDesc = { };
	dViewDesc.Flags = 0;
	dViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	dViewDesc.Texture2DArray.MipSlice = 0;
	dViewDesc.Texture2DArray.ArraySize = 1;

	for (size_t i = 0; i < 4; i++)
	{
		dViewDesc.Texture2DArray.FirstArraySlice = D3D11CalcSubresource(0, i, 1);
		device->CreateDepthStencilView(shadowTexture, &dViewDesc, &(depthDSV[i]));
	}


	// View texture as Shader resource while using it for shadowing in pixel shader
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	srvDesc.Texture2DArray.ArraySize = 4;

	device->CreateShaderResourceView(shadowTexture, &srvDesc, &depthSRV);

	// Rasterizer for depth bias to get rid of self-shadowing
	D3D11_RASTERIZER_DESC rastDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.DepthBias = 100000;
	rastDesc.DepthBiasClamp = 0.0f;
	rastDesc.SlopeScaledDepthBias = 2.0f;

	device->CreateRasterizerState(&rastDesc, &pRasterizer);

	// Sampler of texture. It samples values from texture
	D3D11_SAMPLER_DESC shadowSamplerDesc;
	shadowSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	shadowSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.MipLODBias = 0.0f;
	shadowSamplerDesc.MaxAnisotropy = 1;
	shadowSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	shadowSamplerDesc.BorderColor[0] = 1.0;
	shadowSamplerDesc.BorderColor[1] = 1.0;
	shadowSamplerDesc.BorderColor[2] = 1.0;
	shadowSamplerDesc.BorderColor[3] = 1.0;
	shadowSamplerDesc.MinLOD = 0;
	shadowSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&shadowSamplerDesc, &pSampler);

	shadowSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	shadowSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.MipLODBias = 0.0f;
	shadowSamplerDesc.MaxAnisotropy = 1;
	shadowSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	shadowSamplerDesc.BorderColor[0] = 1.0;
	shadowSamplerDesc.BorderColor[1] = 1.0;
	shadowSamplerDesc.BorderColor[2] = 1.0;
	shadowSamplerDesc.BorderColor[3] = 1.0;
	shadowSamplerDesc.MinLOD = 0;
	shadowSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&shadowSamplerDesc, &pSampler_PCF);;

	// Vertex shader for generating Z-buffer from light
	{
		LPCWSTR filePath = L"./Shaders/ShadowMapVShader.hlsl";

		ID3DBlob* errorVertexCode = nullptr;
		HRESULT hr = D3DCompileFromFile(
			filePath,
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"VSMain",
			"vs_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			&pShadowShaderBytecodeBlob,
			&errorVertexCode);

		if (FAILED(hr)) {
			if (errorVertexCode) {
				char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());
				std::cout << compileErrors << std::endl;
			}
			else {
				std::cout << filePath << L" - Missing Shader File\n";
			}

		}

		device->CreateVertexShader(
			pShadowShaderBytecodeBlob->GetBufferPointer(),
			pShadowShaderBytecodeBlob->GetBufferSize(),
			nullptr,
			&pShadowVertexShader
		);
	}


	{
		D3D11_INPUT_ELEMENT_DESC shadowIALayoutInputElements[1] = {
		D3D11_INPUT_ELEMENT_DESC{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA,
			0 },
		};
		device->CreateInputLayout(
			shadowIALayoutInputElements,
			1,
			pShadowShaderBytecodeBlob->GetBufferPointer(),
			pShadowShaderBytecodeBlob->GetBufferSize(),
			&pShadowInputLayout);
	}

	// Buffer for transformation respect to light camera
	{
		// Transform NDC space [-1,+1]^2 to texture space [0,1]^2 (for XY; Z stays the same)
		DirectX::XMMATRIX T = {
			0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, -0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.0f, 1.0f
		};

		for (size_t i = 0; i < 4; i++)
		{
			lightViewCamera->SetNearZ(cascadeBounds[i] - (i > 0 ? frustrumBias : 0));
			lightViewCamera->SetFarZ(cascadeBounds[i + 1]);

			cascadesData.cascades[i] = {
				lightViewCamera->GetViewMatrix(),
				lightViewCamera->GetProjectionMatrix(),
				DirectX::XMMatrixIdentity()
			};

			cascadesData.cascades[i].shadowTransformFull =
				cascadesData.cascades[i].lightView
				* cascadesData.cascades[i].lightProjection
				* T;

		}
		cascadesData.distances->x = cascadeBounds[1];
		cascadesData.distances->y = cascadeBounds[2];
		cascadesData.distances->z = cascadeBounds[3];
		cascadesData.distances->w = cascadeBounds[4];

		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(ShadowTransformData) + (16 - (sizeof(ShadowTransformData) % 16));  // aligned size
		cbd.StructureByteStride = 0u;

		D3D11_SUBRESOURCE_DATA InitData = {};
		InitData.pSysMem = &(cascadesData.cascades[0]);
		device->CreateBuffer(&cbd, &InitData, &shadowTransformsConstantBuffer);

		// All Cascades (for pixel shader while drawing final scene)

		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(CascadesData) + (16 - (sizeof(CascadesData) % 16));  // aligned size
		cbd.StructureByteStride = 0u;

		InitData.pSysMem = &(cascadesData);
		device->CreateBuffer(&cbd, &InitData, &cascadesConstantBuffer);

	}

	// Point lights
	for (int i = 0; i < pointLightCount; i++)
	{
		lightData.pointLights[i].Ambient = { 0, 0, 0, 1 };
		lightData.pointLights[i].Diffuse = { (i + 1) % 2 * 1.0f, i / 3 * 1.0f, (i + 2) % 6 / 3 * 1.0f, 1 };
		lightData.pointLights[i].Specular = { (i + 1) % 2 * 1.0f, i / 3 * 1.0f, (i + 2) % 6 / 3 * 1.0f, 1 };
		
		float _pLightDist = 5.0f + (rand() % 30) / 10.0f; // 2.0 + rand(0, 3)
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
		for (size_t i = 0; i < pointLightCount; i++)
		{
			lightData.pointLights[i].Position =
				Vector3::Transform(
					pointLightInitPositions[i],
					//Matrix::CreateRotationY(deltaTime * 1.0f)
					player->GetRelativeTransform()
				);
		}

		// --- Shadow Map ---
		
		for (currCascade = 0; currCascade < 4; currCascade++)
		{
			BindDsvAndSetNullRenderTarget();
			DrawSceneToShadowMap();
		}
		context->OMSetRenderTargets(0, NULL, NULL);
		
		// --- Draw ---

		// camera.SetFarZ(1000.0f);


		SetBackBufferOutput(1u, &rtv, depthStencilView);
		float color[] = { 0.0f, 0.1f, 0.5f, 1.0f };
		context->ClearRenderTargetView(rtv, color);
		context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1, 0);
		
		context->RSSetState(defaultRastState);
		
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

		// --- Resources for shadows ---

		context->PSSetSamplers(0u, 1u, pSampler.GetAddressOf());
		context->PSSetSamplers(1u, 1u, pSampler_PCF.GetAddressOf());

		context->PSSetShaderResources(0u, 1u, &depthSRV);

		context->PSSetConstantBuffers(2u, 1u, &cascadesConstantBuffer);

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

void StartGame::BindDsvAndSetNullRenderTarget()
{

	context->OMSetRenderTargets(0, NULL, NULL);
	context->PSSetShaderResources(0u, 0u, nullptr);
	context->RSSetViewports(1, &smViewport);
	// Set null render target because we are only going to draw
	// to depth buffer. Setting a null render target will disable
	// color writes.

	ID3D11RenderTargetView* rtvDepth[1] = { 0 };
	//device->CreateRenderTargetView(shadowTexture, &depthRtvDesc, &rtvDepth[i]);

	context->OMSetRenderTargets(1, rtvDepth, depthDSV[currCascade]);
	context->ClearDepthStencilView(depthDSV[currCascade], D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void StartGame::DrawSceneToShadowMap() {
	
	// --- Per Frame ---

	context->RSSetState(pRasterizer.Get());

	// --- Per object ---

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	context->Map(shadowTransformsConstantBuffer, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource);
	memcpy(mappedResource.pData, &(cascadesData.cascades[currCascade]),
		sizeof(ShadowTransformData) + (16 - (sizeof(ShadowTransformData) % 16))); // aligned size
	context->Unmap(shadowTransformsConstantBuffer, 0);

	for (GameObject* obj : gameObjects) {		
		// Input Layout (wrong!) + Vertex Buffer + Index Buffer + Topology
		obj->SetupIAStage(); // Net, nado drugoi nastroit

		// Constant (Transform) buffer
		obj->UpdateConstantBuffer();
		context->VSSetConstantBuffers(0u, 1u, &(obj->mConstantBuffer));
		context->VSSetConstantBuffers(1u, 1u, &shadowTransformsConstantBuffer);
	
		context->IASetInputLayout(pShadowInputLayout.Get());
		context->VSSetShader(pShadowVertexShader.Get(), nullptr, 0u);
		context->PSSetShader(nullptr, nullptr, 0u);
		context->DrawIndexed(obj->indicesNum, 0, 0);

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
