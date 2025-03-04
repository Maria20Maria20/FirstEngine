#include "Game.h"
// ������������� ������������ �����
Game* Game::instance = nullptr;
Game& Game::getInstance()
{
    if (!instance) {
        instance = new Game();
    }
    return *instance;
}

void Game::destroyInstance()
{
    if (instance) {
        delete instance;
        instance = nullptr;
    }
}
