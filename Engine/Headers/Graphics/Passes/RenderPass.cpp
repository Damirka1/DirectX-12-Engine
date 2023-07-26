#include "RenderPass.h"

#include "../Resources/RootSignature.h"
#include "../Resources/PipeLineState.h"
#include "../Resources/DrawableMesh.h"

RenderPass::RenderPass(Graphics* pGraphics, ResourceManager* pRM)
	:
	Pass(pGraphics, pRM)
{
}

void RenderPass::Bind()
{
	pPipelineStateObject->Bind(pGraphics);
	pRootSignature->Bind(pGraphics);
}

void RenderPass::Execute()
{
	for (auto& mesh : Meshes)
	{
		mesh->BindMaterial(pGraphics);
		mesh->Draw(pGraphics);
	}
}

void RenderPass::AddMesh(DrawableMesh* mesh)
{
	Meshes.push_back(mesh);
}
