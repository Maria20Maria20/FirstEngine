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

	for (auto gameObject : scene.gameObjects) {
		gameObject->camera = renderingSystem->GetMainCamera();
	}

	renderingSystem->GetMainCamera()->SwitchToFollowMode(player->position, player->GetMoveDir(), player->radius);
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
	for (auto gameObject : scene.gameObjects) {
		gameObject->Update(deltaTime);
	}

	for (Item* item : items)
	{
		if (item->CheckCollision(*player))
		{
			item->AttachToBall(player);
			// play.Grow(obj.radius / deltaTime);
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
