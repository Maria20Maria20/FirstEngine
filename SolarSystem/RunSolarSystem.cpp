#include "StartSolarSystem.h"
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

int main()
{
	StartSolarSystem* game = new StartSolarSystem();
	game->InstanceObjects();
}
