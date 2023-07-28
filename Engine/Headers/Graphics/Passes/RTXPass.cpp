#include "RTXPass.h"

#include "../../ResourceManager.h"
#include "../RayTracing/RTXResources.h"

RTXPass::RTXPass(Graphics* pGraphics)
	:
	pGraphics(pGraphics)
{
	pRM = new ResourceManager(pGraphics);
	pRTX = new RTXResources(pGraphics, pRM);
}

RTXPass::~RTXPass()
{
	delete pRM;
	delete pRTX;
}

void RTXPass::Initialize(Camera* pCamera)
{
	pGraphics->SetupInit();

	pRM->InitializeResources();

	if (pRTX->IsNeedUpdate())
		pRTX->Initialize();
	
	pRTX->Update(pCamera);

	pGraphics->Initialize();

	if(pRTX->IsNeedRelease())
			pRTX->ReleaseScratch();
}

void RTXPass::AddMesh(DrawableMesh* mesh, unsigned int hitGroup)
{
	pRTX->PrepareMeshForRtx(mesh, hitGroup);
}

void RTXPass::Execute()
{
	pRTX->CopyBuffer();
	pRTX->DispatchRays();
}
