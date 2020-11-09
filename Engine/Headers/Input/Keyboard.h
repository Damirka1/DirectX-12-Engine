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
		KeyEvent();
		KeyEvent(unsigned char Code, Type type);
		
		Engine_API bool operator==(const char* Key);

	private:
		unsigned char Code;
		Type t;
	};


public:
	Engine_API Keyboard(unsigned int QueueSize = 20);
	Engine_API bool KeyIsPressed(const char* KeyCode);
	Engine_API std::optional<KeyEvent> GetEvent();

private:
	void CALLBACK HandleMsg(HWND& hWnd, UINT& msg, WPARAM& wParam, LPARAM& lParam) override;
	void PopQueue();

	std::bitset<256> keys;
	std::queue<KeyEvent> Events;
	unsigned int QueueSize;
};


#endif