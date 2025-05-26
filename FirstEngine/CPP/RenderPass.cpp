#include "RenderPass.h"
#include "GameObject.h"

RenderPass::RenderPass(ID3D11Device* device, ID3D11DeviceContext* context)
	: device(device), context(context)
{
}

ID3D11Device* RenderPass::GetDevice()
{
	return device.Get();
}

ID3D11DeviceContext* RenderPass::GetDeviceContext()
{
	return context.Get();
}



