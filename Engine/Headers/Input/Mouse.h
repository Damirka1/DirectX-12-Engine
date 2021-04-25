#pragma once
#ifndef MOUSE_HEADER
#define MOUSE_HEADER
#include "..\MessageHandler.h"
#include "..\Header.h"
#include "..\Events\MouseEvents.h"
#include <queue>
#include <optional>
#include <bitset>


class Mouse : public MessageHandler
{
	friend class ScriptManager;
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

public:
	Engine_API Mouse(unsigned int QueueSize = 10, bool UseRawInput = false);
	Engine_API std::optional<MouseEvent> GetEvent() noexcept;
	Engine_API std::optional<RawData> GetRawData() noexcept;
	Engine_API std::pair<short, short> GetPos() noexcept;
	Engine_API void EnableRawInput() noexcept;
	Engine_API void DisableRawInput() noexcept;
	Engine_API bool IsCursorEnabled() noexcept;
	Engine_API bool IsLbPressed() noexcept;
	Engine_API bool IsRbPressed() noexcept;
	Engine_API bool IsMbPressed() noexcept;

private:
	void CALLBACK HandleMsg(HWND& hWnd, UINT& msg, WPARAM& wParam, LPARAM& lParam) noexcept override;
	void PopQueue()  noexcept;
	void PopQueueRawInput() noexcept;


	unsigned int QueueSize;
	bool UseRawInput;
	bool CursorEnabled;
	bool CursorInWindow;
	std::queue<MouseEvent> Events;
	std::queue<MouseEvent> EventsForWindow;
	std::queue<RawData> RawInputEvents;
	std::vector<BYTE> rawBuffer;
	std::pair<short, short> Pos;
	std::bitset<16> Buttons;
	//[Lb, Rb, Mb, ...]
};

#endif
