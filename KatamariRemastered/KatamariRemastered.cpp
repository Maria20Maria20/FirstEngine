#include "KatamariRemastered.h"


KatamariRemastered::KatamariRemastered() :
	DeferredGame(L"KATAMARI II")
{
	InputDevice::getInstance().OnKeyPressed.AddRaw(this, &KatamariRemastered::HandleMoveDown);
	InputDevice::getInstance().MouseMove.AddRaw(this, &KatamariRemastered::HandleMouseMove);

	GBufferPass* gBufferPass = new GBufferPass(renderingSystem->GetDevice(),
		renderingSystem->GetDeviceContext(), renderingSystem->GetBackBuffer(), winWidth, winHeight);
	renderingSystem->SetMainCamera(gBufferPass->GetCamera());
	renderingSystem->AddPass(gBufferPass);

	lightPass = new LightPass(renderingSystem->GetDevice(),
		renderingSystem->GetDeviceContext(), renderingSystem->GetBackBuffer(),
		winWidth, winHeight, gBufferPass->pGBuffer, gBufferPass->GetCamera());
	renderingSystem->AddPass(lightPass);

	ground = new Ground(renderingSystem->GetDevice(), { 0, 0, 0 }, 10);
	scene.AddGameObject(ground);
	player = new Player(renderingSystem->GetDevice(), { 0, 0, 0 }, 1);
	scene.AddGameObject(player);

	for (int i = 0; i < 20; ++i)
	{
		float rad = 1.0f;
		items.push_back(new Item(renderingSystem->GetDevice(), rad, 1.0f));
		items.back()->player = player;
		scene.AddGameObject(items.back());
	}


	scene.AddGameObject(new DirectionalLight(renderingSystem->GetDevice(),
		{ 5.0f, 5.5f, -5.0f }, { 0.0f, -1.0f, 1.0f },
		{ 0.1f, 0.1f, 0.1f, 0 }, { 0.3f, 0.3f, 0.3f, 0 }, { 0.5f, 0.5f, 0.5f, 0 }));
	
	

	// Point lights
	for (int i = 0; i < pointLightCount; i++)
	{
		Vector4 Ambient = { 0, 0, 0, 1 };
		Vector4 Diffuse = { (i + 1) % 2 * 1.0f, i / 3 * 1.0f, (i + 2) % 6 / 3 * 1.0f, 1 };
		Vector4 Specular = { (i + 1) % 2 * 1.0f, i / 3 * 1.0f, (i + 2) % 6 / 3 * 1.0f, 1 };

		float _pLightDist = 5.0f + (rand() % 30) / 10.0f; // 2.0 + rand(0, 3)
		pointLightInitPositions[i] = {_pLightDist, 0.0f, 0.0f};
		pointLightInitPositions[i] =
			Vector3::Transform(
				pointLightInitPositions[i],
				GetRandomRotateTransform()
			);
		//pointLightInitPositions[i].y = 1;

		float Range = 32.0f; // max distance for light from point light
		Vector3 Att = { 0.01f, 1.0f, 0.0f }; // attenuation coefficients

		pointLights.push_back(new PointLight(
			renderingSystem->GetDevice(),
			pointLightInitPositions[i],
			Range,
			Att,
			Ambient,
			Diffuse,
			Specular
		));
		scene.AddGameObject(pointLights.back());
	}
	

	spotLight = new SpotLight(renderingSystem->GetDevice(),
		{ 0.0f, 4.0f, 0.0f }, 5.0f, { 0.0f, -1.0f, 0.0f },
		10, { 0.01f, 0.05f, 0.0f },
		{ 0, 0.1, 0, 1 }, { 1, 1, 0, 1 }, { 1, 1, 0, 1 });
	scene.AddGameObject(spotLight);
	scene.AddGameObject(new AmbientLight(renderingSystem->GetDevice(), { 0,0.5,0.5,0 }));

	gbm = new GBufferMaps(renderingSystem->GetDevice());
	scene.AddGameObject(gbm);
	


	for (auto gameObject : scene.gameObjects) {
		gameObject->camera = renderingSystem->GetMainCamera();
	}


	renderingSystem->GetMainCamera()->SwitchToFollowMode(player->position, player->GetMoveDir(), player->radius);


	// Leafs Particles
	ParticleSystem::EmitterPointConstantBuffer emitterDesc =
	{
		Matrix::Identity,
		{ 0, 0, 0, 1 },
		{ 1, 1, 1, 1 },
		{ 1, 1, 1, 1 },
		100, 1, 10, 1,
		0.5, 0.1,
		0, 3.1415 * 2,
		3.1415, 0, { 0, 0 }
	};
	ParticleSystem::SimulateParticlesConstantBuffer simulatorDesc = {
		{ 0, 0, 0, 0 }
	};
	leafParticleSystem =
		new ParticleSystem(renderingSystem->GetDevice(), renderingSystem->GetDeviceContext(), emitterDesc, simulatorDesc);
	lightPass->particleSystems.push_back(leafParticleSystem);
	leafParticleSystem->camera = lightPass->GetCamera();

	D3D11_BLEND_DESC particleBlendDesc = CD3D11_BLEND_DESC(CD3D11_DEFAULT{});
	particleBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	particleBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	particleBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	particleBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	particleBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	particleBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	particleBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	particleBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	leafParticleSystem->SetBlendState(particleBlendDesc);
	leafParticleSystem->SetTexture(
		new Texture(renderingSystem->GetDevice(), "./Leaf.dds", aiTextureType_DIFFUSE));
	leafParticleSystem->SetEmissionRate(0);

	// Snowflakes Particles
	emitterDesc =
	{
		Matrix::Identity,
		{ 0, 3, 0, 1 },
		{ 1, 1, 1, 1 },
		{ 1, 1, 1, 1 },
		80, 8, 15, 1,
		0.5, 0.5,
		0, 3.1415 * 2,
		3.1415 / 10, 0, { 0, 0 }
	};
	simulatorDesc = {
		{ 0, -5, 0, 0 }
	};
	snowParticleSystem =
		new ParticleSystem(renderingSystem->GetDevice(), renderingSystem->GetDeviceContext(), emitterDesc, simulatorDesc);
	lightPass->particleSystems.push_back(snowParticleSystem);
	snowParticleSystem->camera = lightPass->GetCamera();

	particleBlendDesc = CD3D11_BLEND_DESC(CD3D11_DEFAULT{});
	particleBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	particleBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	particleBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	particleBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	particleBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	particleBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	particleBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	particleBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	snowParticleSystem->SetBlendState(particleBlendDesc);
	snowParticleSystem->SetTexture(
		new Texture(renderingSystem->GetDevice(), "./Snowflake.dds", aiTextureType_DIFFUSE));
	snowParticleSystem->SetEmissionRate(40);


	// Particle Bounces

	normalMap = new Texture(renderingSystem->GetDevice(), gBufferPass->pGBuffer->pNormalSRV.Get());
	worldPosMap = new Texture(renderingSystem->GetDevice(), gBufferPass->pGBuffer->pWorldPosSRV.Get());
	
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderingSystem->GetDevice()->CreateSamplerState(&samplerDesc, &pSampler);

	// ConstantBuffer
	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(ViewProjectionBuffer) + (16 - (sizeof(ViewProjectionBuffer) % 16));  // aligned size
	cbd.StructureByteStride = 0u;

	renderingSystem->GetDevice()->CreateBuffer(&cbd, nullptr, &particlesViewProjMat);


	for (auto partSys : lightPass->particleSystems)
	{
		partSys->hasBounces = true;
		partSys->normalMap = normalMap;
		partSys->worldPosMap = worldPosMap;
		partSys->pSampler = pSampler;
		partSys->particlesViewProjMat = particlesViewProjMat;
	}
}

