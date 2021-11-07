#pragma once
#include "../Headers/Graphics/Resources/Drawable.h"
#include <filesystem>
#include <DirectXMath.h>

struct aiMesh;
struct aiMaterial;
struct aiColor3D;

class Mesh : public Drawable
{
public:
	Mesh(ResourceManager* pRM, aiMesh* m, aiMaterial* material, std::filesystem::path path, float scale);
	void Update();

private:
	std::vector<std::shared_ptr<Resource>> pResources;
	std::shared_ptr<ConstantBuffer> pConstBuffer;
	DirectX::XMFLOAT3 color = { 0.65f,0.45f,0.45f };
};