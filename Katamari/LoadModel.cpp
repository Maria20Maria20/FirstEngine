#include "LoadModel.h"
#include "StartGame.h"
int main()
{
	//Assimp::Importer imp;
	//auto testModel = imp.ReadFile("Models\\little_grass_1.fbx",
	//	aiProcess_Triangulate |
	//	aiProcess_JoinIdenticalVertices
	//);
	StartGame* startGame = new StartGame();
	return 0;
}

//LoadModel::LoadModel(const std::string& path, GameObject* rootNode, UINT attrFlags)
//{
//	rootNode->directory = StringHelper::GetDirectoryFromPath(path);
//	Assimp::Importer importer;
//	const aiScene* pModel = importer.ReadFile(path,
//		aiProcess_Triangulate);
//	//	aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
//	// aiProcess_GenNormals | aiProcess_CalcTangentSpace
//
//	if (!pModel || pModel->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pModel->mRootNode)
//	{
//		rootNode = nullptr;
//		return;
//	}
//
//	unsigned int meshesNum = 1; // pModel->mNumMeshes;
//
//	rootNode->verticesNum = 0;
//	rootNode->indicesNum = 0;
//
//	rootNode->verticesNum += pModel->mMeshes[0]->mNumVertices;
//	rootNode->indicesNum += pModel->mMeshes[0]->mNumFaces * 3;
//
//	rootNode->vertices = (CommonVertex*)calloc(rootNode->verticesNum, sizeof(CommonVertex));
//	rootNode->indices = (int*)calloc(rootNode->indicesNum, sizeof(int));
//
//	size_t vertexIdx = 0;
//	size_t indexIdx = 0;
//
//	static std::random_device rd;
//	static std::mt19937 gen(rd());
//	std::uniform_real_distribution<float> distr(0, 1);
//
//	const auto pMesh = pModel->mMeshes[0];
//
//	if (pMesh->mTextureCoords[0])
//	{
//		std::cout << "aiModel has texture!\n";
//	}
//
//	for (unsigned i = 0; i < pMesh->mNumVertices; i++)
//	{
//		(rootNode->vertices)[vertexIdx++] = {
//			XMFLOAT3(
//			pMesh->mVertices[i].x,
//			pMesh->mVertices[i].y,
//			pMesh->mVertices[i].z) };
//
//		if (pMesh->mTextureCoords[0] && (attrFlags & VertexAttrFlags::TEXTURE))
//		{
//			//std::cout << "aiModel has texture!\n";
//			(rootNode->vertices)[vertexIdx - 1].texCoord.x = (float)pMesh->mTextureCoords[0][i].x;
//			(rootNode->vertices)[vertexIdx - 1].texCoord.y = (float)pMesh->mTextureCoords[0][i].y;
//			//std::cout << (float)pMesh->mTextureCoords[0][i].x << ", " << (float)pMesh->mTextureCoords[0][i].y << "\n";
//		}
//	}
//
//	for (unsigned i = 0; i < pMesh->mNumFaces; i++)
//	{
//		aiFace face = pMesh->mFaces[i];
//		assert(face.mNumIndices == 3);
//
//		auto col = XMFLOAT4(distr(gen), distr(gen), distr(gen), 1);
//
//		for (unsigned j = 0; j < face.mNumIndices; j++) {
//			(rootNode->indices)[indexIdx++] = face.mIndices[j];
//
//			(rootNode->vertices)[face.mIndices[j]].color = col;
//
//		}
//	}
//	/*
//	if (pMesh->mTextureCoords[0] && (attrFlags & VertexAttrFlags::TEXTURE))
//	{
//		aiMaterial* material = pModel->mMaterials[pMesh->mMaterialIndex];
//		std::vector<Texture> diffuseTextures = rootNode->LoadMaterialTextures(material, aiTextureType::aiTextureType_DIFFUSE, pModel);
//		rootNode->textures.insert(rootNode->textures.end(), diffuseTextures.begin(), diffuseTextures.end());
//	}
//	*/
//	return;
//}
//
//void LoadModel::ProcessMesh(aiMesh* mesh, const aiScene* scene, GameObject* meshNode)
//{
//	std::vector<Vertex> vertices;
//	std::vector<int> indices;
//
//	// Обработка вершин
//	for (unsigned i = 0; i < mesh->mNumVertices; i++)
//	{
//		Vertex vertex;
//		vertex.pos = XMFLOAT3(
//			mesh->mVertices[i].x,
//			mesh->mVertices[i].y,
//			mesh->mVertices[i].z
//		);
//		vertices.push_back(vertex);
//	}
//
//	// Обработка индексов
//	for (unsigned i = 0; i < mesh->mNumFaces; i++)
//	{
//		aiFace face = mesh->mFaces[i];
//		for (unsigned j = 0; j < face.mNumIndices; j++)
//			indices.push_back(face.mIndices[j]);
//	}
//
//	meshNode->vertices = vertices.data();
//	meshNode->verticesNum = vertices.size();
//	meshNode->indices = indices.data();
//	meshNode->indicesNum = indices.size();
//
//	return;
//}
