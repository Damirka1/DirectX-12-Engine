#pragma once
#ifndef TRIANGLE_HEADER
#define TRIANGLE_HEADER
#include "..\Headers\Graphics\Resources\Drawable.h"
#include <DirectXMath.h>

class Triangle : public Drawable
{
public:
	Engine_API Triangle(ResourceManager* pRM, std::string TexturePath);
	Engine_API void Update(float up, float left, float forward);
	Engine_API ~Triangle();

private:

	std::shared_ptr<ConstantBuffer> pConstBuffer;
	std::shared_ptr<Texture2D> pTexture;
	std::shared_ptr<Sampler> pSampler;
};

#endif