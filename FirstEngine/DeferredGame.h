#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#include <chrono>

#include "Scene.h"
#include "DeferredRenderer.h"

#include "CPP/InputDevice.h"
#include "CPP/Delegates.h"
#include "CPP/DisplayWin32.h"
#include "CPP/RenderingSystem.h"


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

class DeferredGame
{
public:

    class GameTimer
    {
    public:
        GameTimer() : deltaTime(0.0f)
        {
            lastTime = std::chrono::high_resolution_clock::now();
        };

        void Tick()
        {
            auto currentTime = std::chrono::high_resolution_clock::now();
            deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
            lastTime = currentTime;
        }

        float GetDeltaTime() const
        {
            return deltaTime;
        }

    private:
        std::chrono::high_resolution_clock::time_point lastTime;
        float deltaTime;
    };
public:
    DeferredGame();
    DeferredGame(LPCWSTR appName);
    virtual ~DeferredGame();

    virtual void Run();

    virtual void Update(float deltaTime) = 0;
    virtual void Render();

    GameTimer timer;
    Scene scene;
    RenderingSystem* renderingSystem;

    DisplayWin32* displayWindow;

    HINSTANCE hInstance;
    LPCWSTR applicationName;

    int winWidth = 800;
    int winHeight = 800;

    float deltaTime = 0.0f;
};

