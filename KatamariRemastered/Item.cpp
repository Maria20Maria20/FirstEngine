#include "Item.h"

Item::Item(ID3D11Device* device,
    float radius, float changedScale)
{
    this->device = device;
    this->scale = changedScale;
    this->camera = camera;
    this->radius = radius;
    // for loaded model
    appliedScale = radius / modelRadius;
    
    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_real_distribution<> distr(0, 1);

    int spawnDist = 100;
    float z = (distr(gen) * spawnDist) - spawnDist / 2;
    float x = (distr(gen) * spawnDist) - spawnDist / 2;

    initialPosition = {x, radius, z};
    initRandomRotation = GetRandomRotateTransform();

    // load model
    LoadRandomModel("..\\Models\\");

    {
        // Topology
        topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

        CreateVertexBuffer();
        CreateIndexBuffer();

        // VertexShader
        auto res = D3DCompileFromFile(L"./Shaders/ItemGBufferShaderVS.hlsl", //create vertex shader from  hlsl file
            nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
            "VSMain", "vs_5_0",
            D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
            &vsBlob, &errorVertexCode);
        if (FAILED(res)) {
            // If the shader failed to compile it should have written something to the error message.
            if (errorVertexCode) {
                char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());

                std::cout << compileErrors << std::endl;
            }
            // If there was  nothing in the error message then it simply could not find the shader file itself.
            else
            {
                std::cout << L"./Shaders/ItemGBufferShaderVS.hlsl" << L" - Missing Shader File\n";
            }
        }

        device->CreateVertexShader(
            vsBlob->GetBufferPointer(),
            vsBlob->GetBufferSize(),
            nullptr, &pVertexShader);

        // InputLayout
        this->numInputElements = 4;
        this->IALayoutInputElements = (D3D11_INPUT_ELEMENT_DESC*)malloc(this->numInputElements * sizeof(D3D11_INPUT_ELEMENT_DESC));
        this->IALayoutInputElements[0] =
            D3D11_INPUT_ELEMENT_DESC{
                "POSITION",
                0,
                DXGI_FORMAT_R32G32B32A32_FLOAT,
                0,
                0,
                D3D11_INPUT_PER_VERTEX_DATA,
                0 };

        this->IALayoutInputElements[1] =
            D3D11_INPUT_ELEMENT_DESC{
                "COLOR",
                0,
                DXGI_FORMAT_R32G32B32A32_FLOAT,
                0,
                D3D11_APPEND_ALIGNED_ELEMENT,
                D3D11_INPUT_PER_VERTEX_DATA,
                0 };

        this->IALayoutInputElements[2] =
            D3D11_INPUT_ELEMENT_DESC{
                "TEXCOORD",
                0,
                DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT,
                0,
                D3D11_APPEND_ALIGNED_ELEMENT,
                D3D11_INPUT_PER_VERTEX_DATA,
                0 };
        this->IALayoutInputElements[3] =
            D3D11_INPUT_ELEMENT_DESC{
                "NORMAL",
                0,
                DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
                0,
                D3D11_APPEND_ALIGNED_ELEMENT,
                D3D11_INPUT_PER_VERTEX_DATA,
                0 };

        device->CreateInputLayout(
            this->IALayoutInputElements,
            this->numInputElements,
            vsBlob->GetBufferPointer(),
            vsBlob->GetBufferSize(),
            &pInputLayout);

        // TransformCBuffer
        D3D11_BUFFER_DESC cbd;
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.Usage = D3D11_USAGE_DYNAMIC;
        cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        cbd.MiscFlags = 0u;
        cbd.ByteWidth = sizeof(Transforms) + (16 - (sizeof(Transforms) % 16));  // aligned size
        cbd.StructureByteStride = 0u;

        device->CreateBuffer(&cbd, nullptr, &pTransformConstantBuffer);


        // PixelShader
        res = D3DCompileFromFile(L"./Shaders/ItemGBufferShaderPS.hlsl",
            nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
            "PSMain", "ps_5_0",
            D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
            &psBlob, &errorPixelCode);
        if (FAILED(res)) {
            // If the shader failed to compile it should have written something to the error message.
            if (errorPixelCode) {
                char* compileErrors = (char*)(errorPixelCode->GetBufferPointer());
                std::cout << compileErrors << " - // -- " << std::endl;
            }
            // If there was  nothing in the error message then it simply could not find the shader file itself.
            else
            {
                std::cout << L"./Shaders/ItemGBufferShaderPS.hlsl" << L" - Missing Shader File\n";
            }

            return;
        }
        device->CreatePixelShader(
            psBlob->GetBufferPointer(),
            psBlob->GetBufferSize(),
            nullptr, &pPixelShader);

        // Texture
        std::string texture_name = "..\\Textures\\Flowers_Diffuse.dds";
        pTexture = new Texture(device, texture_name, aiTextureType_DIFFUSE);

        // Texture sampler
        D3D11_SAMPLER_DESC samplerDesc;
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.MipLODBias = 0.0f;
        samplerDesc.MaxAnisotropy = 1;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        samplerDesc.BorderColor[0] = 0;
        samplerDesc.BorderColor[1] = 0;
        samplerDesc.BorderColor[2] = 0;
        samplerDesc.BorderColor[3] = 0;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
        device->CreateSamplerState(&samplerDesc, &pSampler);


        // PixelConstantBuffer
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.Usage = D3D11_USAGE_DYNAMIC;
        cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        cbd.MiscFlags = 0u;
        cbd.ByteWidth = sizeof(ItemPixelConstantBuffer) + (16 - (sizeof(ItemPixelConstantBuffer) % 16));  // aligned size
        cbd.StructureByteStride = 0u;

        device->CreateBuffer(&cbd, nullptr, &pItemPixelConstantBuffer);

        // Rasterizer
        D3D11_RASTERIZER_DESC rastDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
        rastDesc.CullMode = D3D11_CULL_BACK;
        rastDesc.FillMode = D3D11_FILL_SOLID;
        device->CreateRasterizerState(&rastDesc, &pRasterizer);


        // BlendState
        D3D11_BLEND_DESC blendDesc = CD3D11_BLEND_DESC(CD3D11_DEFAULT{});
        blendDesc.RenderTarget[0].BlendEnable = TRUE;
        blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        blendDesc.RenderTarget[1].BlendEnable = TRUE;
        blendDesc.RenderTarget[1].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[1].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[1].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[1].SrcBlendAlpha = D3D11_BLEND_ZERO;
        blendDesc.RenderTarget[1].DestBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[1].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[1].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        blendDesc.RenderTarget[2].BlendEnable = TRUE;
        blendDesc.RenderTarget[2].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[2].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[2].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[2].SrcBlendAlpha = D3D11_BLEND_ZERO;
        blendDesc.RenderTarget[2].DestBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[2].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[2].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        blendDesc.RenderTarget[3].BlendEnable = TRUE;
        blendDesc.RenderTarget[3].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[3].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[3].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[3].SrcBlendAlpha = D3D11_BLEND_ZERO;
        blendDesc.RenderTarget[3].DestBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[3].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[3].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        device->CreateBlendState(&blendDesc, &pBlendState);

        // DepthStencilState
        D3D11_DEPTH_STENCIL_DESC depthStencilDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT{});
        device->CreateDepthStencilState(&depthStencilDesc, &pDepthStencilState);
    }
 
     //this->textures.push_back(Texture(device, "models\\Textures\\basketballskin.dds", aiTextureType_DIFFUSE));
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
        mWorldMatrix = Matrix::CreateScale(appliedScale) *
            Matrix::CreateTranslation(initialPosition);
    }
}



