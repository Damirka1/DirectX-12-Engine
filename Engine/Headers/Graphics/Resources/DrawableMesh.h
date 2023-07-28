#pragma once
#include <memory>
#include <vector>
#include <filesystem>

#include "../../Input/Camera.h"
#include "VertexLayout.h"

#include "../../Scene/Transform.h"

class Graphics;
class SceneResources;
struct aiMesh;
struct aiMaterial;
class VertexBuffer;
class IndexBuffer;
class ConstantBuffer;
class MeshMaterial;


class DrawableMesh
{
	friend class RTXResources;
public:
	DrawableMesh(SceneResources* pSceneResources, aiMesh* m, aiMaterial* mat, std::filesystem::path path, float scale = 1.0f);

	void BindMaterial(Graphics* pGraphics);

	void Draw(Graphics* pGraphics);

	void UpdateColor(DirectX::XMFLOAT3 color);

	void Update(const Transform& parent, Camera* pCamera);

	DirectX::XMMATRIX& GetPosMatrix();

	std::string GetTag();

private:
	std::shared_ptr<VertexBuffer> pVertexBuffer;
	std::shared_ptr<IndexBuffer> pIndexBuffer;

	std::shared_ptr<ConstantBuffer> pConstBuffer;
	Transform T = { };
	DxTransform DxT = { };

	std::string Tag;

	std::shared_ptr<MeshMaterial> Material;
};