#pragma once
#ifndef RECT_HEADER
#define RECT_HEADER
#include "..\Headers\Graphics\Resources\UI_Element.h"

class Rect : public UI_Element
{
public:
	Engine_API Rect(ResourceManager* pRM, std::string Name, DirectX::XMFLOAT2 Pos, DirectX::XMFLOAT2 Size) noexcept;
	Engine_API ~Rect();
	Engine_API void SetColor(DirectX::XMFLOAT3 Color);
	void Update() override;

private:
	std::shared_ptr<ConstantBuffer> pBufferColor;
	std::shared_ptr<ConstantBuffer> pBufferProjection;
	DirectX::XMFLOAT3 Color = { 1.0f, 0.5f, 0.5f };
};

#endif