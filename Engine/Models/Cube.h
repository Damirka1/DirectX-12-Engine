#ifndef CUBE_HEADER
#define CUBE_HEADER
#include "..\Headers\Graphics\Resources\Drawable.h"


class Cube : public Drawable
{
public:
	Engine_API Cube(ResourceManager* pRM, DirectX::XMFLOAT3 Pos) noexcept;
	Engine_API void Update() override;
	Engine_API ~Cube();

private:
	
	float angle;
	float dAngle;
	float ax, ay, az;
	DirectX::XMFLOAT4 Color;


	std::shared_ptr<ConstantBuffer> pConstBuffer;
	std::shared_ptr<ConstantBuffer> pConstBufferColors;
};
#endif