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
	Planet* satellite2 = nullptr;
	Planet* satellite3 = nullptr;
	Planet* satellite4 = nullptr;
	Planet* satellite5 = nullptr;
	Planet* earth = nullptr;
	Planet* moon = nullptr;
	Planet* cubeEarth = nullptr;
	Planet* cubeMoon = nullptr;
	Planet* skyBox = nullptr;
	GameObject* grid = nullptr;

	PlanetSystem planetSystem;

	Camera camera;
private:
	Planet* focusedBody = nullptr;
	void SolarSystemWindowLoop(std::chrono::steady_clock::time_point& PrevTime, float& totalTime, unsigned int& frameCount);
	void HandleMoveDown(Keys key);
	void HandleMouseMove(const InputDevice::MouseMoveEventArgs& args);
	float deltaTime;
};