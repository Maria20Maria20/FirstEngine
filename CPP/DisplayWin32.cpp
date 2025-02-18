#include "DisplayWin32.h"

DisplayWin32::DisplayWin32()
{
    ScreenHeight = 800;
    ScreenWidth = 800;
    applicationName = L"My3DApp";
    hInstance = GetModuleHandle(nullptr);    
}

void DisplayWin32::InitWindow()
{
    // ��������� ��������� WNDCLASSEX
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = applicationName;

    // ������������ ����� ����
    RegisterClassEx(&wc);

    RECT windowRect = { 0, 0, ScreenWidth, ScreenHeight };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    auto dwStyle = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME;

    auto posX = (GetSystemMetrics(SM_CXSCREEN) - ScreenWidth) / 2;
    auto posY = (GetSystemMetrics(SM_CYSCREEN) - ScreenHeight) / 2;

    // ������ ����
    hWnd = CreateWindowEx(WS_EX_APPWINDOW, applicationName, applicationName,
        dwStyle,
        posX, posY,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr, nullptr, hInstance, nullptr);

    ShowWindow(hWnd, SW_SHOW);
    SetForegroundWindow(hWnd);
    SetFocus(hWnd);

    ShowCursor(true);
}

// ���������� ��������� ����
LRESULT CALLBACK DisplayWin32::WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
    switch (umessage)
    {
    case WM_KEYDOWN:
        std::cout << "Key: " << static_cast<unsigned int>(wparam) << std::endl;
        if (static_cast<unsigned int>(wparam) == 27) PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hwnd, umessage, wparam, lparam);
    }
}
