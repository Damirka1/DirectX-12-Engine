#pragma once
#ifndef TRIANGLE_HEADER
#define TRIANGLE_HEADER
#include "..\Headers\Graphics\Resources\Drawable.h"
#include <DirectXMath.h>
class ResourceManager;
class Texture2D;
class Sampler;

class Triangle : public Drawable
{
public:
	Engine_API Triangle(Graphics* pGraphics, ResourceManager* pRM);
	Engine_API void Draw(Graphics* pGraphics);
	Engine_API void Update(float r, float g, float b, float up, float left);
	Engine_API ~Triangle();

private:
	struct buf
	{
		DirectX::XMFLOAT4 color;
		DirectX::XMMATRIX pos;
	};
	buf b;

	std::shared_ptr<ConstantBuffer> pConstBuffer;
	std::shared_ptr<Texture2D> pTexture;
	std::shared_ptr<Sampler> pSampler;
};

#endif