#pragma once
#ifndef EVENTLISTENER_HEADER
#define EVENTLISTENER_HEADER

#include "..\Header.h"
#include "..\Window.h"
#include "MouseEvents.h"
#include "KeyboardEvents.h"
#include <functional>

class Drawable;
class Window;

class EventListener
{
	friend class ScriptManager;
public:
	Engine_API virtual void ListenMouseEvents(Drawable* pObject, MouseEvent* pEvent, Window* pWindow);
	Engine_API virtual void ListenKeyboardEvents(Drawable* pObject, KeyEvent* pEvent, Window* pWindow);
};


class DefaultEventListener : public EventListener
{
public:
	DefaultEventListener();

	std::function<void(UI_Element*, Window*)> OnMouseMove;
	std::function<void(UI_Element*, Window*)> OnMouseEnter;
	std::function<void(UI_Element*, Window*)> OnMouseLeave;
	std::function<void(UI_Element*, Window*)> OnMouseScrollDown;
	std::function<void(UI_Element*, Window*)> OnMouseScrollUp;
	std::function<void(UI_Element*, Window*)> OnMouseDoubleClick;
	std::function<void(UI_Element*, Window*)> OnMouseLbClick;
	std::function<void(UI_Element*, Window*)> OnMouseLbUp;
	std::function<void(UI_Element*, Window*)> OnMouseRbClick;
	std::function<void(UI_Element*, Window*)> OnMouseRbUp;
	std::function<void(UI_Element*, Window*)> OnMouseMbClick;
	std::function<void(UI_Element*, Window*)> OnMouseMbUp;

private:
	virtual void ListenMouseEvents(Drawable* pObject, MouseEvent* pEvent, Window* pWindow) override;
	virtual void ListenKeyboardEvents(Drawable* pObject, KeyEvent* pEvent, Window* pWindow) override;
	
};

#endif