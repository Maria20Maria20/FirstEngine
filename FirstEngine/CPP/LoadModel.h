#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <filesystem>
#include <random>
#include "StringHelper.h"
#include <wrl.h>
#include "GameObject.h"

class LoadModel
{
public:
    enum VertexAttrFlags : UINT {
        POSITION = 0x1,
        COLOR = 0x2,
        TEXTURE = 0x4,
        NORMAL = 0x8,
    };
    LoadModel(const std::string& path, GameObject* gameObject, UINT attrFlags = VertexAttrFlags::POSITION);
    // void ProcessMesh(aiMesh* mesh, const aiScene* scene, GameObject* meshNode);
};