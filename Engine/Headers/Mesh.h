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
	Mesh(ResourceManager* pRM, const aiMesh& m, const aiMaterial& material, std::filesystem::path path, float scale);
	void Update();

private:
	std::shared_ptr<ConstantBuffer> pConstBuffer;
	DirectX::XMFLOAT3 color = { 0.65f,0.45f,0.45f };
};