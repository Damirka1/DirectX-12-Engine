#pragma once
#include <memory>
#include <vector>

#include "../../Input/Camera.h"

class Graphics;
class ResourceManager;
struct aiMesh;
struct aiMaterial;
class VertexBuffer;
class IndexBuffer;
class MeshMaterial;

class DrawableMesh
{
public:
	DrawableMesh(ResourceManager* pRM, aiMesh* m, aiMaterial* mat, float scale = 1.0f);

	void BindMaterial(Graphics* pGraphics);

	void Draw(Graphics* pGraphics);

private:
	std::shared_ptr<VertexBuffer> pVertexBuffer;
	std::shared_ptr<IndexBuffer> pIndexBuffer;

	std::shared_ptr<MeshMaterial> Material;
};