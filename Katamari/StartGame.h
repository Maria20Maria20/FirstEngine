#pragma once
#include "Player.h"
#include <CPP/Game.h>
#include "Ground.h"
class StartGame : public Game 
{
public:
	Player* player = nullptr;
	Ground* plane;
	StartGame();
private:
	float deltaTime;
	void KatamariWindowLoop(std::chrono::steady_clock::time_point& PrevTime, float& totalTime, unsigned int& frameCount);
	void HandleMoveDown(Keys key);
	void HandleMouseMove(const InputDevice::MouseMoveEventArgs& args);
};