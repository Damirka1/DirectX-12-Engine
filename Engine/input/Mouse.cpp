#include "..\Headers\Input\Mouse.h"
#include "..\Headers\Window.h"

Mouse::Mouse(unsigned int QueueSize, bool UseRawInput)
	:
	QueueSize(QueueSize),
	UseRawInput(UseRawInput),
	CursorEnabled(true)
{
	// register mouse raw input device
	RAWINPUTDEVICE rid;
	rid.usUsagePage = 0x01; // mouse page
	rid.usUsage = 0x02; // mouse usage
	rid.dwFlags = 0;
	rid.hwndTarget = nullptr;
	
	if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
	{
		std::exception("Can't initialize raw input for mouse");
	}
}

std::optional<Mouse::MouseEvent> Mouse::GetEvent() noexcept
{
	if (Events.size() > 0)
	{
		auto ev = Events.front();
		Events.pop();
		return ev;
	}
	return std::nullopt;
}

std::optional<Mouse::RawData> Mouse::GetRawData() noexcept
{
	if (RawInputEvents.size() > 0)
	{
		auto ev = RawInputEvents.front();
		RawInputEvents.pop();
		return ev;
	}
	return std::nullopt;
}

std::pair<short, short> Mouse::GetPos() noexcept
{
	return Pos;
}

void Mouse::EnableRawInput() noexcept
{
	UseRawInput = true;
}

void Mouse::DisableRawInput() noexcept
{
	UseRawInput = false;
}

bool Mouse::IsCursorEnabled() noexcept
{
	return CursorEnabled;
}


void Mouse::HandleMsg(HWND& hWnd, UINT& msg, WPARAM& wParam, LPARAM& lParam) noexcept
{
	Window* const pWindow = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	switch (msg)
	{
		case WM_ACTIVATE:
		// confine/free cursor on window to foreground/background if cursor disabled
		if (!CursorEnabled)
		{
			if (wParam & WA_ACTIVE)
			{
				pWindow->ConfineCursor();
				pWindow->HideCursor();
			}
			else
			{
				pWindow->FreeCursor();
				pWindow->ShowCursor();
			}
		}
		break;

		case WM_MOUSEMOVE:
		{
			Pos = { static_cast<short>(lParam), static_cast<short>(lParam >> 16) };
			// cursorless exclusive gets first dibs
			if (!CursorEnabled)
			{
				if (!CursorInWindow)
				{
					SetCapture(hWnd);
					Events.emplace(MouseEvent::MouseType::Enter, Pos);
					CursorInWindow = true;
					pWindow->HideCursor();
				}
				break;
			}
			const auto res = pWindow->GetWindowResolution();
			// in client region -> log move, and log enter + capture mouse (if not previously in window)
			if (Pos.first >= 0 && Pos.first < res.first && Pos.second >= 0 && Pos.second < res.second)
			{
				if (!CursorInWindow)
				{
					SetCapture(hWnd);
					Events.emplace(MouseEvent::MouseType::Enter, Pos);
					CursorInWindow = true;
				}
			}
			// not in client -> log move / maintain capture if button down
			else
			{
				// button up -> release capture / log event for leaving
				if (!(wParam & (MK_LBUTTON | MK_RBUTTON)))
				{
					ReleaseCapture();
					Events.emplace(MouseEvent::MouseType::Leave, Pos);
					CursorInWindow = false;
				}
			}
			break;
		}

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

		/************** RAW MOUSE MESSAGES **************/
		case WM_INPUT:
		{
			if (!UseRawInput)
			{
				break;
			}
			UINT size;
			// first get the size of the input data
			if (GetRawInputData(
				reinterpret_cast<HRAWINPUT>(lParam),
				RID_INPUT,
				nullptr,
				&size,
				sizeof(RAWINPUTHEADER)) == -1)
			{
				// bail msg processing if error
				break;
			}
			rawBuffer.resize(size);
			// read in the input data
			if (GetRawInputData(
				reinterpret_cast<HRAWINPUT>(lParam),
				RID_INPUT,
				rawBuffer.data(),
				&size,
				sizeof(RAWINPUTHEADER)) != size)
			{
				// bail msg processing if error
				break;
			}
			// process the raw input data
			auto& ri = reinterpret_cast<const RAWINPUT&>(*rawBuffer.data());
			if (ri.header.dwType == RIM_TYPEMOUSE &&
				(ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0))
			{
				RawInputEvents.emplace(ri.data.mouse.lLastX, ri.data.mouse.lLastY);
				PopQueueRawInput();
			}
			break;
		}
		/************** END RAW MOUSE MESSAGES **************/

	}

	PopQueue();
	
}

void Mouse::PopQueue() noexcept
{
	while (Events.size() > QueueSize)
		Events.pop();
}

void Mouse::PopQueueRawInput() noexcept
{
	while (RawInputEvents.size() > QueueSize)
		RawInputEvents.pop();
}

Mouse::MouseEvent::MouseEvent() noexcept
	:
	t(Mouse::MouseEvent::MouseType::Undefined),
	Pos(-1, -1)
{
}

Mouse::MouseEvent::MouseEvent(MouseType Type, std::pair<short, short> Pos) noexcept
	:
	t(Type),
	Pos(Pos)
{
}

bool Mouse::MouseEvent::L_Pressed() noexcept
{
	return t == MouseType::L_Pressed ? true : false;
}

bool Mouse::MouseEvent::R_Pressed() noexcept
{
	return t == MouseType::R_Pressed ? true : false;
}

bool Mouse::MouseEvent::WheelDown() noexcept
{
	return t == MouseType::WheelDown ? true : false;
}

bool Mouse::MouseEvent::WheelUp() noexcept
{
	return t == MouseType::WheelUp? true : false;
}

bool Mouse::MouseEvent::WheelPressed() noexcept
{
	return t == MouseType::WheelPressed ? true : false;
}

std::pair<short, short> Mouse::MouseEvent::GetPos() noexcept
{
	return Pos;
}
