#pragma once
#include "Player.h"
#include <CPP/Game.h>
class StartGame : public Game 
{
public:
	Player* player = nullptr;
	StartGame();
private:
	float deltaTime;
	void KatamariWindowLoop(std::chrono::steady_clock::time_point& PrevTime, float& totalTime, unsigned int& frameCount);
};