#pragma once
#ifndef MOUSEEVENTS_HEADER
#define MOUSEEVENTS_HEADER
#include "..\Header.h"
#include <utility>

class MouseEvent
{
public:
	enum class Type
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
		Move,
		EnterWindow,
		LeaveWindow,
		DoubleClick,
	};

	MouseEvent() noexcept;
	MouseEvent(Type Type, std::pair<short, short> Pos) noexcept;
	Engine_API bool L_Pressed() noexcept;
	Engine_API bool R_Pressed() noexcept;
	Engine_API bool WheelDown() noexcept;
	Engine_API bool WheelUp() noexcept;
	Engine_API bool WheelPressed() noexcept;
	Engine_API std::pair<short, short> GetPos() noexcept;
	Engine_API bool operator==(Type t) noexcept;

private:
	std::pair<short, short> Pos;
	Type t;
};

#endif