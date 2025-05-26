#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <string>

#include "../Scene.h"


class RenderPass
{
public:
	RenderPass(ID3D11Device* device, ID3D11DeviceContext* context);
	virtual void Pass(const Scene& scene) = 0;
	virtual void StartFrame() = 0;
	virtual void EndFrame() = 0;

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
protected:

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
};

