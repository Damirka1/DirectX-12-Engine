#pragma once

#include "../Graphics/Passes/ColorRenderPass.h"
#include "../Graphics/Passes/TextureRenderPass.h"
#include "../PhysX/PhysxManager.h"

class Graphics;

class SceneResources
{
public:
	SceneResources(Graphics* pGraphics)
		:
		pGraphics(pGraphics)
	{
		pPhysx = new PhysxManager();

		// Create passes
		pColorPass = new ColorRenderPass(pGraphics);
		pTexturePass = new TextureRenderPass(pGraphics);
	}

	~SceneResources()
	{
		delete pPhysx;
		delete pColorPass;
		delete pTexturePass;
	}
	
public:
	Graphics* pGraphics;
	PhysxManager* pPhysx;

	ColorRenderPass* pColorPass;
	TextureRenderPass* pTexturePass;
};