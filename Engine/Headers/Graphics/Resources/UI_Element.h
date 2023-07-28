//#pragma once
//#ifndef UI_ELEMENT_HEADER
//#define UI_ELEMENT_HEADER
//#include "Drawable.h"
//
//class UI_Element : public Drawable
//{
//	friend class Window;
//	friend EventListener;
//	friend DefaultEventListener;
//public:
//	Engine_API UI_Element() noexcept = default;
//	Engine_API UI_Element(ResourceManager* pRM, std::string ObjectName, DirectX::XMFLOAT2 Pos, DirectX::XMFLOAT2 ActiveSize, bool UseDefaultListener = true) noexcept;
//	Engine_API void Init(ResourceManager* pRM, std::string ObjectName, DirectX::XMFLOAT2 Pos, DirectX::XMFLOAT2 ActiveSize, bool UseDefaultListener = true) noexcept;
//	
//	// Return listener wich contains function like OnMouseMove, OnMouseEnter, and so on.
//	Engine_API DefaultEventListener* GetDefaultListener();
//	Engine_API bool IsMouseInside();
//
//	Engine_API ~UI_Element();
//
//protected:
//	std::string ObjectName;
//	DirectX::XMFLOAT2 ActiveSize;
//	bool MouseInside = false;
//	
//	LONG_PTR DefaultEvListKey = -1;
//};
//
//#endif