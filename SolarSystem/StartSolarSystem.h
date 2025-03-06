#pragma once
#include <CPP/Game.h>
#include <CPP/Cube.h>
class StartSolarSystem: public Game
{
public:
	StartSolarSystem();
	int InstanceObjects();
	std::unique_ptr<Cube> mCube;
private:
	void SolarSystemWindowLoop(std::chrono::steady_clock::time_point& PrevTime, float& totalTime, unsigned int& frameCount);
};