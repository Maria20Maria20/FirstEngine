#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <iostream>
#include <d3dcompiler.h>
#include "DisplayWin32.h"
#include "DisplayWin32.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

class Game {
protected:
    // Приватные конструкторы
    Game() : display(new DisplayWin32()) {}
    Game(const Game&) = delete;             // Запрет копирования
    Game& operator=(const Game&) = delete;  // Запрет присваивания

    static Game* instance;  // Единственный экземпляр

public:
    DisplayWin32* display;

    // Метод доступа к экземпляру
    static Game& getInstance();

    // Метод для явного удаления экземпляра
    static void destroyInstance();

    ~Game() {
        delete display;  // Освобождаем ресурсы
    }
};

