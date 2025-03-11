#include "GameObject.h"

GameObject::GameObject(Microsoft::WRL::ComPtr<ID3D11Device> device, ID3DBlob* vertexBC, ID3D11VertexShader* vs,
	ID3D11PixelShader* ps, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* depthStencilView,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, ObjectType objectType)
	: mRotationAngle(0.0f), vsBlob(vertexBC), mVertexShader(vs), mPixelShader(ps),
	renderTargetView(rtv), depthStencilView(depthStencilView), device(device),
	context(context), currentObject(objectType)
{
	InitializeBuffers();
	InitializeShaders();
}

void GameObject::InitializeBuffers()
{
	mWorldMatrix = mRotationMatrix * DirectX::XMMatrixTranslation(0, 0.3, 0.3);
	if (currentObject == ObjectType::SPHERE)
	{
		CreateSphereVertexBuffer();
		CreateSphereIndexBuffer();
	}
	if (currentObject == ObjectType::CUBE)
	{
		CreateCubeVertexBuffer();
		CreateCubeIndexBuffer();
	}
	CreateConstantBuffer();
	CreateInputLayout();
}
void GameObject::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC constantBufferDesc = {};
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC; //0 = CPU don't need, D3D11_USAGE_DYNAMIC = CPU need
	constantBufferDesc.ByteWidth = sizeof(ConstantBuffer);
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = &cb;
	device->CreateBuffer(&constantBufferDesc, &initData, &mConstantBuffer);
}
void GameObject::CreateSphereVertexBuffer()
{
	//sliceCount = max(sliceCount, 4);
	//elevationCount = max(elevationCount, 1);

	verticesNum = 2 + (2 * elevationCount + 1) * sliceCount;
	vertices = (Vertex*)malloc(verticesNum * sizeof(Vertex));

	float sliceStep = DirectX::XM_2PI / sliceCount;
	float elevationStep = DirectX::XM_PIDIV2 / (elevationCount + 1);


	UINT _offsetVertexIdx = 0;
	// top vertex
	vertices[_offsetVertexIdx++] = { DirectX::XMFLOAT4(0.0f, radius, 0.0f, 1.0f),
		sphere_color_1 };
	// other vertices
	for (UINT i = 1; i <= 2 * elevationCount + 1; ++i)
	{
		for (UINT j = 0; j < sliceCount; ++j) {
			vertices[_offsetVertexIdx++] =
			{ DirectX::XMFLOAT4(
				radius * sinf(elevationStep * i) * cosf(sliceStep * j),
				radius * cosf(elevationStep * i),
				radius * sinf(elevationStep * i) * sinf(sliceStep * j),
				1.0f
			), (_offsetVertexIdx % 2 == 0 ? sphere_color_1 : sphere_color_2) };
		}
	}
	// bottom vertex
	vertices[_offsetVertexIdx++] = { DirectX::XMFLOAT4(0.0f, -radius, 0.0f, 1.0f), sphere_color_1 };

	D3D11_BUFFER_DESC vbd = {};
	vbd.ByteWidth = sizeof(Vertex) * verticesNum;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.Usage = D3D11_USAGE_DEFAULT; //how often does writing and reading to the buffer occur (default = read/write from GPU)
	vbd.CPUAccessFlags = 0; //0 = CPU don't need, D3D11_CPU_ACCESS_WRITE = CPU need
	vbd.MiscFlags = 0; //optional parameters
	vbd.StructureByteStride = 0; //size per element in buffer structure

	D3D11_SUBRESOURCE_DATA vinitData = {};
	//vinitData.pSysMem = vertices;
	vinitData.pSysMem = vertices;
	vinitData.SysMemPitch = 0;
	vinitData.SysMemSlicePitch = 0;

	device->CreateBuffer(&vbd, &vinitData, &mVertexBuffer);
}
void GameObject::CreateSphereIndexBuffer()
{
	indicesNum = 6 * sliceCount + 2 * 6 * elevationCount * sliceCount;
	//std::cout << *indicesNum << " << \n";
	indices = (UINT*)malloc(indicesNum * sizeof(int));

	size_t indexIndex = 0;

	for (UINT j = 0; j < sliceCount - 1; ++j) {
		indices[indexIndex++] = 0;
		indices[indexIndex++] = j + 2;
		indices[indexIndex++] = j + 1;
	}

	indices[indexIndex++] = 0;
	indices[indexIndex++] = 1;
	indices[indexIndex++] = sliceCount;

	for (UINT i = 0; i < 2 * elevationCount; ++i) {
		UINT startIndex = 1 + i * sliceCount;
		UINT nextStartIndex = startIndex + sliceCount;
		for (UINT j = 0; j < sliceCount - 1; ++j) {

			indices[indexIndex++] = startIndex + j;
			indices[indexIndex++] = startIndex + j + 1;
			indices[indexIndex++] = nextStartIndex + j;

			indices[indexIndex++] = startIndex + j + 1;
			indices[indexIndex++] = nextStartIndex + j + 1;
			indices[indexIndex++] = nextStartIndex + j;
		}

		indices[indexIndex++] = startIndex + sliceCount - 1;
		indices[indexIndex++] = startIndex;
		indices[indexIndex++] = nextStartIndex + sliceCount - 1;

		indices[indexIndex++] = startIndex;
		indices[indexIndex++] = nextStartIndex;
		indices[indexIndex++] = nextStartIndex + sliceCount - 1;
	}

	UINT bottomIndex = 2 + (2 * elevationCount + 1) * sliceCount - 1;
	UINT startIndex = 1 + 2 * elevationCount * sliceCount;
	for (UINT j = 0; j < sliceCount - 1; ++j) {
		indices[indexIndex++] = bottomIndex;
		indices[indexIndex++] = startIndex + j;
		indices[indexIndex++] = startIndex + j + 1;
	}

	indices[indexIndex++] = bottomIndex;
	indices[indexIndex++] = startIndex + sliceCount - 1;
	indices[indexIndex++] = startIndex;

	D3D11_BUFFER_DESC ibd = {};
	ibd.ByteWidth = sizeof(UINT) * indicesNum;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT; //how often does writing and reading to the buffer occur (default = read/write from GPU)
	ibd.CPUAccessFlags = 0; //0 = CPU don't need, 1 = CPU need
	ibd.MiscFlags = 0; //optional parameters
	ibd.StructureByteStride = 0; //size per element in buffer structure

	D3D11_SUBRESOURCE_DATA iinitData = {};
	iinitData.pSysMem = indices;
	iinitData.SysMemPitch = 0;
	iinitData.SysMemSlicePitch = 0;

	device->CreateBuffer(&ibd, &iinitData, &mIndexBuffer);
}
void GameObject::CreateCubeVertexBuffer()
{
	vertices = (Vertex*) malloc(8 * sizeof(Vertex));
	Vertex _vertices[8] = {
	XMFLOAT4(-0.1f, -0.1f, -0.1f, 1.0f), XMFLOAT4(+1.0f, 0.0f, 0.0f, 1.0f),
	XMFLOAT4(-0.1f, +0.1f, -0.1f, 1.0f), XMFLOAT4(0.0f, +1.0f, 0.0f, 1.0f),
	XMFLOAT4(+0.1f, +0.1f, -0.1f, 1.0f), XMFLOAT4(0.0f, 0.0f, +1.0f, 1.0f),
	XMFLOAT4(+0.1f, -0.1f, -0.1f, 1.0f), XMFLOAT4(+0.0f, +1.0f, +1.0f, 1.0f),
	XMFLOAT4(-0.1f, -0.1f, 0.1f, 1.0f), XMFLOAT4(+1.0f, +0.0f, +1.0f, 1.0f),
	XMFLOAT4(-0.1f, +0.1f, 0.1f, 1.0f), XMFLOAT4(+1.0f, +1.0f, +0.0f, 1.0f),
	XMFLOAT4(+0.1f, +0.1f, 0.1f, 1.0f), XMFLOAT4(+0.0f, +0.0f, +0.0f, 1.0f),
	XMFLOAT4(+0.1f, -0.1f, 0.1f, 1.0f), XMFLOAT4(+0.0f, +0.0f, +0.0f, 1.0f),
	};
	for (size_t i = 0; i < 8; i++) //8 vertecies (vertex = position + color)
	{
		vertices[i] = _vertices[i];
	}

	D3D11_BUFFER_DESC vbd = {};
	vbd.ByteWidth = sizeof(Vertex) * 8;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.Usage = D3D11_USAGE_DEFAULT; //how often does writing and reading to the buffer occur (default = read/write from GPU)
	vbd.CPUAccessFlags = 0; //0 = CPU don't need, D3D11_CPU_ACCESS_WRITE = CPU need
	vbd.MiscFlags = 0; //optional parameters
	vbd.StructureByteStride = 0; //size per element in buffer structure

	D3D11_SUBRESOURCE_DATA vinitData = {};
	//vinitData.pSysMem = vertices;
	vinitData.pSysMem = vertices;
	vinitData.SysMemPitch = 0;
	vinitData.SysMemSlicePitch = 0;

	device->CreateBuffer(&vbd, &vinitData, &mVertexBuffer);
}

