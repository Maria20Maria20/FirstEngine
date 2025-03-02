#include "InputDevice.h"

InputDevice::InputDevice()
{
}

void InputDevice::Update()
{
	upPressed = GetAsyncKeyState(VK_UP) & 0x8000;
	downPressed = GetAsyncKeyState(VK_DOWN) & 0x8000;
}
