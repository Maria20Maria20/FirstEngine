#pragma once
#include <CPP/Game.h>
#include <CPP/GameObject.h>
#include "Planet.h"
#include "Camera.h"
#include "PlanetSystem.h"


class StartSolarSystem: public Game
{
public:
	StartSolarSystem();
	int InstanceObjects();
	Planet* sun = nullptr;
	Planet* satellite = nullptr;
	Planet* earth = nullptr;
	Planet* moon = nullptr;

	PlanetSystem planetSystem;

	Camera camera;
private:
	Planet* focusedBody = nullptr;
	void SolarSystemWindowLoop(std::chrono::steady_clock::time_point& PrevTime, float& totalTime, unsigned int& frameCount);
	void HandleMoveDown(Keys key);
	float	deltaTime;
};