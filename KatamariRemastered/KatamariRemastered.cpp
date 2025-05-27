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

	LightPass* lightPass = new LightPass(renderingSystem->GetDevice(),
		renderingSystem->GetDeviceContext(), renderingSystem->GetBackBuffer(),
		winWidth, winHeight, gBufferPass->pGBuffer, gBufferPass->GetCamera());
	renderingSystem->AddPass(lightPass);

	ground = new Ground(renderingSystem->GetDevice(), { 0, 0, 0 }, 1);
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


	scene.AddGameObject(new AmbientLight(renderingSystem->GetDevice(), { 0,0.5,0.5,0 }));
	scene.AddGameObject(new DirectionalLight(renderingSystem->GetDevice(),
		{ 5.0f, 5.5f, -5.0f }, { 0.0f, -1.0f, 1.0f },
		{ 0.1f, 0.1f, 0.1f, 0 }, { 0.3f, 0.3f, 0.3f, 0 }, { 0.5f, 0.5f, 0.5f, 0 }));
	scene.AddGameObject(new PointLight(
		renderingSystem->GetDevice(),
		{ 0, 1.0f, 0 },
		10.0f,
		{ 0.01f, 1.0f, 0.0f },
		{ 0, 0, 0, 1 },
		// { 0, 0, 0, 1 },
		// { 0, 0, 0, 1 }
		{ 1.0f, 0.2f, 0.0f, 1 },
		{ 1.0f, 0.2f, 0.0f, 1 }
	));
	spotLight = new SpotLight(renderingSystem->GetDevice(),
		{ 0.0f, 4.0f, 0.0f }, 5.0f, { 0.0f, -1.0f, 0.0f },
		10, { 0.01f, 0.05f, 0.0f },
		{ 0, 0.1, 0, 1 }, { 1, 1, 0, 1 }, { 1, 1, 0, 1 });
	scene.AddGameObject(spotLight);


	for (auto gameObject : scene.gameObjects) {
		gameObject->camera = renderingSystem->GetMainCamera();
	}

	renderingSystem->GetMainCamera()->SwitchToFollowMode(player->position, player->GetMoveDir(), player->radius);


	// Stars Particles
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
	starParticleSystem =
		new ParticleSystem(renderingSystem->GetDevice(), renderingSystem->GetDeviceContext(), emitterDesc, simulatorDesc);
	lightPass->particleSystems.push_back(starParticleSystem);
	starParticleSystem->camera = lightPass->GetCamera();

	D3D11_BLEND_DESC particleBlendDesc = CD3D11_BLEND_DESC(CD3D11_DEFAULT{});
	particleBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	particleBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	particleBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	particleBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	particleBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	particleBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	particleBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	particleBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	starParticleSystem->SetBlendState(particleBlendDesc);
	starParticleSystem->SetTexture(
		new Texture(renderingSystem->GetDevice(), "./Leaf.dds", aiTextureType_DIFFUSE));
	starParticleSystem->SetEmissionRate(0);

	// Bubble Particles
	emitterDesc =
	{
		Matrix::Identity,
		{ 0, 40, 0, 1 },
		{ 1, 1, 1, 1 },
		{ 1, 1, 1, 0 },
		100, 8, 8, 1,
		0.5, 0.5,
		0, 3.1415 * 2,
		3.1415 / 10, 0, { 0, 0 }
	};
	simulatorDesc = {
		{ 0, -5, 0, 0 }
	};
	bubbleParticleSystem =
		new ParticleSystem(renderingSystem->GetDevice(), renderingSystem->GetDeviceContext(), emitterDesc, simulatorDesc);
	lightPass->particleSystems.push_back(bubbleParticleSystem);
	bubbleParticleSystem->camera = lightPass->GetCamera();

	particleBlendDesc = CD3D11_BLEND_DESC(CD3D11_DEFAULT{});
	particleBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	particleBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	particleBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	particleBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	particleBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	particleBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	particleBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	particleBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	bubbleParticleSystem->SetBlendState(particleBlendDesc);
	bubbleParticleSystem->SetTexture(
		new Texture(renderingSystem->GetDevice(), "./Snowflake.dds", aiTextureType_DIFFUSE));
	bubbleParticleSystem->SetEmissionRate(40);
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
	starParticleSystem->SetEmitPosition(Vector4(Vector3(player->position)));
	float dt = 5 - min(max(0, timer._GetCurrentTime() - lastTimeGrowth), 5);
	starParticleSystem->SetEmissionRate(20 * dt);
	starParticleSystem->Update(deltaTime);

	bubbleFlowDirection = Vector3::Transform(bubbleFlowDirection, Matrix::CreateRotationY(10 * deltaTime));
	bubbleParticleSystem->SetEmitDir(bubbleFlowDirection);
	bubbleParticleSystem->Update(deltaTime);

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
	player->AddTurn(args.Offset.x * 1.0f, deltaTime);
}
