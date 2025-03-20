#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <CPP/GameObject.h>
#include <filesystem>
#include <random>

class ModelLoader
{
public:
    template <class T>
    static void LoadModel(const std::string& path, GameObject<T>* rootNode);
};