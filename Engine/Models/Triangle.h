#pragma once
#ifndef TRIANGLE_HEADER
#define TRIANGLE_HEADER
#include "..\Headers\Graphics\Resources\Drawable.h"
class ResourceManager;

class Triangle : public Drawable
{
public:
	Engine_API Triangle(Graphics* pGraphics, ResourceManager* pRM);

	Engine_API void Draw(Graphics* pGraphics);

private:
};

#endif