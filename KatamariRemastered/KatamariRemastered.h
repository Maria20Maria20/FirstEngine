#pragma once
#include <DeferredGame.h>
#include <CPP/GBufferPass.h>
#include <CPP/LightPass.h>
#include "Player.h"
#include "Ground.h"
#include "Item.h"
#include <Light/AmbientLight.h>
#include <Light/DirectionalLight.h>

class KatamariRemastered : public DeferredGame
{
public:
    KatamariRemastered();
    ~KatamariRemastered();

    void Update(float deltaTime) override;
    void Render() override;
    
    Player* player = nullptr;
    Ground* ground;
    std::vector<Item*> items;

    float CameraFOV = 3;

    void HandleMoveDown(Keys key);
    void HandleMouseMove(const InputDevice::MouseMoveEventArgs& args);
    /*
    std::vector<CollectibleObject*> collectibles;
    void SpawnCollectibles();
    StickyBall* ball;
    Floor* floor;

    DirectionalLight* _dl_1;
    SpotLight* _sl_1;
    std::vector<PointLight*> pointLights;

    XMFLOAT3 pointLightColors[8];
    XMFLOAT3 pointLightDirections[8];
    float pointLightLifeTime[8];
    float lifeTime = 2.0f;
    size_t currPointLightBullet = 0;

    ParticleSystem* dustParticleSystem;
    ParticleSystem* starParticleSystem;
    ParticleSystem* bubbleParticleSystem;
    Vector3 bubbleFlowDirection = { 1, 0, 0 };
    */
};

