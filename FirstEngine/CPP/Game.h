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
    // ��������� ������������
    Game() : display(new DisplayWin32()) {}
    Game(const Game&) = delete;             // ������ �����������
    Game& operator=(const Game&) = delete;  // ������ ������������

    static Game* instance;  // ������������ ���������

public:
    DisplayWin32* display;

    // ����� ������� � ����������
    static Game& getInstance();

    // ����� ��� ������ �������� ����������
    static void destroyInstance();

    ~Game() {
        delete display;  // ����������� �������
    }
};

