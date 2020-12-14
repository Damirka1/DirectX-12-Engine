#include "..\..\Headers\Graphics\Resources\UI_Element.h"
#include <windows.h>

UI_Element::UI_Element(ResourceManager* pRM, std::string ObjectName, DirectX::XMFLOAT2 Pos, DirectX::XMFLOAT2 ActiveSize) noexcept
	:
	Drawable(pRM)
{
	Drawable::UI_element = true;
	Init(pRM, ObjectName, Pos, ActiveSize);
}

void UI_Element::Init(ResourceManager* pRM, std::string ObjectName, DirectX::XMFLOAT2 Pos, DirectX::XMFLOAT2 ActiveSize) noexcept
{
	this->ObjectName = ObjectName;
	this->Pos = Pos;
	this->ActiveSize = ActiveSize;
	Drawable::UI_element = true;
	Drawable::Init(pRM);
}

void UI_Element::UpdateWindow(void* pWindow) noexcept
{
	::UpdateWindow(static_cast<HWND>(pWindow));
}
