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
	friend class RTXResources;
public:
	MeshMaterial() = default;
	MeshMaterial(ResourceManager* pRM, aiMaterial* mat, std::string RootPath, VertexLayout& VLay);

	void Bind(Graphics* pGraphics);

	void UpdateColor(DirectX::XMFLOAT3 color);

	static std::shared_ptr<MeshMaterial> GetDefaultMaterial(ResourceManager* pRM, VertexLayout& VLay);

private:
	std::shared_ptr<PipelineStateObject> pPipelineStateObject;
	std::shared_ptr<RootSignature> pRootSignature;

	std::vector<std::shared_ptr<Resource>> Resources;
	std::shared_ptr<ConstantBuffer> pConstBuffer;
	DirectX::XMFLOAT3 color;
};