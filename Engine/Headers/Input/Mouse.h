#pragma once
#ifndef MOUSE_HEADER
#define MOUSE_HEADER
#include "..\MessageHandler.h"
#include "..\Header.h"
#include <queue>
#include <optional>


class Mouse : public MessageHandler
{
	
	class MouseEvent
	{
		friend Mouse;
		enum class MouseType
		{
			L_Pressed,
			L_Released,
			R_Pressed,
			R_Released,
			WheelDown,
			WheelUp,
			WheelPressed,
			WheelReleased,
			Undefined
		};
	public:
		MouseEvent();
		MouseEvent(MouseType Type, std::pair<short, short> Pos);
		Engine_API bool L_Pressed();
		Engine_API bool R_Pressed();
		Engine_API bool WheelDown();
		Engine_API bool WheelUp();
		Engine_API bool WheelPressed();
		Engine_API std::pair<short, short> GetPos();

	private:
		std::pair<short, short> Pos;
		MouseType t;
	};


public:

	Engine_API Mouse(unsigned int QueueSize = 10);
	Engine_API std::optional<MouseEvent> GetEvent();
	Engine_API std::pair<short, short> GetPos();

private:
	void CALLBACK HandleMsg(HWND& hWnd, UINT& msg, WPARAM& wParam, LPARAM& lParam) override;
	void PopQueue();


	std::queue<MouseEvent> Events;
	unsigned int QueueSize;
	std::pair<short, short> Pos;
};

#endif
