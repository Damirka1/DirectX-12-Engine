#pragma once
#ifndef WINDOW_HEADER
#define Window_HEADER
#include <Windows.h>
#include "MessageHandler.h"
#include <unordered_map>
#include <unordered_set>

#include "Header.h"
#include "Timer.h"

class Graphics;
class UI_Element;
class Keyboard;
class Mouse;

class Window
{
	friend class FrameCommanderHWND;
public:
	Window() = delete;
	Engine_API Window(HINSTANCE hInst, const wchar_t* WindowName, short Width = 800, short Height = 600, bool VSync = true);
	Engine_API Window(const wchar_t* WindowName, short Width = 800, short Height = 600, bool VSync = true);
	Engine_API ~Window();
	Engine_API void	SetWindowName(const char* Name)	const noexcept;
	Engine_API void	Show() const noexcept;
	Engine_API void	Hide() const noexcept;
	Engine_API bool	IsExist() const noexcept;
	Engine_API void	ProcessMessages() const;
	Engine_API HWND	GetHWND() noexcept;
	Engine_API void UpdateWindow() noexcept;

	Engine_API void EnableVSync() noexcept;
	Engine_API void DisableVSync() noexcept;
	Engine_API bool IsEnableVSycn() const noexcept;

	Engine_API std::pair<short, short> GetWindowResolution() const noexcept;
	Engine_API std::pair<short, short> GetGraphicsResolution() const noexcept;

	// Return elapsed time from start window.				 
	Engine_API float TimerPeek() const noexcept;

	// Return elapsed time from frame render.				 
	Engine_API float TimerMark() const noexcept;

	// All handler to map. Handler will be used in messages process.
	Engine_API bool	AddHandler(MessageHandler* ptr, const char* Name) noexcept;
	Engine_API bool	RemoveHandler(const char* Name) noexcept;

					// Add UI elements to the window.						 
	Engine_API void	AddElement(UI_Element* pElement);
	Engine_API void	RemoveElement(UI_Element* pElement) noexcept;
																						 
	Engine_API const Graphics*	GetGraphics() const noexcept;
	Engine_API const Keyboard*	GetKeyboard() const noexcept;
	Engine_API const Mouse*		GetMouse() const noexcept;

	Engine_API		 Graphics*	GetGraphics() noexcept;
	Engine_API		 Keyboard*	GetKeyboard() noexcept;
	Engine_API		 Mouse*		GetMouse() noexcept;

	Engine_API void EnableCursor() noexcept;
	Engine_API void DisableCursor() noexcept;

	void ConfineCursor() noexcept;
	void FreeCursor() noexcept;
	void HideCursor() noexcept;
	void ShowCursor() noexcept;

private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK StaticHandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT CALLBACK HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	static void HandleUIElements(Window* pWindow);

private:
	mutable bool Exist = false;
	mutable short Width, Height;

	HWND pWindow = nullptr;
	HINSTANCE hInst = nullptr;
	Graphics* pGraphics;
	bool VSync;
	BYTE SleepTime = 0;
	mutable bool Visible;
	
	mutable std::unordered_map<std::string, MessageHandler*> MessageHandlers;
	mutable std::unordered_map<std::string, UI_Element*> UI_elements;

	// Input
	Keyboard* pKeyboard;
	Mouse* pMouse;

	Timer* t;

	// Handle thread.
	HANDLE ThreadElements;
};
#endif