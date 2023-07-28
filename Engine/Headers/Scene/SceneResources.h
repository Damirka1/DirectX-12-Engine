#pragma once

#include "../Graphics/Passes/ColorRenderPass.h"
#include "../Graphics/Passes/TextureRenderPass.h"
#include "../Graphics/Passes/RTXPass.h"
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

		pRTXPass = new RTXPass(pGraphics);
	}

	~SceneResources()
	{
		delete pPhysx;
		delete pColorPass;
		delete pTexturePass;
		delete pRTXPass;
	}
	
public:
	Graphics* pGraphics;
	PhysxManager* pPhysx;

	ColorRenderPass* pColorPass;
	TextureRenderPass* pTexturePass;
	RTXPass* pRTXPass;
};