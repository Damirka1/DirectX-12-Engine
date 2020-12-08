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
	Engine_API Cube(ResourceManager* pRM, DirectX::XMFLOAT3 Pos);
	Engine_API void Update(DirectX::XMFLOAT3 Translation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
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
	DirectX::XMFLOAT3 Pos;
	float angle;
	float dAngle;
	float ax, ay, az;


	std::shared_ptr<ConstantBuffer> pConstBuffer;
	std::shared_ptr<ConstantBuffer> pConstBufferColors;
};
#endif