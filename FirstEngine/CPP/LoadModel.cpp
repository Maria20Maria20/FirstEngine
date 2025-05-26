#include "LoadModel.h"


LoadModel::LoadModel(const std::string& path, GameObject* gameObject, UINT attrFlags)
{
	Assimp::Importer importer;
	const aiScene* pModel = importer.ReadFile(path,
		aiProcess_Triangulate | aiProcess_FlipUVs
		| (((attrFlags & LoadModel::VertexAttrFlags::NORMAL) != 0) ? aiProcess_GenNormals : 0x0)
	);

	//	aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
	// aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_MakeLeftHanded

	if (!pModel || pModel->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pModel->mRootNode)
	{
		gameObject = nullptr;
		return;
	}

	unsigned int meshesNum = 1; // pModel->mNumMeshes;
	
	gameObject->verticesNum = 0;
	gameObject->indicesNum = 0;

	gameObject->verticesNum += pModel->mMeshes[0]->mNumVertices;
	gameObject->indicesNum += pModel->mMeshes[0]->mNumFaces * 3; //3, because use triangle

	gameObject->vertices = (Vertex*)calloc(gameObject->verticesNum, sizeof(Vertex));
	gameObject->indices = (int*)calloc(gameObject->indicesNum, sizeof(int));

	size_t vertexIdx = 0;
	size_t indexIdx = 0;

	const auto pMesh = pModel->mMeshes[0];

	for (unsigned i = 0; i < pMesh->mNumVertices; i++)
	{
		(gameObject->vertices)[vertexIdx++] = {
			XMFLOAT4(
			pMesh->mVertices[i].x,
			pMesh->mVertices[i].y,
			pMesh->mVertices[i].z,
				1
			) };

		if (pMesh->mTextureCoords[0] && (attrFlags & VertexAttrFlags::TEXTURE))
		{
			//std::cout << "aiModel has texture!\n";
			(gameObject->vertices)[vertexIdx - 1].texCoord.x = (float)pMesh->mTextureCoords[0][i].x;
			(gameObject->vertices)[vertexIdx - 1].texCoord.y = (float)pMesh->mTextureCoords[0][i].y;
			//std::cout << (float)pMesh->mTextureCoords[0][i].x << ", " << (float)pMesh->mTextureCoords[0][i].y << "\n";
		}
		
		if (attrFlags & VertexAttrFlags::NORMAL) {
			(gameObject->vertices)[vertexIdx - 1].normal = {
				XMFLOAT3(
				pMesh->mNormals[i].x,
				pMesh->mNormals[i].y,
				pMesh->mNormals[i].z
				) };
		}
	}

	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<float> distr(0, 1);

	for (unsigned i = 0; i < pMesh->mNumFaces; i++)
	{
		aiFace face = pMesh->mFaces[i];
		assert(face.mNumIndices == 3);
		auto col = XMFLOAT4(distr(gen), distr(gen), distr(gen), 1);
		for (unsigned j = 0; j < face.mNumIndices; j++) {
			(gameObject->indices)[indexIdx++] = face.mIndices[j];
			(gameObject->vertices)[face.mIndices[j]].color = col;
		}
	}
	/*
	if (pMesh->mTextureCoords[0] && (attrFlags & VertexAttrFlags::TEXTURE))
	{
		aiMaterial* material = pModel->mMaterials[pMesh->mMaterialIndex];
		std::vector<Texture> diffuseTextures = gameObject->LoadMaterialTextures(material, aiTextureType::aiTextureType_DIFFUSE, pModel);
		gameObject->textures.insert(gameObject->textures.end(), diffuseTextures.begin(), diffuseTextures.end());
	}
	*/
	return;
}