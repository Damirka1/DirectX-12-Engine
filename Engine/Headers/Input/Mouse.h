#pragma once
#ifndef MOUSE_HEADER
#define MOUSE_HEADER
#include "..\MessageHandler.h"
#include "..\Header.h"
#include <queue>
#include <optional>


class Mouse : public MessageHandler
{
	friend class Window;
	struct RawData
	{
		RawData(LONG dx, LONG dy)
			:
			dx(dx),
			dy(dy)
		{}
		LONG dx, dy;
	};
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
			Undefined,
			Enter,
			Leave,
		};
	public:
		MouseEvent() noexcept;
		MouseEvent(MouseType Type, std::pair<short, short> Pos) noexcept;
		Engine_API bool L_Pressed() noexcept;
		Engine_API bool R_Pressed() noexcept;
		Engine_API bool WheelDown() noexcept;
		Engine_API bool WheelUp() noexcept;
		Engine_API bool WheelPressed() noexcept;
		Engine_API std::pair<short, short> GetPos() noexcept;

	private:
		std::pair<short, short> Pos;
		MouseType t;
	};


public:

	Engine_API Mouse(unsigned int QueueSize = 10, bool UseRawInput = false);
	Engine_API std::optional<MouseEvent> GetEvent() noexcept;
	Engine_API std::optional<RawData> GetRawData() noexcept;
	Engine_API std::pair<short, short> GetPos() noexcept;
	Engine_API void EnableRawInput() noexcept;
	Engine_API void DisableRawInput() noexcept;
	Engine_API bool IsCursorEnabled() noexcept;

private:
	void CALLBACK HandleMsg(HWND& hWnd, UINT& msg, WPARAM& wParam, LPARAM& lParam) noexcept override;
	void PopQueue()  noexcept;
	void PopQueueRawInput() noexcept;


	unsigned int QueueSize;
	bool UseRawInput;
	bool CursorEnabled;
	bool CursorInWindow;
	std::queue<MouseEvent> Events;
	std::queue<RawData> RawInputEvents;
	std::vector<BYTE> rawBuffer;
	std::pair<short, short> Pos;
};

#endif
