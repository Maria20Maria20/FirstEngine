#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <iostream>
#include <d3dcompiler.h>
#include <CPP/DisplayWin32.h>
#include <chrono>
#include <d3d.h>
#include <directxmath.h>
#include <CPP/TriangleComponent.h>
#include <CPP/ShapeType.h>
#include <CPP/Triangle.h>
#include <CPP/Square.h>
#include "NPCPaddle.h"
#include "PlayerPaddle.h"
#include "Ball.h"
#include <CPP/Game.h>
#include <CPP/Cube.h>


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

class PongGame : public Game
{
public:
	PongGame();
	int InstanceObjects();

	std::unique_ptr<Cube> mCube;
private:
	Ball* ball;
	NPCPaddle* npcPaddle = nullptr;
	PlayerPaddle* playerPaddle = nullptr;
	void PongWindowLoop(std::chrono::steady_clock::time_point& PrevTime, float& totalTime, unsigned int& frameCount);
};
