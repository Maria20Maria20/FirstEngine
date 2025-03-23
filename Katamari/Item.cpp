#include "Item.h"

Item::Item(ID3D11Device* device, ID3DBlob* vertexBC, ID3D11VertexShader* vs, ID3D11PixelShader* ps,
    ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* depthStencilView,
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Camera* camera,
    ObjectType objectType, const DirectX::XMFLOAT3& startPosition, float radius, float changedScale, LPCWSTR shaderFilePath)
    : GameObject(device, vertexBC, vs, ps, rtv, depthStencilView, context, ObjectType::CUBE, shaderFilePath)
{
    // DirectX::XMFLOAT4 color(0.2f, 0.8f, 0.3f, 1.0f);
    initRandomRotation = GetRandomRotateTransform(); 
    initialPosition = startPosition;
    initialPosition.y = radius;
    this->scale = changedScale;
    this->camera = camera;
    this->radius = radius;
    // for loaded model
    appliedScale = radius / modelRadius;


    // shaderFilePath = L"./Shaders/PlaneShader.hlsl";
    // this->textures.push_back(Texture(device, "models\\Textures\\basketballskin.dds", aiTextureType_DIFFUSE));
    // hasTexture = true;
}

Vector3 Item::GetCenterLocation()
{
    return initialPosition;
}

bool Item::CheckCollision(Player& player)
{
    if (isAttached) return false;

    Vector3 ballPos = player.GetCenterLocation();
    Vector3 objPos = GetCenterLocation();
    float distance = Vector3::Distance(ballPos, objPos);
    return distance < (player.radius + radius);
}

void Item::AttachToBall(Player* ball)
{
    isAttached = true;
    player = ball;

    attachTransform = mWorldMatrix * XMMatrixInverse(nullptr, Matrix::CreateScale(1 / player->radius) * player->mWorldMatrix);
}

void Item::Update(float deltaTime)
{
    if (isAttached && player)
    {
        mWorldMatrix = attachTransform * Matrix::CreateScale(1 / player->radius) * player->mWorldMatrix;
    }
    else 
    {
        mWorldMatrix = Matrix::CreateScale(scale) * 
            Matrix::CreateTranslation(initialPosition);
    }

    Matrix viewMat = camera->GetViewMatrix();
    Matrix projMat = camera->GetProjectionMatrix();

    // Update constant buffer
    cb.worldViewProj = mWorldMatrix * (XMMATRIX)(viewMat * projMat);
}



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

