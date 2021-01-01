#pragma once
#ifndef UI_ELEMENT_HEADER
#define UI_ELEMENT_HEADER
#include "Drawable.h"
#include <DirectXMath.h>
#include "..\..\Events\EventListener.h"


class UI_Element : public Drawable
{
	friend class Window;
	friend EventListener;
	friend DefaultEventListener;
public:
	Engine_API UI_Element() noexcept = default;
	Engine_API UI_Element(ResourceManager* pRM, std::string ObjectName, DirectX::XMFLOAT2 Pos, DirectX::XMFLOAT2 ActiveSize, bool UseDefaultListener = true) noexcept;
	Engine_API void Init(ResourceManager* pRM, std::string ObjectName, DirectX::XMFLOAT2 Pos, DirectX::XMFLOAT2 ActiveSize, bool UseDefaultListener = true) noexcept;

	Engine_API void AddEventListener(EventListener* EvListener);
	Engine_API void operator+=(EventListener* EvListener);
	Engine_API void operator=(EventListener* EvListener);
	
	// Return listener wich contains function like OnMouseMove, OnMouseEnter, and so on.
	Engine_API DefaultEventListener* GetDefaultListener();
	Engine_API bool IsMouseInside();

	Engine_API ~UI_Element();

protected:
	std::string ObjectName;
	DirectX::XMFLOAT2 ActiveSize;
	bool MouseInside = false;
	std::vector<EventListener*> EventListeners;
	int DefaultEvListIndex = -1;
};

#endif