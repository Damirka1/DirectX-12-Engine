#pragma once

#include "Graphics/Resources/Drawable.h"
#include "Mesh.h"

class Model : public Drawable
{
public:
	Engine_API Model(ResourceManager* pRM, std::string ModelPath, float scale = 1.0f);
	Engine_API void Update();

	Engine_API ~Model();

private:
	void ReadObjFile(std::string ModelPath);

private:
	std::string Name;
	std::vector<Mesh*> Meshes;
};