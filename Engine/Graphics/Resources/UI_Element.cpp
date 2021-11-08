#include "..\..\Headers\Graphics\Resources\UI_Element.h"


UI_Element::UI_Element(ResourceManager* pRM, std::string ObjectName, DirectX::XMFLOAT2 Pos, DirectX::XMFLOAT2 ActiveSize, bool UseDefaultListener) noexcept
{
	Init(pRM, ObjectName, Pos, ActiveSize, UseDefaultListener);
}

void UI_Element::Init(ResourceManager* pRM, std::string ObjectName, DirectX::XMFLOAT2 Pos, DirectX::XMFLOAT2 ActiveSize, bool UseDefaultListener) noexcept
{
	this->ObjectName = ObjectName;
	Drawable::Pos = DirectX::XMFLOAT3(Pos.x, Pos.y, 0.0f);
	this->ActiveSize = ActiveSize;
	//Drawable::UI_element = true;
	//Drawable::Init(pRM);

	if (UseDefaultListener && (DefaultEvListKey == -1))
	{
		DefaultEventListener* Listener = new DefaultEventListener();
		AddEventListener(Listener);
		DefaultEvListKey = LONG_PTR(Listener);
	}
}

DefaultEventListener* UI_Element::GetDefaultListener()
{
	if (DefaultEvListKey != -1)
		return static_cast<DefaultEventListener*>(EventListeners[DefaultEvListKey]);
	return nullptr;
}

bool UI_Element::IsMouseInside()
{
	return MouseInside;
}

UI_Element::~UI_Element()
{
	if (DefaultEvListKey != -1)
		delete static_cast<DefaultEventListener*>(EventListeners[DefaultEvListKey]);
}

