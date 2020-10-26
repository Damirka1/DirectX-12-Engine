#include "..\Headers\Window.h"
#include <exception>

Window::Window(HINSTANCE hInst, const wchar_t* WindowName, short Width, short Height)
	:
	hInst(hInst),
	Width(Width),
	Height(Height)
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

	// calculate window size based on desired client region size
	RECT wr;
	wr.left = 100;
	wr.right = Width + wr.left;
	wr.top = 100;
	wr.bottom = Height + wr.top;

	if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
	{
		throw std::exception("Window size error");
	}
	// create window & get hWnd
	pWindow = CreateWindowW(
		L"Default Class", WindowName,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, hInst, this
	);
	// check for error
	if (pWindow == nullptr)
	{
		throw std::exception("Window creation error");
	}

	Exist = true;

}

Window::Window(const wchar_t* WindowName, short Width, short Height)
	:
	hInst(GetModuleHandle(nullptr)),
	Width(Width),
	Height(Height)
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

	// calculate window size based on desired client region size
	RECT wr;
	wr.left = 100;
	wr.right = Width + wr.left;
	wr.top = 100;
	wr.bottom = Height + wr.top;

	if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
	{
		throw std::exception("Window size error");
	}
	// create window & get hWnd
	pWindow = CreateWindowW(
		L"Default Class", WindowName,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, hInst, this
	);
	// check for error
	if (pWindow == nullptr)
	{
		throw std::exception("Window creation error");
	}
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

LRESULT Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// use create parameter passed in from CreateWindow() to store window class pointer at WinAPI side
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
	// if we get a message before the WM_NCCREATE message, handle with default handler
	return DefWindowProcW(hWnd, msg, wParam, lParam);
}

LRESULT Window::StaticHandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// retrieve ptr to window instance
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	// forward message to window instance handler
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
	return DefWindowProcW(hWnd, msg, wParam, lParam);
}
