#pragma once
#include <DeferredGame.h>
#include <CPP/GBufferPass.h>
#include <CPP/LightPass.h>
#include "Player.h"
#include "Ground.h"
#include "Item.h"
#include <Light/AmbientLight.h>
#include <Light/DirectionalLight.h>
#include <Light/PointLight.h>
#include <Light/SpotLight.h>

class KatamariRemastered : public DeferredGame
{
public:
    KatamariRemastered();
    ~KatamariRemastered();

    void Update(float deltaTime) override;
    void Render() override;

    void HandleMoveDown(Keys key);
    void HandleMouseMove(const InputDevice::MouseMoveEventArgs& args);

    float CameraFOV = 3;

    Player* player = nullptr;
    Ground* ground;
    std::vector<Item*> items;
    SpotLight* spotLight;
    // std::vector<PointLight*> pointLights;

    //ParticleSystem* dustParticleSystem;
    ParticleSystem* starParticleSystem;
    ParticleSystem* bubbleParticleSystem;
    Vector3 bubbleFlowDirection = { 1, 0, 0 };

    float lastTimeGrowth = -100;
};

