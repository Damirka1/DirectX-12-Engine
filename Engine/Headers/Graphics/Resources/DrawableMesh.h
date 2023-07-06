#pragma once
#include <memory>
#include <vector>
#include <filesystem>

#include "../../Input/Camera.h"
#include "VertexLayout.h"

class Graphics;
class ResourceManager;
struct aiMesh;
struct aiMaterial;
class VertexBuffer;
class IndexBuffer;
class MeshMaterial;


class DrawableMesh
{
	friend class RTXResources;
public:
	DrawableMesh(ResourceManager* pRM, aiMesh* m, aiMaterial* mat, std::filesystem::path path, float scale, DirectX::XMMATRIX* transfrom);

	void BindMaterial(Graphics* pGraphics);

	void Draw(Graphics* pGraphics);

private:
	DirectX::XMMATRIX* transfrom;

	VertexLayout Lay;
	std::shared_ptr<VertexBuffer> pVertexBuffer;
	std::shared_ptr<IndexBuffer> pIndexBuffer;

	std::shared_ptr<MeshMaterial> Material;
};