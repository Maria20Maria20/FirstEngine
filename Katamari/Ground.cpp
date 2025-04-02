#include "Ground.h"


Ground::Ground(ID3D11Device* device, ID3D11VertexShader* vs, ID3D11PixelShader* ps,
	ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* depthStencilView,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, XMFLOAT3 startPosition, Camera* camera,
	ObjectType objectType,
	float changedScale, LPCWSTR shaderFilePath
) //: GameObject(device,
	//vs, ps, rtv, depthStencilView, context, objectType, shaderFilePath)
{
	position = startPosition;
	this->changedScale = changedScale;
	this->camera = camera;
    mVertexShader = vs;
    mPixelShader = ps;
    renderTargetView = rtv;
    this->depthStencilView = depthStencilView;
    this->device = device;
    this->context = context;
    currentObject = objectType;
    this->shaderFilePath = L"./Shaders/CubeShader.hlsl";


    //InitializeShaders();
    //CreateRandomHeightPlane(50.0f, 50.0f, 4, 4, 0.05f,
    //    DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), &vertices, &verticesNum, &indices, &indicesNum);

    //CreateVertexBuffer();
    //CreateIndexBuffer();
    //mWorldMatrix = DirectX::XMMatrixIdentity();
    //CreateConstantBuffer();

    InitializeShaders();
    InitializeBuffers();

    {
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
            &mInputLayout);
    }

}

void Ground::Update(float deltaTime)
{
	//RotateShape(rotationDirection, speedRotation, deltaTime);
	mWorldMatrix = XMMatrixTranslation(position.x, position.y, position.z);


	Matrix viewMat = camera->GetViewMatrix();
	Matrix projMat = camera->GetProjectionMatrix();


	// Update constant buffer
	cb.worldViewProj = XMMatrixScaling(changedScale, changedScale, changedScale) * mWorldMatrix * (XMMATRIX)(viewMat * projMat);
    cb.cameraPosition = camera->GetPosition();
}
