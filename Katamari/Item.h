#pragma once
#include <CPP\GameObject.h>
#include "Player.h"

class Item :
    public GameObject
{
public:
    // Item(ID3D11Device* device, const std::string& modelsFolder, const DirectX::XMFLOAT3& position);
    Item(ID3D11Device* device, ID3DBlob* vertexBC, ID3D11VertexShader* vs, ID3D11PixelShader* ps,
        ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* depthStencilView,
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Camera* camera,
        ObjectType objectType, const DirectX::XMFLOAT3& startPosition,
        float radius, float changedScale, LPCWSTR shaderFilePath = L"./Shaders/CubeShader.hlsl");

    void Update(float deltaTime);
    Vector3 GetCenterLocation();
    bool CheckCollision(Player& ball);
    void AttachToBall(Player* ball);
    //void LoadRandomModel(const std::string& folder);
    //std::vector<std::string> GetModelList(const std::string& modelsFolder);

    float radius;
    float modelRadius = 0.5f;
    bool isAttached = false;
    float appliedScale = 1.0f;

private:
   /* static std::vector<std::string> GetModelList(const std::string& folder);
    void LoadRandomModel(const std::string& folder);*/

    Player* player = nullptr;
    Matrix attachTransform;

    Matrix initRandomRotation;

public:
    DirectX::XMFLOAT3 initialPosition;

    Camera* camera;
    float scale = 1.0f;

};