void GameObject::CreateCubeIndexBuffer()
{
	indicesNum = 36;
	indices = (UINT*)malloc(indicesNum * sizeof(int));
	UINT _indices[] = {
		// Front Face
		0, 1, 2,
		0, 2, 3,

		// Back Face
		4, 6, 5,
		4, 7, 6,

		// Left Face
		4, 5, 1,
		4, 1, 0,

		// Right Face
		3, 2, 6,
		3, 6, 7,

		// Top Face
		1, 5, 6,
		1, 6, 2,

		// Bottom Face
		4, 0, 3,
		4, 3, 7
	};

	for (size_t i = 0; i < 36; i++)
	{
		indices[i] = _indices[i];
	}

	D3D11_BUFFER_DESC ibd = {};
	ibd.ByteWidth = sizeof(UINT) * std::size(_indices);
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT; //how often does writing and reading to the buffer occur (default = read/write from GPU)
	ibd.CPUAccessFlags = 0; //0 = CPU don't need, 1 = CPU need
	ibd.MiscFlags = 0; //optional parameters
	ibd.StructureByteStride = 0; //size per element in buffer structure

	D3D11_SUBRESOURCE_DATA iinitData = {};
	iinitData.pSysMem = indices;
	iinitData.SysMemPitch = 0;
	iinitData.SysMemSlicePitch = 0;

	device->CreateBuffer(&ibd, &iinitData, &mIndexBuffer);
}

