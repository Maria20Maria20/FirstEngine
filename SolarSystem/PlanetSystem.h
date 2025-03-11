#pragma once
#include <vector>
#include <memory>
#include <random>
#include <d3d11.h>
#include <CPP/GameObject.h>
#include "Camera.h"
#include "Planet.h"


class PlanetSystem {
public:
    struct PlanetParams {
        GameObject::ObjectType type;
        XMFLOAT3 position;
        float scale;
        float speed;
        float orbitRadius;
        XMVECTOR rotationAxis;
        Planet* parent = nullptr;
    };

    void Initialize(ID3D11Device* device, ID3DBlob* vertexBC, ID3D11VertexShader* vs,
        ID3D11PixelShader* ps, ID3D11RenderTargetView* rtv,
        ID3D11DepthStencilView* depthStencilView,
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
        Camera* camera);

    void GenerateRandom(int totalPlanets);
    void Update(float deltaTime);
    void Draw(ID3D11DeviceContext* context, const XMMATRIX& projection);

private:
    std::vector<std::unique_ptr<Planet>> m_allPlanets;

    // Параметры устройств DirectX
    ID3D11Device* m_device;
    ID3DBlob* m_vertexBC;
    ID3D11VertexShader* m_vs;
    ID3D11PixelShader* m_ps;
    ID3D11RenderTargetView* m_rtv;
    ID3D11DepthStencilView* m_depthStencilView;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
    Camera* m_camera;

    PlanetParams GenerateRootPlanetParams(std::mt19937& gen);
    PlanetParams GenerateSatelliteParams(Planet* parent, std::mt19937& gen);
    void CreatePlanet(const PlanetParams& params);
};