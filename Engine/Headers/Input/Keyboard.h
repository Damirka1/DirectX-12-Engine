#pragma once
#ifndef KEYBOARD_HEADER
#define KEYBOARD_HEADER
#include "..\MessageHandler.h"
#include "..\Header.h"
#include <bitset>
#include <queue>
#include <optional>

class Keyboard : public MessageHandler
{
	class KeyEvent
	{
		friend Keyboard;
		enum class Type
		{
			Pressed,
			Released,
			Undefined
		};

	public:
		KeyEvent() noexcept;
		KeyEvent(unsigned char Code, Type type) noexcept;
		
		Engine_API bool operator==(const char Key)  noexcept;

	private:
		unsigned char Code;
		Type t;
	};


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
	std::queue<char> Characters;
	unsigned int QueueSize;
};


#endif