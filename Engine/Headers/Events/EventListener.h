#pragma once
#ifndef EVENTLISTENER_HEADER
#define EVENTLISTENER_HEADER

#include "..\Header.h"
#include "..\Window.h"
#include "MouseEvents.h"
#include "KeyboardEvents.h"
#include <functional>

class EventListener
{
	friend class Window;
public:
	virtual void ListenMouseEvents(UI_Element* pObject, MouseEvent* pEvent, Window* pWindow) = 0;
	virtual void ListenKeyboardEvents(UI_Element* pObject, KeyEvent* pEvent, Window* pWindow) = 0;
};


class DefaultEventListener : public EventListener
{
public:
	DefaultEventListener();

	std::function<void(UI_Element*, Window*)> OnMouseMove;
	std::function<void(UI_Element*, Window*)> OnMouseEnter;
	std::function<void(UI_Element*, Window*)> OnMouseLeave;
	std::function<void(UI_Element*, Window*)> OnMouseClick;
	std::function<void(UI_Element*, Window*)> OnMouseDoubleClick;
	std::function<void(UI_Element*, Window*)> OnMouseScrollDown;
	std::function<void(UI_Element*, Window*)> OnMouseScrollUp;
	std::function<void(UI_Element*, Window*)> OnMouseRbClick;
	std::function<void(UI_Element*, Window*)> OnMouseMbClick;

private:
	virtual void ListenMouseEvents(UI_Element* pObject, MouseEvent* pEvent, Window* pWindow) override;
	virtual void ListenKeyboardEvents(UI_Element* pObject, KeyEvent* pEvent, Window* pWindow) override;
	
};

#endif