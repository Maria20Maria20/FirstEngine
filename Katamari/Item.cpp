//#include "Item.h"
//
//Item::Item(ID3D11Device* device, const std::string& modelsFolder, const DirectX::XMFLOAT3& position)
//{
//    DirectX::XMFLOAT4 color(0.2f, 0.8f, 0.3f, 1.0f);
//    CreateSimpleSphereMesh(radius, 20, 10, color,
//        &vertices, &verticesNum, &indices, &indicesNum);
//    mWorldMatrix = Matrix::CreateTranslation(position);
//
//    numInputElements = 3;
//    IALayoutInputElements = (D3D11_INPUT_ELEMENT_DESC*)malloc(numInputElements * sizeof(D3D11_INPUT_ELEMENT_DESC));
//
//    IALayoutInputElements[0] =
//        D3D11_INPUT_ELEMENT_DESC{
//            "POSITION",
//            0,
//            DXGI_FORMAT_R32G32B32_FLOAT,
//            0,
//            0,
//            D3D11_INPUT_PER_VERTEX_DATA,
//            0 };
//
//    IALayoutInputElements[1] =
//        D3D11_INPUT_ELEMENT_DESC{
//            "COLOR",
//            0,
//            DXGI_FORMAT_R32G32B32A32_FLOAT,
//            0,
//            D3D11_APPEND_ALIGNED_ELEMENT,
//            D3D11_INPUT_PER_VERTEX_DATA,
//            0 };
//    IALayoutInputElements[2] =
//        D3D11_INPUT_ELEMENT_DESC{
//            "TEXCOORD",
//            0,
//            DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT,
//            0,
//            D3D11_APPEND_ALIGNED_ELEMENT, // 28,
//            D3D11_INPUT_PER_VERTEX_DATA,
//            0 };
//
//
//    shaderFilePath = L"./Shaders/PlaneShader.hlsl";
//    this->textures.push_back(Texture(device, "models\\Textures\\basketballskin.dds", aiTextureType_DIFFUSE));
//    hasTexture = true;
//}
//
//void Item::LoadRandomModel(const std::string& folder)
//{
//    static std::random_device rd;
//    static std::mt19937 gen(rd());
//
//    auto models = GetModelList(folder);
//    if (models.empty()) return;
//
//    std::uniform_int_distribution<> distr(0, models.size() - 1);
//    auto chosen_model = models[distr(gen)];
//    //auto chosen_model = "models\\plane.obj";
//    ModelLoader::LoadModel(chosen_model, this, ModelLoader::VertexAttrFlags::POSITION | ModelLoader::VertexAttrFlags::TEXTURE);
//
//    // ModelLoader::LoadModel("models\\suzanne.obj", this);
//
//    float maxDistance = 0.0f;
//    for (int i = 0; i < this->verticesNum; i++)
//    {
//        XMFLOAT3 pos = this->vertices[i].pos;
//        float dist = sqrt(pos.x * pos.x + pos.y * pos.y + pos.z * pos.z);
//        maxDistance = max(maxDistance, dist);
//    }
//    modelRadius = maxDistance;
//
//    std::cout << chosen_model << " :: " << verticesNum << " :: " << StringHelper::GetFileNameWithoutExtension(chosen_model) << "\n";
//
//
//    if (chosen_model == "models\\plane.obj") {
//        this->hasTexture = true;
//    }
//}
//
//std::vector<std::string> Item::GetModelList(const std::string& modelsFolder)
//{
//    std::vector<std::string> models;
//    for (const auto& entry : std::filesystem::directory_iterator(modelsFolder))
//    {
//        if (entry.is_regular_file() &&
//            (entry.path().extension() == ".fbx" ||
//                entry.path().extension() == ".obj"))
//        {
//            models.push_back(entry.path().string());
//        }
//    }
//    return models;
//}
//
//Vector3 Item::GetCenterLocation()
//{
//    return Vector3(initialPosition);
//}
//
//bool Item::CheckCollision(StickyBall& ball)
//{
//    if (isAttached) return false;
//
//    Vector3 ballPos = ball.GetCenterLocation();
//    Vector3 objPos = GetCenterLocation();
//    float distance = Vector3::Distance(ballPos, objPos);
//    return distance < (ball.radius + radius);
//}
//
//void Item::AttachToBall(Player* ball)
//{
//    isAttached = true;
//    player = ball;
//
//    attachTransform = mWorldMatrix * XMMatrixInverse(nullptr, Matrix::CreateScale(1 / ball->radius) * ball->mWorldMatrix);
//}
//
//void Item::Update(float deltaTime)
//{
//    if (isAttached && player)
//    {
//        mWorldMatrix = attachTransform * Matrix::CreateScale(1 / player->radius) * player->mWorldMatrix;
//    }
//    else {
//        mWorldMatrix = Matrix::CreateScale(appliedScale) * Matrix::CreateTranslation(initialPosition);
//    }
//}
//
//
