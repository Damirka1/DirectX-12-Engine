#pragma once
#ifndef MESSAGE_HANDLER_HEADER
#define MESSAGE_HANDLER_HEADER
#include <Windows.h>

class MessageHandler
{
public:
	MessageHandler() = default;
	MessageHandler(MessageHandler&) = delete;
	virtual void CALLBACK HandleMsg(HWND& hWnd, UINT& msg, WPARAM& wParam, LPARAM& lParam) noexcept = 0;
	virtual ~MessageHandler() = default;
};


#endif