#include "..\Headers\Window.h"
#include "..\Headers\Graphics\Resources\UI_Element.h"
#include <exception>

Window::Window(HINSTANCE hInst, const wchar_t* WindowName, short Width, short Height)
	:
	hInst(hInst),
	Width(Width),
	Height(Height),
	t(new Timer)
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;

	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = L"Default Class";

	RegisterClassEx(&wc);

	// Calculate window size based on desired client region size.
	RECT wr;
	wr.left = 100;
	wr.right = Width + wr.left;
	wr.top = 100;
	wr.bottom = Height + wr.top;

	if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
	{
		throw std::exception("Window size error");
	}
	

	// Get center of the screen.
	RECT sr;
	LONG x = 0u, y = 0u, Wwidth = wr.right - wr.left, Wheight = wr.bottom - wr.top;
	if(GetWindowRect(GetDesktopWindow(), &sr))
	{
		x = (sr.right / 2u) - (Wwidth / 2u);
		y = (sr.bottom / 2u) - (Wheight / 2u);
	}
	else
	{
		x = sr.right / 4u;
		y = sr.bottom / 4u;
	}

	// Create window and get hWnd.
	pWindow = CreateWindowW(
		L"Default Class", WindowName,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		x, y, Wwidth, Wheight,
		nullptr, nullptr, hInst, this
	);

	// Check for error.
	if (pWindow == nullptr)
	{
		throw std::exception("Window creation error");
	}

	Exist = true;

}

Window::Window(const wchar_t* WindowName, short Width, short Height)
	:
	Window(GetModuleHandle(nullptr), WindowName, Width, Height)
{
}

Window::~Window()
{
	delete t;
}

void Window::SetWindowName(const char* Name)
{
	SetWindowTextA(pWindow, Name);
}

void Window::Show()
{
	ShowWindow(pWindow, SW_SHOWDEFAULT);
}

void Window::Hide()
{
	ShowWindow(pWindow, SW_HIDE);
}

bool Window::IsExist()
{
	return Exist;
}

void Window::ProcessMessages()
{
	MSG msg;
	// while queue has messages, remove and dispatch them (but do not block on empty queue)
	while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		// TranslateMessage will post auxilliary WM_CHAR messages from key msgs
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
}

void Window::SetGraphics(Graphics* pGtx)
{
	this->pGtx = pGtx;
}

HWND Window::GetHWND()
{
	return pWindow;
}

float Window::TimerPeek()
{
	return t->Peek();
}

float Window::TimerMark()
{
	return t->Mark();
}

bool Window::AddHandler(MessageHandler* ptr, const char* Name)
{
	const auto i = MessageHandlers.find(Name);
	if (i == MessageHandlers.end())
	{
		MessageHandlers[Name] = ptr;
		return true;
	}
	return false;
}

bool Window::RemoveHandler(const char* Name)
{
	const auto i = MessageHandlers.find(Name);
	if (i == MessageHandlers.end())
	{
		MessageHandlers.erase(i);
		return true;
	}
	return false;
}

void Window::AddElement(UI_Element* pElement)
{
	if (pElement)
	{
		auto i = UI_elements.find(pElement->ObjectName);

		if (i == UI_elements.end())
		{
			UI_elements[pElement->ObjectName] = pElement;
		}
		else
			throw std::exception((std::string("Element with name: ") + pElement->ObjectName + " already exsist").c_str());
	}
}

void Window::RemoveElement(UI_Element* pElement)
{
	if(pElement)
		UI_elements.erase(pElement->ObjectName);
}

LRESULT Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Use create parameter passed in from CreateWindow() to store window class pointer at WinAPI side.
	if (msg == WM_NCCREATE)
	{
		// extract ptr to window class from creation data
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		// set WinAPI-managed user data to store ptr to window instance
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		// set message proc to normal (non-setup) handler now that setup is finished
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::StaticHandleMsg));
		// forward message to window instance handler
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	// If we get a message before the WM_NCCREATE message, handle with default handler.
	return DefWindowProcW(hWnd, msg, wParam, lParam);
}

LRESULT Window::StaticHandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Retrieve ptr to window instance.
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	// Forward message to window instance handler.
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		Exist = false;
		PostQuitMessage(0);
		return 0;
	}

	// Processing all handlers in map.
	for (auto& el : MessageHandlers)
		el.second->HandleMsg(hWnd, msg, wParam, lParam);

	return DefWindowProcW(hWnd, msg, wParam, lParam);
}
