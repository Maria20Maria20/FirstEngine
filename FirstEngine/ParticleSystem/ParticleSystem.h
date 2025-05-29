#pragma once

#include <d3d11.h>

#include <d3dcompiler.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <SimpleMath.h>
#include <string>

#include "../CPP/Camera.h"
#include "../CPP/Texture.h"

class ParticleSystem
{

public:

    struct Particle {
        DirectX::XMFLOAT4 Position;
        DirectX::XMFLOAT4 Velocity;
        DirectX::XMFLOAT4 ColorStart;
        DirectX::XMFLOAT4 ColorEnd;

        float SizeStart;
        float SizeEnd;
        float Age;
        float LifeSpan;
        
        float Mass;
        float ScreenSpin;
        float ScreenSpinSpeed;
        float padding;
    };


    struct EmitterPointConstantBuffer
    {
        Matrix rotMatrix;

        Vector4 position;
        Vector4 colorStart;
        Vector4 colorEnd;

        UINT maxSpawn;
        float particlesLifeSpan;
        float particlesBaseSpeed;
        float particlesMass;

        float particleSizeStart;
        float particleSizeEnd;

        float longitudeMin;
        float longitudeMax;

        float latitudeMax;
        float particleScreenSpinSpeed;
        UINT padding[2];
    };

    struct ParticleIndexElement
    {
        float distance; //squared distance from camera
        float index; //index in the particle buffer
    };

    struct InitIndirectComputeArgs1DConstantBuffer
    {
        float nbThreadGroupX;

        UINT padding[3];
    };

    struct SimulateParticlesConstantBuffer
    {
        Vector4 force;
    };

    int m_maxParticles = 4 * 1024;
    //int m_maxParticles = 512;


    Microsoft::WRL::ComPtr<ID3D11Buffer>                m_particleBuffer;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_particleSRV;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   m_particleUAV;

    Microsoft::WRL::ComPtr<ID3D11Buffer>                m_deadListBuffer;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   m_deadListUAV;

    Microsoft::WRL::ComPtr<ID3D11Buffer>                m_aliveIndexBuffer[2];
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_aliveIndexSRV[2];
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   m_aliveIndexUAV[2];
    // Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   m_aliveIndexUAVSorting[2];
    int                                                 m_currentAliveBuffer = 0;

    Microsoft::WRL::ComPtr<ID3D11Buffer>                m_indirectDispatchArgsBuffer[2];    // для комп шейдера
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   m_indirectDispatchArgsUAV[2];       // для рендеринга


    Microsoft::WRL::ComPtr<ID3D11Buffer>                m_initSimulateDispatchArgsBuffer;
    InitIndirectComputeArgs1DConstantBuffer             m_initSimulateDispatchArgsData;

    Microsoft::WRL::ComPtr<ID3D11Buffer>                m_indirectDrawArgsBuffer;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   m_indirectDrawArgsUAV;


    Microsoft::WRL::ComPtr<ID3D11Buffer>                m_deadListCountConstantBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer>                m_deadListCountConstantBuffer_2;
    Microsoft::WRL::ComPtr<ID3D11Buffer>                m_aliveListCountConstantBuffer;

    ParticleSystem() {};
    ParticleSystem(ID3D11Device* device, ID3D11DeviceContext* context,
        EmitterPointConstantBuffer emitterDesc,
        SimulateParticlesConstantBuffer simulatorDesc);
    void LoadCS(LPCWSTR computeFilename, ID3D11ComputeShader* m_computeShader);
    void ResetParticles();

    void Update(float deltaTime);
    void UpdateEmitter(float deltaTime);

    void Render();
    void Emit();
    void Simulate();
    void Draw();

    void SetEmissionRate(float emissionRate) { m_emissionRate = max(0, min(emissionRate, m_maxParticles * 1. / 4)); };
    void IncrementEmissionRate(float deltaEmissionRate) { SetEmissionRate(m_emissionRate + deltaEmissionRate); };
    void DecrementEmissionRate(float deltaEmissionRate) { SetEmissionRate(m_emissionRate - deltaEmissionRate); };

    void SetBlendState(D3D11_BLEND_DESC particleBlendDesc);
    void SetTexture(Texture* newTexture);

    void SetEmitPosition(Vector4 newPosition);
    void SetEmitDir(Vector3 newEmitDir);

    Microsoft::WRL::ComPtr<ID3D11ComputeShader>          m_resetCShader;
    Microsoft::WRL::ComPtr<ID3D11ComputeShader>          m_initSimulateDispatchArgsCShader; // pre-simulate
    Microsoft::WRL::ComPtr<ID3D11ComputeShader>          m_emitParticlesCShader;
    Microsoft::WRL::ComPtr<ID3D11ComputeShader>          m_simulateParticlesCShader;

    Microsoft::WRL::ComPtr<ID3D11Device>               m_d3dDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext>        m_d3dContext;

    Camera* camera;

private:
    struct DeadListCountConstantBuffer
    {
        UINT nbDeadParticles;

        UINT padding[3];
    };

    float m_emissionRate = 100.0f;  // tool
    float m_emissionRateAccumulation = 0.0f;

    EmitterPointConstantBuffer  m_emitterConstantBufferData;
    Microsoft::WRL::ComPtr<ID3D11Buffer>    m_emitterConstantBuffer;

    SimulateParticlesConstantBuffer  m_simulateParticlesConstantBufferData;
    Microsoft::WRL::ComPtr<ID3D11Buffer>    m_simulateParticlesConstantBuffer;

    inline int align(int value, int alignment) { return (value + (alignment - 1)) & ~(alignment - 1); };

    // temp

    struct SceneConstantBuffer
    {
        Vector4 camPosition;
        float dt;
        float rngSeed;
        UINT padding[2];
    };
    SceneConstantBuffer m_sceneConstantBufferData;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_sceneConstantBuffer;

    ID3D11RasterizerState* rasterState;
    ID3D11DepthStencilState* depthState;
    Microsoft::WRL::ComPtr<ID3D11BlendState> m_blendState; // tool
    struct TransformsParticles
    {
        DirectX::XMMATRIX viewMat;
        DirectX::XMMATRIX projMat;
        DirectX::XMMATRIX viewProjMat;
    };
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_viewProjBuffer;

    Microsoft::WRL::ComPtr<ID3D11InputLayout>       pInputLayout;
    Microsoft::WRL::ComPtr<ID3D11VertexShader>      m_renderParticleVS;
    Microsoft::WRL::ComPtr<ID3D11GeometryShader>    m_renderParticleGS;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>       m_renderParticlePS;

    // texture
    Texture* m_texture;    // tool
    ID3D11SamplerState* textureSampler;

public:
    bool hasBounces;

    Texture* normalMap;
    Texture* worldPosMap;

    Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;

    struct viewProjectionBuffer
    {
        Matrix vMat;
        Matrix pMat;
        bool hasBounces;
    };
    Microsoft::WRL::ComPtr<ID3D11Buffer> particlesViewProjMat;

};

