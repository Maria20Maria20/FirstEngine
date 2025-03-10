#pragma once
#include <CPP/Game.h>
#include <CPP/Cube.h>
#include <CPP/Sphere.h>
#include "Planet.h"
#include "Camera.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

using namespace DirectX::SimpleMath;

class StartSolarSystem: public Game
{
public:
	StartSolarSystem();
	int InstanceObjects();
	Cube* mCube = nullptr;
	Cube* mCube2 = nullptr;
	Planet* sun = nullptr;
	Planet* satellite = nullptr;

	Camera camera;
private:
	void SolarSystemWindowLoop(std::chrono::steady_clock::time_point& PrevTime, float& totalTime, unsigned int& frameCount);
};