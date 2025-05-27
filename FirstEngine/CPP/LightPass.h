#pragma once
#include <stdexcept>

#include "RenderPass.h"
#include "Camera.h"
#include "GBuffer.h"
//#include "LightCollection.h"

#include "../ParticleSystem/ParticleSystem.h"

class LightPass :
    public RenderPass
{
public:
    LightPass(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11Texture2D* backBuffer,
        UINT screenWidth, UINT screenHeight, GBuffer* pGBuffer, Camera* camera);

    void StartFrame() override;
    void Pass(const Scene& scene) override;
    void EndFrame() override;

    Camera* GetCamera();
    void SetCamera(Camera* camera);
    Camera* camera;

    UINT screenWidth = 800;
    UINT screenHeight = 800;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;

    GBuffer* pGBuffer;

    ID3D11RenderTargetView* renderTargetView;
    D3D11_VIEWPORT viewport;

    struct CameraData {
        XMMATRIX viewMatInverse;
        XMMATRIX projMatInverse;
        XMFLOAT3 camPos;
        float pad;
    } cameraData;
    
    Texture* pNormalMap;
    Texture* pAlbedoMap;
    Texture* pSpecularMap;
    Texture* pWorldPosMap;
    // Sampler for maps
    Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;

    //Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer;
    //Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDepthStencilState;
    Microsoft::WRL::ComPtr<ID3D11BlendState> pBlendState;

    ID3D11Buffer* pCameraDataPixelConstantConstantBuffer; // slot 0u
    //Bind::PixelConstantBuffer<CamPCB>* camPCB;



public:
    std::vector<ParticleSystem*> particleSystems;

};
