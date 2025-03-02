#pragma once
#include <d3d11.h>
#include <Windows.h>

class InputDevice
{
public:
    InputDevice();

    void Update();

    bool upPressed = false;
    bool downPressed = false;
};
