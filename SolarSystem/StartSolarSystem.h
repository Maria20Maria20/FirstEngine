#pragma once
#include <CPP/Game.h>
#include <CPP/Cube.h>
#include <CPP/Sphere.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

class StartSolarSystem: public Game
{
public:
	StartSolarSystem();
	int InstanceObjects();
	Cube* mCube = nullptr;
	Cube* mCube2 = nullptr;
	Sphere* mSphere = nullptr;
private:
	void SolarSystemWindowLoop(std::chrono::steady_clock::time_point& PrevTime, float& totalTime, unsigned int& frameCount);
};