void Item::LoadRandomModel(const std::string& folder)
{
    // разобраться здесь с model_name и chosen_model
    static std::random_device rd;
    static std::mt19937 gen(rd());

    auto models = GetModelList(folder);
    if (models.empty()) return;

    std::uniform_int_distribution<> distr(0, models.size() - 1);
    auto chosen_model = models[distr(gen)];
    LoadModel(chosen_model, this, LoadModel::VertexAttrFlags::POSITION | LoadModel::VertexAttrFlags::TEXTURE
        | LoadModel::VertexAttrFlags::NORMAL);

    float maxDistance = 0.0f;
    for (int i = 0; i < this->verticesNum; i++)
    {
        XMFLOAT4 pos = this->vertices[i].position;
        float dist = sqrt(pos.x * pos.x + pos.y * pos.y + pos.z * pos.z);
        maxDistance = max(maxDistance, dist);
    }
    modelRadius = maxDistance;

    std::cout << chosen_model << " :: " << verticesNum << " :: " << StringHelper::GetFileNameWithoutExtension(chosen_model) << "\n";
}

std::vector<std::string> Item::GetModelList(const std::string& modelsFolder)
{
    std::vector<std::string> models;
    for (const auto& entry : std::filesystem::directory_iterator(modelsFolder))
    {
        if (entry.is_regular_file() &&
            (entry.path().extension() == ".fbx" ||
                entry.path().extension() == ".obj"))
        {
            models.push_back(entry.path().string());
        }
    }
    return models;
}



