#pragma once

#include "Graphics/Resources/Drawable.h"
#include "Mesh.h"

class Model
{
public:
	Engine_API Model(ResourceManager* pRM, std::string ModelPath, float scale = 1.0f);
	Engine_API void Update();
	Engine_API void SetPos(DirectX::XMFLOAT3 Pos);
	Engine_API DirectX::XMFLOAT3 GetPos();

	Engine_API ~Model();

private:
	std::string Name;
	DirectX::XMFLOAT3 Position;
	std::vector<Mesh*> Meshes;
};