KatamariRemastered::~KatamariRemastered()
{

}

void KatamariRemastered::Render()
{
	// Отрисовка сцены
	renderingSystem->RenderScene(scene);
}

void KatamariRemastered::Update(float deltaTime)
{
	// DEPTHMAP777
	gbm->UpdateBuffers(renderingSystem->GetDeviceContext());

	leafParticleSystem->SetEmitPosition(Vector4(Vector3(player->position)));
	float dt = 5 - min(max(0, timer._GetCurrentTime() - lastTimeGrowth), 5);
	leafParticleSystem->SetEmissionRate(20 * dt);
	leafParticleSystem->Update(deltaTime);

	snowFlowDirection = Vector3::Transform(snowFlowDirection, Matrix::CreateRotationZ(13 * deltaTime));
	snowParticleSystem->SetEmitDir(snowFlowDirection);
	snowParticleSystem->Update(deltaTime);

	{
		float curr_angle = XM_PIDIV4 * sin(10*timer._GetCurrentTime());
		Vector3 _spotDir = Vector3{ sin(curr_angle), -cos(curr_angle), 0};
		_spotDir.Normalize();
		spotLight->spotLightData.Direction = _spotDir;
	}

	for (auto gameObject : scene.gameObjects) {
		gameObject->Update(deltaTime);
	}

	for (Item* item : items)
	{
		if (item->CheckCollision(*player))
		{
			item->AttachToBall(player);
			lastTimeGrowth = timer._GetCurrentTime();
		}
	}

	for (size_t i = 0; i < pointLightCount; i++)
	{
		pointLights[i]->pointLightData.Position =
			Vector3::Transform(
				pointLightInitPositions[i],
				//Matrix::CreateRotationY(deltaTime * 1.0f)
				player->GetRelativeTransform()
			);
	}

	renderingSystem->GetMainCamera()->Update(deltaTime, player->mWorldMatrix, player->GetMoveDir(), CameraFOV * player->radius);


}


void KatamariRemastered::HandleMoveDown(Keys key)
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

void KatamariRemastered::HandleMouseMove(const InputDevice::MouseMoveEventArgs& args)
{
	renderingSystem->GetMainCamera()->RotatePitch(-deltaTime * args.Offset.y * 1.0);
	player->AddTurn(args.Offset.x * 1.0f, deltaTime);
}