void GameObject::Update(float dt)
{

	//mWorldMatrix = mRotationMatrix * DirectX::XMMatrixTranslation(0, -0.3, 0.3);

	//// Обновление константного буфера
	//cb.worldViewProj = mWorldMatrix; // *viewProj;
}

void GameObject::Draw(ID3D11DeviceContext* context, const DirectX::XMMATRIX& viewProj)
{
	SetupIAStage();


	D3D11_MAPPED_SUBRESOURCE mappedResource;
	context->Map(mConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &cb, sizeof(ConstantBuffer));
	context->Unmap(mConstantBuffer, 0);

	context->VSSetConstantBuffers(0u, 1u, &mConstantBuffer);

	// Установка шейдеров и ресурсов
	context->VSSetShader(mVertexShader, nullptr, 0);
	context->PSSetShader(mPixelShader, nullptr, 0);

	context->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
	// Отрисовка
	context->DrawIndexed(indicesNum, 0, 0);
}

void GameObject::RotateShape(DirectX::XMVECTOR Axis, FLOAT Angle, float deltaTime)
{
	//DirectX::XMFLOAT4 f4_axis;
	//DirectX::XMStoreFloat4(&f4_axis, Axis);
	//f4_axis = DirectX::XMFLOAT4(f4_axis.x / f4_axis.w, f4_axis.y / f4_axis.w, f4_axis.z / f4_axis.w, 1.0f);
	//Axis = DirectX::XMLoadFloat4(&f4_axis);
	
	DirectX::XMVECTOR normalizedAxis = DirectX::XMVector3Normalize(Axis); // Используем новую переменную

	// Проверяем, что вектор не нулевой
	if (DirectX::XMVector3Equal(normalizedAxis, DirectX::XMVectorZero()))
	{
		return; // Если ось некорректна, не выполняем вращение
	}
	DirectX::XMMATRIX rotateMatrix = DirectX::XMMatrixRotationAxis(normalizedAxis, DirectX::XMConvertToRadians(Angle * deltaTime));
	mRotationMatrix *= rotateMatrix;
}

