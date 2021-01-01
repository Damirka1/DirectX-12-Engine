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
	OnMouseClick = DoNothing;
	OnMouseDoubleClick = DoNothing;
	OnMouseScrollDown = DoNothing;
	OnMouseScrollUp = DoNothing;
	OnMouseRbClick = DoNothing;
	OnMouseMbClick = DoNothing;
}

void DefaultEventListener::ListenMouseEvents(UI_Element* pObject, MouseEvent* pEvent, Window* pWindow)
{
	// Listen default mouse events.

	auto MousePos = pEvent->GetPos();

	auto ElementPos = pObject->GetPos();

	short Xmin = short(ElementPos.x - pObject->ActiveSize.x);
	short Xmax = short(ElementPos.x + pObject->ActiveSize.x);

	short Ymin = short(ElementPos.y - pObject->ActiveSize.y);
	short Ymax = short(ElementPos.y + pObject->ActiveSize.y);

	if ((MousePos.first >= Xmin && MousePos.first <= Xmax) && (MousePos.second >= Ymin && MousePos.second <= Ymax))
	{
		if (*pEvent == MouseEvent::Type::Move)
		{
			if (!pObject->MouseInside)
			{
				OnMouseEnter(pObject, pWindow);
				pObject->MouseInside = true;
			}
			else
				OnMouseMove(pObject, pWindow);
		}
		else if (*pEvent == MouseEvent::Type::L_Pressed)
			OnMouseClick(pObject, pWindow);
		else if (*pEvent == MouseEvent::Type::R_Pressed)
			OnMouseRbClick(pObject, pWindow);
		else if (*pEvent == MouseEvent::Type::WheelPressed)
			OnMouseMbClick(pObject, pWindow);
		else if (*pEvent == MouseEvent::Type::WheelDown)
			OnMouseScrollDown(pObject, pWindow);
		else if (*pEvent == MouseEvent::Type::WheelUp)
			OnMouseScrollUp(pObject, pWindow);
	}
	else
	{
		if (pObject->MouseInside)
		{
			OnMouseLeave(pObject, pWindow);
			pObject->MouseInside = false;
		}
	}
}

void DefaultEventListener::ListenKeyboardEvents(UI_Element* pObject, KeyEvent* pEvent, Window* pWindow)
{
}


