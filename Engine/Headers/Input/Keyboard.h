#pragma once
#ifndef KEYBOARD_HEADER
#define KEYBOARD_HEADER
#include "..\MessageHandler.h"
#include "..\Header.h"
#include "..\Events\KeyboardEvents.h"
#include <bitset>
#include <queue>
#include <optional>

class Keyboard : public MessageHandler
{
	friend class ScriptManager;
	friend class Window;
public:
	Engine_API Keyboard(unsigned int QueueSize = 20) noexcept;
	Engine_API bool KeyIsPressed(const char KeyCode) noexcept;
	Engine_API std::optional<KeyEvent> GetEvent() noexcept;
	Engine_API std::optional<char> GetCharacters() noexcept;

private:
	void CALLBACK HandleMsg(HWND& hWnd, UINT& msg, WPARAM& wParam, LPARAM& lParam) noexcept override;
	void PopQueueEv() noexcept;
	void PopQueueCh() noexcept;
	void ClearState() noexcept;

	std::bitset<256> keys;
	std::queue<KeyEvent> Events;
	std::queue<KeyEvent> EventsForWindow;
	std::queue<char> Characters;
	unsigned int QueueSize;
};


#endif