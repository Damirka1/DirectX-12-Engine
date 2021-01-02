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
	Engine_API Cube(ResourceManager* pRM, DirectX::XMFLOAT3 Pos) noexcept;
	Engine_API void Update() override;
	Engine_API ~Cube();

private:
	
	struct FaceColors
	{
		float r, g, b, a;
	} FaceColor[6];
	float angle;
	float dAngle;
	float ax, ay, az;


	std::shared_ptr<ConstantBuffer> pConstBuffer;
	std::shared_ptr<ConstantBuffer> pConstBufferColors;
};
#endif