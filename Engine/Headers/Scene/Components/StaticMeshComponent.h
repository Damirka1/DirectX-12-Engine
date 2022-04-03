#pragma once
#include <string>
#include <vector>
#include <memory>
#include <DirectXMath.h>

#include "../../Header.h"

#include "../../Graphics/Resources/DrawableMesh.h"

class ResourceManager;
class DrawableMesh;
class ConstantBuffer;
class Graphics;
class Camera;

class StaticMeshComponent
{
public:
	Engine_API StaticMeshComponent(ResourceManager* pRM, std::string ModelPath, float scale = 1.0f);

	Engine_API void Draw(Graphics* pGraphics);

	Engine_API void Update(Camera* cam);

	Engine_API void SetPos(DirectX::XMFLOAT3 Pos);

private:
	std::vector<DrawableMesh> Meshes;
	std::shared_ptr<ConstantBuffer> CB;
	DirectX::XMFLOAT3 Pos;
	DirectX::XMMATRIX Transformation;
};