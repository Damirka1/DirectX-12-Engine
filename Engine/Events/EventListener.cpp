#include "..\Headers\Events\EventListener.h"
#include "..\Headers\Graphics\Resources\UI_Element.h"


void DoNothing(UI_Element* pObject, Window* pWindow)
{
}

DefaultEventListener::DefaultEventListener()
{
	OnMouseMove = DoNothing;
	OnMouseEnter = DoNothing;
	OnMouseLeave = DoNothing;
	OnMouseLbClick = DoNothing;
	OnMouseLbUp = DoNothing;
	OnMouseDoubleClick = DoNothing;
	OnMouseScrollDown = DoNothing;
	OnMouseScrollUp = DoNothing;
	OnMouseRbClick = DoNothing;
	OnMouseRbUp = DoNothing;
	OnMouseMbClick = DoNothing;
	OnMouseMbUp = DoNothing;
}

void DefaultEventListener::ListenMouseEvents(Drawable* pObject, MouseEvent* pEvent, Window* pWindow)
{
	// Listen default mouse events.

	UI_Element* pElement = static_cast<UI_Element*>(pObject);
	auto MousePos = pEvent->GetPos();

	auto ElementPos = pObject->GetPos();

	short Xmin = short(ElementPos.x - pElement->ActiveSize.x);
	short Xmax = short(ElementPos.x + pElement->ActiveSize.x);

	short Ymin = short(ElementPos.y - pElement->ActiveSize.y);
	short Ymax = short(ElementPos.y + pElement->ActiveSize.y);

	if ((MousePos.first >= Xmin && MousePos.first <= Xmax) && (MousePos.second >= Ymin && MousePos.second <= Ymax))
	{
		if (*pEvent == MouseEvent::Type::Move)
		{
			if (!pElement->MouseInside)
			{
				OnMouseEnter(pElement, pWindow);
				pElement->MouseInside = true;
			}
			else
				OnMouseMove(pElement, pWindow);
		}
		else if (*pEvent == MouseEvent::Type::L_Pressed)
			OnMouseLbClick(pElement, pWindow);
		else if (*pEvent == MouseEvent::Type::L_Released)
			OnMouseLbUp(pElement, pWindow);
		else if (*pEvent == MouseEvent::Type::R_Pressed)
			OnMouseRbClick(pElement, pWindow);
		else if (*pEvent == MouseEvent::Type::R_Released)
			OnMouseRbUp(pElement, pWindow);
		else if (*pEvent == MouseEvent::Type::WheelPressed)
			OnMouseMbClick(pElement, pWindow);
		else if (*pEvent == MouseEvent::Type::WheelReleased)
			OnMouseMbUp(pElement, pWindow);
		else if (*pEvent == MouseEvent::Type::WheelDown)
			OnMouseScrollDown(pElement, pWindow);
		else if (*pEvent == MouseEvent::Type::WheelUp)
			OnMouseScrollUp(pElement, pWindow);
	}
	else
	{
		if (pElement->MouseInside)
		{
			OnMouseLeave(pElement, pWindow);
			pElement->MouseInside = false;
		}
	}
}

void DefaultEventListener::ListenKeyboardEvents(Drawable* pObject, KeyEvent* pEvent, Window* pWindow)
{
}


