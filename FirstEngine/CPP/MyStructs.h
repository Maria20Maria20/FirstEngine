#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <memory>
#include <d3dcompiler.h>
#include <iostream>
#include <wrl.h>
#include <chrono>
#include <SimpleMath.h>
struct Vertex
{
    DirectX::XMFLOAT4 position;
    DirectX::XMFLOAT4 color;
    DirectX::XMFLOAT2 texCoord;
    DirectX::XMFLOAT3 normal;
};
struct ConstantBuffer
{
    DirectX::XMMATRIX worldViewProj = DirectX::XMMatrixIdentity();
    DirectX::XMMATRIX worldMat = DirectX::XMMatrixIdentity();
    DirectX::XMMATRIX worldMatInvTranspose = DirectX::XMMatrixIdentity();
    DirectX::XMFLOAT3 cameraPosition;
};
struct DirectionalLight
{
    XMFLOAT4 Ambient;
    XMFLOAT4 Diffuse;
    XMFLOAT4 Specular;
    XMFLOAT3 Direction;
    float Pad;
};

struct PointLight
{
    DirectX::XMFLOAT4 Ambient;
    DirectX::XMFLOAT4 Diffuse;
    DirectX::XMFLOAT4 Specular;
    DirectX::XMFLOAT3 Position;
    float Range;

    DirectX::XMFLOAT3 Att;
    float pad;
};

struct LightData {
	DirectionalLight dLight;
	PointLight pointLights[10];
};
