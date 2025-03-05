#include "Ball.h"

Ball::Ball()
{
}

Ball::Ball(DirectX::XMFLOAT4 vertexPositions[4],
	DirectX::XMFLOAT4 colors[4],
	Microsoft::WRL::ComPtr<ID3D11Device> device,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
	ID3DBlob* vertexBC, ID3D11RenderTargetView* rtv,
	ID3D11VertexShader* vs, ID3D11PixelShader* ps, DisplayWin32* display,
	DirectX::XMFLOAT4 startPos, PlayerPaddle* player, NPCPaddle* npc)
	: Square(vertexPositions, colors, startPos, device, context, vertexBC),
	minClamp(minClamp), maxClamp(maxClamp), directionY(1.0f), directionX(-0.5f), speed(0.5f), renderTargetView(rtv),
	vertexShader(vs), pixelShader(ps), Display(display), position(startPos), player(player), npc(npc),
	Flicker(0.1f)
{
	ScalingShape(changedScaleX,changedScaleY,0);
	RotateShape(DirectX::XMVectorSet(0, 0, 1, 1), 30); // Вращение вокруг Z-оси
}

void Ball::Update(float deltaTime)
{
	UINT strides[] = { 32 };
	UINT offsets[] = { 0 };

	SetupIAStage(strides, offsets);
	context->RSSetState(rastState);

	SetupViewport();

	//square->SetupIAStage(strides, offsets);
	SetVertexAndPixelShaders();

	SetBackBufferOutput(1, &renderTargetView, nullptr);

	// Двигаем ракетку
	MoveShape(directionX * speed * deltaTime, directionY * speed * deltaTime, 0);
	if (CheckBorderCollision())
	{
		const float pushBackDistance = 0.05f; // Смещение для предотвращения залипания

		DirectX::XMMATRIX pushBack = DirectX::XMMatrixIdentity();

		if (hitNPC && directionX > 0) //&& directionX > 0 for limit reflections inside object (paddle)
		{
			directionX *= -1;
			pushBack = DirectX::XMMatrixTranslation(pushBackDistance * directionX, 0, 0);
		}
		if (hitPlayer && directionX < 0)
		{
			directionX *= -1;
			pushBack = DirectX::XMMatrixTranslation(pushBackDistance * directionX, 0, 0);
		}
		if (hitY)
		{
			directionY *= -1;
			pushBack = DirectX::XMMatrixTranslation(0, pushBackDistance * directionY, 0);
		}

		// Смещение для выхода из коллизии
		transformData.translation = DirectX::XMMatrixMultiply(transformData.translation, pushBack);
	}
}
void Ball::SetupViewport()
{
	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(Display->ScreenWidth);
	viewport.Height = static_cast<float>(Display->ScreenHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	context->RSSetViewports(1, &viewport);
}
void Ball::SetVertexAndPixelShaders()
{
	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);
}
void Ball::SetBackBufferOutput(UINT NumViews, ID3D11RenderTargetView* const* ppRenderTargetViews, ID3D11DepthStencilView* pDepthStencilView)
{
	context->OMSetRenderTargets(NumViews, ppRenderTargetViews, pDepthStencilView);
}
DirectX::BoundingBox Ball::GetBallBoundingBox() const
{
	DirectX::BoundingBox bbox;

	DirectX::XMVECTOR center = DirectX::XMVectorSet(position.x * changedScaleX,
		position.y * changedScaleY,
		position.z, 1.0f);
	DirectX::XMFLOAT3 extents(width * 0.5f * changedScaleX,
		height * 0.5f * changedScaleY,
		0.0f);
	DirectX::XMStoreFloat3(&(bbox.Center), DirectX::XMVector4Transform(center, transformData.translation));

	bbox.Extents = extents;
	return bbox;
}
bool Ball::CheckBorderCollision()
{
	DirectX::BoundingBox* screenBox = Display->GetScreenBorders();
	DirectX::BoundingBox playerBox = player->GetPlayerBoundingBox();
	DirectX::BoundingBox npcBox = npc->GetNPCBoundingBox();
	DirectX::BoundingBox ballBox = GetBallBoundingBox();
	hitY = screenBox[0].Intersects(ballBox) || screenBox[1].Intersects(ballBox);
	hitXNPC = screenBox[2].Intersects(ballBox);
	hitXPlayer = screenBox[3].Intersects(ballBox);
	hitPlayer = ballBox.Intersects(playerBox);
	hitNPC = ballBox.Intersects(npcBox);
	if (hitXPlayer)
	{
		npc->ScoreCount++;
		npc->speed = npc->DefaultSpeed;
		player->speed = player->DefaultSpeed;
		speed = defaultSpeed;
		Flicker = 0.1f;
		changedScaleX = 1;
		changedScaleY = 1;
		std::cout << "NPC score count = " << npc->ScoreCount << std::endl;
		transformData.translation = DirectX::XMMatrixIdentity();
	}
	if (hitPlayer || hitNPC)
	{
		speed += upgradeSpeed;
		npc->speed += upgradeSpeed;
		player->speed += upgradeSpeed;
	}
	if (hitXNPC)
	{
		player->ScoreCount++;
		Flicker += 0.3f;
		std::cout << "Player score count = " << player->ScoreCount << std::endl;
		transformData.translation = DirectX::XMMatrixIdentity();
	}
	return hitY || hitPlayer || hitNPC;
}


