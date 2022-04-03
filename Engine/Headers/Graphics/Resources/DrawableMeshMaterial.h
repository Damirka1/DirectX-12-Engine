#pragma once
#include <memory>
#include <vector>

#include "Resource.h"
#include <DirectXMath.h>
#include "Buffers/Buffers.h"

class PipelineStateObject;
class RootSignature;
class Graphics;
struct aiMaterial;
class ResourceManager;
class PSO_Layout;
class RS_Layout;
class VertexLayout;
class Camera;

class MeshMaterial
{
public:
	MeshMaterial() = default;
	MeshMaterial(ResourceManager* pRM, aiMaterial* mat);

	void Bind(Graphics* pGraphics);

	static std::shared_ptr<MeshMaterial> GetDefaultMaterial(ResourceManager* pRM, VertexLayout& VLay);

private:
	std::shared_ptr<PipelineStateObject> pPipelineStateObject;
	std::shared_ptr<RootSignature> pRootSignature;

	std::vector<std::shared_ptr<Resource>> Resources;
	
};