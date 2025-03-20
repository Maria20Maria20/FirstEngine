//#pragma once
//#include <CPP\GameObject.h>
//#include "Player.h"
//
//class Item :
//    public GameObject
//{
//public:
//    Item(ID3D11Device* device, const std::string& modelsFolder, const DirectX::XMFLOAT3& position);
//    Item(ID3D11Device* device, float radius, const DirectX::XMFLOAT3& position);
//
//    void Update(float deltaTime) override;
//    Vector3 GetCenterLocation() override;
//    bool CheckCollision(Player& ball);
//    void AttachToBall(Player* ball);
//
//    float radius = 1.0f;
//    float modelRadius = 0.5f;
//    bool isAttached = false;
//    float appliedScale = 1.0f;
//
//private:
//    static std::vector<std::string> GetModelList(const std::string& folder);
//    void LoadRandomModel(const std::string& folder);
//
//    DirectX::XMFLOAT3 initialPosition;
//    Player* player = nullptr;
//    Matrix attachTransform;
//};
