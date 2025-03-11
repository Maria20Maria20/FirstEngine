#include "PlanetSystem.h"

void PlanetSystem::Initialize(ID3D11Device* device, ID3DBlob* vertexBC, ID3D11VertexShader* vs, ID3D11PixelShader* ps,
 ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* depthStencilView, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Camera* camera)
{
    m_device = device;
    m_vertexBC = vertexBC;
    m_vs = vs;
    m_ps = ps;
    m_rtv = rtv;
    m_depthStencilView = depthStencilView;
    m_context = context;
    m_camera = camera;
}

void PlanetSystem::GenerateRandom(int totalPlanets) {
    std::random_device rd;
    std::mt19937 gen(rd());

    // Настройки генерации
    const int maxSatellitesPerPlanet = 3;
    const float satelliteOrbitReduction = 0.5f;
    const float satelliteScaleReduction = 0.4f;

    // Распределения
    std::uniform_int_distribution<int> typeDist(0, 1);
    std::uniform_real_distribution<float> posDist(-1.0f, 1.0f);
    std::uniform_real_distribution<float> scaleDist(0.5f, 5.0f);
    std::uniform_real_distribution<float> speedDist(-1.5f, 1.5f);
    std::uniform_real_distribution<float> orbitDist(5.0f, 30.0f);
    std::uniform_real_distribution<float> axisDist(-1.0f, 1.0f);

    // Генерация корневых планет
    int generated = 0;
    while (generated < totalPlanets) {
        // Создаем корневую планету
        PlanetParams mainParams;
        mainParams.type = static_cast<GameObject::ObjectType>(typeDist(gen));
        mainParams.position = XMFLOAT3(posDist(gen), posDist(gen), posDist(gen));
        mainParams.scale = scaleDist(gen);
        mainParams.speed = speedDist(gen);
        mainParams.orbitRadius = 0.0f; // Корневые планеты не имеют орбиты
        mainParams.rotationAxis = XMVectorSet(axisDist(gen), axisDist(gen), axisDist(gen), 0.0f);

        CreatePlanet(mainParams);
        generated++;

        // Генерация спутников
        std::uniform_int_distribution<int> satellitesDist(0, maxSatellitesPerPlanet);
        int satellites = satellitesDist(gen);

        for (int i = 0; i < satellites && generated < totalPlanets; ++i, ++generated) {
            PlanetParams satelliteParams;
            satelliteParams.type = static_cast<GameObject::ObjectType>(typeDist(gen));
            satelliteParams.position = mainParams.position; // Стартовая позиция совпадает с родителем
            satelliteParams.scale = mainParams.scale * satelliteScaleReduction;
            satelliteParams.speed = speedDist(gen) * 2.0f; // Спутники движутся быстрее
            satelliteParams.orbitRadius = orbitDist(gen) * satelliteOrbitReduction;
            satelliteParams.rotationAxis = XMVectorSet(axisDist(gen), axisDist(gen), axisDist(gen), 0.0f);
            satelliteParams.parent = m_allPlanets.back().get(); // Последняя созданная планета - родитель

            CreatePlanet(satelliteParams);
        }
    }
}

void PlanetSystem::CreatePlanet(const PlanetParams& params) {
    m_allPlanets.push_back(std::make_unique<Planet>(
        m_device,
        m_vertexBC,
        m_vs,
        m_ps,
        m_rtv,
        m_depthStencilView,
        m_context,
        params.position,
        m_camera,
        params.rotationAxis,
        params.speed,
        params.type,
        params.scale,
        params.parent,
        params.orbitRadius
    ));
}

void PlanetSystem::Update(float deltaTime) {
    // Сначала обновляем все корневые планеты
    for (auto& planet : m_allPlanets) {
        if (planet->parentPlanet == nullptr) {
            planet->Update(deltaTime);
        }
    }

    // Затем обновляем спутники
    for (auto& planet : m_allPlanets) {
        if (planet->parentPlanet != nullptr) {
            planet->Update(deltaTime);
        }
    }
}

void PlanetSystem::Draw(ID3D11DeviceContext* context, const XMMATRIX& projection) {
    for (auto& planet : m_allPlanets) {
        planet->Draw(context, projection);
    }
}