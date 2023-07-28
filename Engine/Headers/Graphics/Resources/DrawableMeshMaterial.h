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
class SceneResources;
class PSO_Layout;
class RS_Layout;
class VertexLayout;
class Camera;

class MeshMaterial
{
	friend class RTXResources;
public:

	enum MaterialType
	{
		Color,
		Texture,
	};

	MeshMaterial() = default;
	MeshMaterial(SceneResources* pSceneResources, aiMaterial* mat, std::string RootPath);

	MaterialType GetType();

	void Bind(Graphics* pGraphics);

	void UpdateColor(DirectX::XMFLOAT3 color);

	static std::shared_ptr<MeshMaterial> GetDefaultMaterial(SceneResources* pSceneResources);

private:
	std::shared_ptr<PipelineStateObject> pPipelineStateObject;
	std::shared_ptr<RootSignature> pRootSignature;

	std::vector<std::shared_ptr<Resource>> Resources;
	std::shared_ptr<ConstantBuffer> pConstBuffer;
	DirectX::XMFLOAT3 dxColor;

	MaterialType Type;
};