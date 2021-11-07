#ifndef CUBE_HEADER
#define CUBE_HEADER
#include "..\Headers\Graphics\Resources\Drawable.h"


class Cube : public Drawable
{
public:
	Cube(ResourceManager* pRM, DirectX::XMFLOAT3 Pos) noexcept;
	void Update();
	~Cube();

private:
	
	float angle;
	float dAngle;
	float ax, ay, az;
	DirectX::XMFLOAT4 Color;


	std::shared_ptr<ConstantBuffer> pConstBuffer;
	std::shared_ptr<ConstantBuffer> pConstBufferColors;
};
#endif