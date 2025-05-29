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
#include <Light/GBufferMaps.h>

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
    std::vector<PointLight*> pointLights;
    UINT pointLightCount = 10;
    XMFLOAT3 pointLightInitPositions[10];
    GBufferMaps* gbm;

    //ParticleSystem* dustParticleSystem;
    ParticleSystem* leafParticleSystem;
    ParticleSystem* snowParticleSystem;
    Vector3 snowFlowDirection = { 1, 0, 0 };

    float lastTimeGrowth = -100;
    
    LightPass* lightPass;

    // Particle Bounces
    Texture* normalMap;
    Texture* worldPosMap;

    Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;

    struct ViewProjectionBuffer
    {
        Matrix vMat;
        Matrix pMat;
        bool hasBounces;
    };
    Microsoft::WRL::ComPtr<ID3D11Buffer> particlesViewProjMat;
};