void GameObject::ScalingShape(float scaleFactor)
{
	DirectX::XMMATRIX scalingMatrix = DirectX::XMMatrixScaling(scaleFactor, scaleFactor, scaleFactor);
	cb.worldViewProj *= scalingMatrix;
}





void GameObject::InitializeShaders()
{
	auto res = D3DCompileFromFile(L"./Shaders/CubeShader.hlsl", //create vertex shader from  hlsl file
		nullptr /*macros*/,
		nullptr /*include*/,
		"VSMain", //function name from hlsl file for run it
		"vs_5_0", //shader target - vertex shader (vs)
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, //options for compile shader
		0, //parameters for compile effects (if 0, then don't compile them)
		&vsBlob,
		&errorVertexCode);

	if (FAILED(res)) {
		std::cout << "failed res load!\n";
	}

	D3D_SHADER_MACRO Shader_Macros[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr };
	//"TEST" - use TEST define from hlsl file
	//float4(0.0f, 1.0f, 0.0f, 1.0f) - color for square right

	res = D3DCompileFromFile(L"./Shaders/CubeShader.hlsl", //create pixel shaders from hlsl file
		Shader_Macros /*macros*/, //macros shaders
		nullptr /*include*/,
		"PSMain", //function name from hlsl file for run it
		"ps_5_0", //shader target - pixel shader (ps)
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, //options for compile shader
		0, //parameters for compile effects (if 0, then don't compile them)
		&psBlob,
		&errorPixelCode);


	if (FAILED(res)) {
		std::cout << "failed res load!\n";
	}

	device->CreateVertexShader(
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		nullptr, &mVertexShader);

	device->CreatePixelShader(
		psBlob->GetBufferPointer(),
		psBlob->GetBufferSize(),
		nullptr, &mPixelShader);
}

void GameObject::CreateInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
D3D11_INPUT_ELEMENT_DESC{
	"POSITION", //parameter name from hlsl file
	0, //need if we have more one element with same semantic
	DXGI_FORMAT_R32G32B32A32_FLOAT, //parameter for create 3D object
	0, //vertex index (between 0 and 15)
	0, //translation from beginning vertex
	D3D11_INPUT_PER_VERTEX_DATA, //class input data for input slot (for each vertex or instance)
	0 },
	D3D11_INPUT_ELEMENT_DESC{
	"COLOR",
	0,
	DXGI_FORMAT_R32G32B32A32_FLOAT,
	0,
	D3D11_APPEND_ALIGNED_ELEMENT,
	D3D11_INPUT_PER_VERTEX_DATA,
	0 }
	};

	device->CreateInputLayout(
		inputElements,
		2,
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		&mInputLayout);
}

void GameObject::SetupIAStage()
{
	context->IASetInputLayout(mInputLayout);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// Установка вершинного буфера
	UINT stride[] = { sizeof(Vertex) };
	UINT offset = 0;
	context->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetVertexBuffers(0, 1, &mVertexBuffer, stride, &offset);
}
