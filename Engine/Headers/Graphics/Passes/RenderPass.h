#pragma once

#include "Pass.h"
#include <vector>
#include <memory>

class RootSignature;
class PipelineStateObject;

class RenderPass : public Pass
{
public:
	RenderPass(Graphics* pGraphics, ResourceManager* pRM);

	void Bind() override;

	void Execute() override;

	void AddMesh(DrawableMesh* mesh) override;

private:
	std::shared_ptr<RootSignature> pRootSignature;
	std::shared_ptr<PipelineStateObject> pPipelineStateObject;

	std::vector<DrawableMesh*> Meshes;

};