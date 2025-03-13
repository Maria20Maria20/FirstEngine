#include "LoadModel.h"
int main()
{
	Assimp::Importer imp;
	auto testModel = imp.ReadFile("Models\\testModel.fbx",
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices
	);
	return 0;
}