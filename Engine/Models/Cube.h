#ifndef CUBE_HEADER
#define CUBE_HEADER
#include "..\Headers\Graphics\Resources\Drawable.h"


#include <DirectXMath.h>
class ResourceManager;
class Texture2D;
class Sampler;

class Cube : public Drawable
{
public:
	Engine_API Cube(std::string TexturePath, ResourceManager* pRM);
	Engine_API void Update(float* pArray, int Count);
	Engine_API ~Cube();

private:
	struct buf
	{
		DirectX::XMMATRIX Projection;
		DirectX::XMMATRIX View;
		DirectX::XMMATRIX Pos;
	};
	buf b;

	struct FaceColors
	{
		float r, g, b, a;
	} FaceColor[6];

	std::shared_ptr<ConstantBuffer> pConstBuffer;
	std::shared_ptr<ConstantBuffer> pConstBufferColors;
	std::shared_ptr<Texture2D> pTexture;
	std::shared_ptr<Sampler> pSampler;
};
#endif