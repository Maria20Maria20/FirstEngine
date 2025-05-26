#pragma once

#include <d3d11.h>
#include <wrl.h>    
#include <directxmath.h>

#include "CPP/RenderingSystem.h"
#include "CPP/GBuffer.h"
#include "CPP/DisplayWin32.h"
#include "Scene.h"

class DeferredRenderer :
    public RenderingSystem
{
public:
    DeferredRenderer();
    DeferredRenderer(DisplayWin32* displayWin);

    void RenderScene(const Scene& scene) override;
    void AddPass(RenderPass* pass) override;

    
    std::vector<RenderPass*> passes;
};

