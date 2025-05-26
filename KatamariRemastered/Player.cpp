#include "Player.h"


Player::Player(ID3D11Device* device, XMFLOAT3 startPosition,
	float changedScale)
{
	this->device = device;
	this->scale = changedScale;
	initRandomRotation = Matrix::CreateFromYawPitchRoll(XM_PIDIV2, 0, 0);// GetRandomRotateTransform();
	position = startPosition;
	position.y = radius;

	{
        // Topology
        topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

        CreateSphereVertexBuffer();
        CreateSphereIndexBuffer();

        // VertexShader
        auto res = D3DCompileFromFile(L"./Shaders/PlayerGBufferShaderVS.hlsl", //create vertex shader from  hlsl file
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
                std::cout << L"./Shaders/PlayerGBufferShaderVS.hlsl" << L" - Missing Shader File\n";
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
        res = D3DCompileFromFile(L"./Shaders/PlayerGBufferShaderPS.hlsl",
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
                std::cout << L"./Shaders/PlayerGBufferShaderPS.hlsl" << L" - Missing Shader File\n";
            }

            return;
        }
        device->CreatePixelShader(
            psBlob->GetBufferPointer(),
            psBlob->GetBufferSize(),
            nullptr, &pPixelShader);

        // Texture
        std::string texture_name = "./Kevin.dds";
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
        cbd.ByteWidth = sizeof(PlayerPixelConstantBuffer) + (16 - (sizeof(PlayerPixelConstantBuffer) % 16));  // aligned size
        cbd.StructureByteStride = 0u;

        device->CreateBuffer(&cbd, nullptr, &pPlayerPixelConstantBuffer);

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
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        res = device->CreateBlendState(&blendDesc, pBlendState.GetAddressOf());
        if (FAILED(res)) {
            {
                std::cout << " CreateBlendState - FAIL!\n";
            }

            return;
        }

        // DepthStencilState
        D3D11_DEPTH_STENCIL_DESC depthStencilDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT{});
        device->CreateDepthStencilState(&depthStencilDesc, &pDepthStencilState);
    }
}

void Player::Update(float deltaTime)
{
	Move(deltaTime);
	//RotateShape(rotationDirection, speedRotation, deltaTime);
	Matrix spinTransform = Matrix::CreateFromYawPitchRoll(0.0f, currentSpin, 0.0f);
	Matrix rotTransform = Matrix::CreateFromYawPitchRoll(currentRotation, 0.0f, 0.0f);

	mWorldMatrix = 
		Matrix::CreateScale(radius)
		* initRandomRotation
		* spinTransform * rotTransform
		* Matrix::CreateTranslation(position);
}

// For lighting
Matrix Player::GetRelativeTransform() {
	return Matrix::CreateFromYawPitchRoll(currentRotation, currentSpin, 0.0f) * Matrix::CreateTranslation(position);
}

void Player::Move(float deltaTime)
{
	SlowDown(deltaTime);
	position.y = radius;
	position = position + deltaTime * velocity * GetMoveDir() * faceDirection;
	currentSpin += spinSpeed * deltaTime;
}

Vector3 Player::GetCenterLocation()
{
	return position;
}

Vector3 Player::GetMoveDir()
{
	return Vector3::Transform(Vector3(0.0f, 0.0f, faceDirection), Matrix::CreateFromYawPitchRoll(currentRotation, 0.0f, 0.0f));
}

void Player::AddTurn(float direction, float deltaTime)
{
	currentRotation += direction * deltaTime * rotationSpeed;
	//currentRotation = max(-XM_PI, min(currentRotation, XM_PI));
	if (currentRotation > XM_PI) {
		currentRotation -= XM_2PI;
	}
	else if (currentRotation < -XM_PI) {
		currentRotation += XM_2PI;
	}
}

void Player::PushForward(float deltaTime)
{
	velocity = min(maxVelocity, velocity + deltaTime * 2 * acceleration);
	spinSpeed = velocity / radius;
	faceDirection = 1;
}
void Player::PushBackward(float deltaTime)
{
	velocity = min(maxVelocity, velocity + deltaTime * 2 * acceleration);
	spinSpeed = velocity / radius;
	faceDirection = -1;
}

void Player::SlowDown(float deltaTime)
{
	velocity = max(0.0, velocity - deltaTime * acceleration);
	spinSpeed = velocity / radius;
}


void Player::applyGBufferPass(ID3D11DeviceContext* context)
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

    const PlayerPixelConstantBuffer gpcb = { camera->GetPosition() };
    context->Map(pPlayerPixelConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource);
    memcpy(mappedResource.pData, &gpcb, sizeof(gpcb) + (16 - (sizeof(gpcb) % 16))); // aligned size
    context->Unmap(pPlayerPixelConstantBuffer.Get(), 0);
    context->VSSetConstantBuffers(1u, 1u, pPlayerPixelConstantBuffer.GetAddressOf());

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

void Player::applyLightPass(ID3D11DeviceContext* context)
{

}
