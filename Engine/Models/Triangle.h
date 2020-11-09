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
	Engine_API void Update(float r, float g, float b);
	Engine_API ~Triangle();

private:
	struct Color
	{
		float color[3];
	};
	Color c;

	std::shared_ptr<ConstantBuffer> pConstBuffer;
};

#endif