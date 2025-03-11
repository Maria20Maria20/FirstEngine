#pragma once
#include <CPP/Game.h>
#include <CPP/Cube.h>
#include <CPP/Sphere.h>
#include "Planet.h"
#include "Camera.h"


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
	Planet* focusedBody = nullptr;
	void SolarSystemWindowLoop(std::chrono::steady_clock::time_point& PrevTime, float& totalTime, unsigned int& frameCount);
	void HandleMoveDown(Keys key);
};