void Item::applyGBufferPass(ID3D11DeviceContext* context)
{
    context->IASetPrimitiveTopology(topology);
    UINT stride = sizeof(Vertex);
    const UINT offset = 0u;
    context->IASetVertexBuffers(0u, 1u, &mVertexBuffer, &stride, &offset);
    context->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    context->VSSetShader(pVertexShader.Get(), nullptr, 0u);
    context->IASetInputLayout(pInputLayout.Get());

    DirectX::XMMATRIX A = mWorldMatrix;
    A.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
    DirectX::XMVECTOR det = XMMatrixDeterminant(A);
    const auto wMatInvTranspose = DirectX::XMMatrixTranspose(XMMatrixInverse(&det, A));
    const Matrix vpMat = Matrix(camera->GetViewMatrix()) * Matrix(camera->GetProjectionMatrix());
    const Transforms tf = {
            mWorldMatrix, wMatInvTranspose,
            vpMat
    };
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    context->Map(pTransformConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource);
    memcpy(mappedResource.pData, &tf, sizeof(tf) + (16 - (sizeof(tf) % 16))); // aligned size
    context->Unmap(pTransformConstantBuffer.Get(), 0);
    context->VSSetConstantBuffers(0u, 1u, pTransformConstantBuffer.GetAddressOf());

    const ItemPixelConstantBuffer ipcb = { camera->GetPosition() };
    context->Map(pItemPixelConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource);
    memcpy(mappedResource.pData, &ipcb, sizeof(ipcb) + (16 - (sizeof(ipcb) % 16))); // aligned size
    context->Unmap(pItemPixelConstantBuffer.Get(), 0);
    context->VSSetConstantBuffers(1u, 1u, pItemPixelConstantBuffer.GetAddressOf());

    context->PSSetShader(pPixelShader.Get(), nullptr, 0u);
    context->PSSetShaderResources(0u, 1, pTexture->GetTextureResourceViewAddress());
    context->PSSetSamplers(0u, 1u, pSampler.GetAddressOf());
    context->RSSetState(pRasterizer.Get());

    FLOAT blendFactor[4] = { 1, 1, 1, 1 };
    UINT sampleMask = 0xffffffff;
    context->OMSetBlendState(pBlendState.Get(), blendFactor, sampleMask);

    context->OMSetDepthStencilState(pDepthStencilState.Get(), 1u);

    context->DrawIndexed(indicesNum, 0, 0);
}

void Item::applyLightPass(ID3D11DeviceContext* context)
{

}
