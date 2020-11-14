#include "..\Headers\Input\Mouse.h"

Mouse::Mouse(unsigned int QueueSize)
	:
	QueueSize(QueueSize)
{
}

std::optional<Mouse::MouseEvent> Mouse::GetEvent()
{
	if (Events.size() > 0)
	{
		auto ev = Events.front();
		Events.pop();
		return ev;
	}
	return std::nullopt;
}

std::pair<short, short> Mouse::GetPos()
{
	return Pos;
}

void Mouse::HandleMsg(HWND& hWnd, UINT& msg, WPARAM& wParam, LPARAM& lParam)
{

	switch (msg)
	{
		case WM_LBUTTONDOWN:
			Pos = { static_cast<short>(lParam), static_cast<short>(lParam >> 16) };
			Events.emplace(MouseEvent::MouseType::L_Pressed, Pos);
			break;
		case WM_LBUTTONUP:
			Pos = { static_cast<short>(lParam), static_cast<short>(lParam >> 16) };
			Events.emplace(MouseEvent::MouseType::L_Released, Pos);
			break;
		case WM_RBUTTONDOWN:
			Pos = { static_cast<short>(lParam), static_cast<short>(lParam >> 16) };
			Events.emplace(MouseEvent::MouseType::R_Pressed, Pos);
			break;
		case WM_RBUTTONUP:
			Pos = { static_cast<short>(lParam), static_cast<short>(lParam >> 16) };
			Events.emplace(MouseEvent::MouseType::R_Released, Pos);
			break;
		case WM_MBUTTONDOWN:
			Pos = { static_cast<short>(lParam), static_cast<short>(lParam >> 16) };
			Events.emplace(MouseEvent::MouseType::WheelPressed, Pos);
			break;
		case WM_MBUTTONUP:
			Pos = { static_cast<short>(lParam), static_cast<short>(lParam >> 16) };
			Events.emplace(MouseEvent::MouseType::WheelReleased, Pos);
			break;
		case WM_MOUSEMOVE:
			Pos = { static_cast<short>(lParam), static_cast<short>(lParam >> 16) };
			break;
		case WM_MOUSEWHEEL:
		{
			Pos = { static_cast<short>(lParam), static_cast<short>(lParam >> 16) };
			short WheelCount = static_cast<short>(wParam >> 16) / 120;
			if (static_cast<short>(wParam >> 16) > 0)
			{
				while (WheelCount != 0)
				{
					Events.emplace(MouseEvent::MouseType::WheelUp, Pos);
					WheelCount--;
				}
			}
			else
			{
				while (WheelCount != 0)
				{
					Events.emplace(MouseEvent::MouseType::WheelDown, Pos);
					WheelCount++;
				}
			}

			break;
		}

	}

	PopQueue();
}

void Mouse::PopQueue()
{
	while (Events.size() > QueueSize)
		Events.pop();
}

Mouse::MouseEvent::MouseEvent()
	:
	t(Mouse::MouseEvent::MouseType::Undefined),
	Pos(-1, -1)
{
}

Mouse::MouseEvent::MouseEvent(MouseType Type, std::pair<short, short> Pos)
	:
	t(Type),
	Pos(Pos)
{
}

bool Mouse::MouseEvent::L_Pressed()
{
	return t == MouseType::L_Pressed ? true : false;
}

bool Mouse::MouseEvent::R_Pressed()
{
	return t == MouseType::R_Pressed ? true : false;
}

bool Mouse::MouseEvent::WheelDown()
{
	return t == MouseType::WheelDown ? true : false;
}

bool Mouse::MouseEvent::WheelUp()
{
	return t == MouseType::WheelUp? true : false;
}

bool Mouse::MouseEvent::WheelPressed()
{
	return t == MouseType::WheelPressed ? true : false;
}

std::pair<short, short> Mouse::MouseEvent::GetPos()
{
	return Pos;
}
