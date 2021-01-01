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

std::optional<MouseEvent> Mouse::GetEvent() noexcept
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

	auto GetPos = [=]()
	{
		const auto res = pWindow->GetWindowResolution();
		Pos = { static_cast<short>(lParam), -(static_cast<short>(lParam >> 16)) + res.second };
	};

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
			const auto res = pWindow->GetWindowResolution();
			Pos = { static_cast<short>(lParam), -(static_cast<short>(lParam >> 16)) + res.second };
			Events.emplace(MouseEvent::Type::Move, Pos);
			EventsForWindow.emplace(MouseEvent::Type::Move, Pos);
			// cursorless exclusive gets first dibs
			if (!CursorEnabled)
			{
				if (!CursorInWindow)
				{
					SetCapture(hWnd);
					Events.emplace(MouseEvent::Type::EnterWindow, Pos);
					EventsForWindow.emplace(MouseEvent::Type::EnterWindow, Pos);
					CursorInWindow = true;
					pWindow->HideCursor();
				}
				break;
			}
			
			// in client region -> log move, and log enter + capture mouse (if not previously in window)
			if (Pos.first >= 0 && Pos.first < res.first && Pos.second >= 0 && Pos.second < res.second)
			{
				if (!CursorInWindow)
				{
					SetCapture(hWnd);
					Events.emplace(MouseEvent::Type::EnterWindow, Pos);
					EventsForWindow.emplace(MouseEvent::Type::EnterWindow, Pos);
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
					Events.emplace(MouseEvent::Type::LeaveWindow, Pos);
					EventsForWindow.emplace(MouseEvent::Type::LeaveWindow, Pos);
					CursorInWindow = false;
				}
			}
			break;
		}

		case WM_LBUTTONDOWN:
			SetForegroundWindow(hWnd);
			if (!CursorEnabled)
			{
				pWindow->ConfineCursor();
				pWindow->HideCursor();
			}
			GetPos();
			Events.emplace(MouseEvent::Type::L_Pressed, Pos);
			EventsForWindow.emplace(MouseEvent::Type::L_Pressed, Pos);
			break;
		case WM_LBUTTONUP:
			GetPos();
			Events.emplace(MouseEvent::Type::L_Released, Pos);
			EventsForWindow.emplace(MouseEvent::Type::L_Released, Pos);
			break;
		case WM_RBUTTONDOWN:
			GetPos();
			Events.emplace(MouseEvent::Type::R_Pressed, Pos);
			EventsForWindow.emplace(MouseEvent::Type::R_Pressed, Pos);
			break;
		case WM_RBUTTONUP:
			GetPos();
			Events.emplace(MouseEvent::Type::R_Released, Pos);
			EventsForWindow.emplace(MouseEvent::Type::R_Released, Pos);
			break;
		case WM_MBUTTONDOWN:
			GetPos();
			Events.emplace(MouseEvent::Type::WheelPressed, Pos);
			EventsForWindow.emplace(MouseEvent::Type::WheelPressed, Pos);
			break;
		case WM_MBUTTONUP:
			GetPos();
			Events.emplace(MouseEvent::Type::WheelReleased, Pos);
			EventsForWindow.emplace(MouseEvent::Type::WheelReleased, Pos);
			break;
		
		case WM_MOUSEWHEEL:
		{
			short WheelCount = static_cast<short>(wParam >> 16) / 120;
			if (static_cast<short>(wParam >> 16) > 0)
			{
				while (WheelCount != 0)
				{
					Events.emplace(MouseEvent::Type::WheelUp, Pos);
					EventsForWindow.emplace(MouseEvent::Type::WheelUp, Pos);
					WheelCount--;
				}
			}
			else
			{
				while (WheelCount != 0)
				{
					Events.emplace(MouseEvent::Type::WheelDown, Pos);
					EventsForWindow.emplace(MouseEvent::Type::WheelDown, Pos);
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
