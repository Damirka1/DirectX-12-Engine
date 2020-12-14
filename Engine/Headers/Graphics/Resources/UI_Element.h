#pragma once
#ifndef UI_ELEMENT_HEADER
#define UI_ELEMENT_HEADER
#include "Drawable.h"
#include <DirectXMath.h>

class UI_Element : public Drawable
{
	friend class Window;
public:
	UI_Element() noexcept = default;
	UI_Element(ResourceManager* pRM, std::string ObjectName, DirectX::XMFLOAT2 Pos, DirectX::XMFLOAT2 ActiveSize) noexcept;

	void Init(ResourceManager* pRM, std::string ObjectName, DirectX::XMFLOAT2 Pos, DirectX::XMFLOAT2 ActiveSize) noexcept;

	void (*OnMouseLbClick)(void* This, void* pWindow) = nullptr;
	void (*OnMouseRbClick)(void* This, void* pWindow) = nullptr;
	void (*OnMouseMbClick)(void* This, void* pWindow) = nullptr;
	void (*OnMouseEnter)(void* This, void* pWindow) = nullptr;
	void (*OnMouseLeave)(void* This, void* pWindow) = nullptr;
	void (*OnMouseScrollDown)(void* This, void* pWindow) = nullptr;
	void (*OnMouseScrollUp)(void* This, void* pWindow) = nullptr;

	void (*OnUpdate)(void* This, void* pWindow) = nullptr;

	void UpdateWindow(void* pWindow) noexcept;
protected:
	std::string ObjectName;
	DirectX::XMFLOAT2 Pos;
	DirectX::XMFLOAT2 ActiveSize;

	struct
	{
		DirectX::XMMATRIX Pos;
		DirectX::XMMATRIX Projection;
	}Cbuff;


};


#endif