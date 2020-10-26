#pragma once
#ifndef WINDOW_HEADER
#define Window_HEADER
#include <Windows.h>

#include "Header.h"

class Graphics;

class Window
{
public:
	Window() = delete;
	Engine_API Window(HINSTANCE hInst, const wchar_t* WindowName, short Width = 800, short Height = 600);
	Engine_API Window(const wchar_t* WindowName, short Width = 800, short Height = 600);
	~Window() = default;
	Engine_API void Show();
	Engine_API void Hide();
	Engine_API bool IsExist();
	Engine_API void ProcessMessages();
	Engine_API void SetGraphics(Graphics* pGtx);
	Engine_API HWND GetHWND();

private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK StaticHandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	bool Exist = false;
	short Width, Height;
	HWND pWindow = nullptr;
	HINSTANCE hInst = nullptr;
	Graphics* pGtx = nullptr;
};
#endif