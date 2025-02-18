#pragma once

#include <iostream>
#include <windows.h>
#include <WinUser.h>
#include <wrl.h>

class DisplayWin32 
{
public:
	DisplayWin32();
	void InitWindow();
	int ScreenHeight;
	int ScreenWidth;
	HMODULE Module;
	HINSTANCE hInstance;
	HWND hWnd;
	WNDCLASSEX wc;
	LPCWSTR applicationName;
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);
};