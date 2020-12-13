#include "..\..\Headers\Graphics\Resources\UI_Element.h"
#include <windows.h>

UI_Element::UI_Element(std::string ObjectName, DirectX::XMFLOAT2 Pos, DirectX::XMFLOAT2 ActiveSize)
{
	Drawable::UI_element = true;
	Init(ObjectName, Pos, ActiveSize);
}

void UI_Element::Init(std::string ObjectName, DirectX::XMFLOAT2 Pos, DirectX::XMFLOAT2 ActiveSize)
{
	this->ObjectName = ObjectName;
	this->Pos = Pos;
	this->ActiveSize = ActiveSize;
	Drawable::UI_element = true;
}

void UI_Element::UpdateWindow(void* pWindow)
{
	::UpdateWindow(static_cast<HWND>(pWindow));
}
