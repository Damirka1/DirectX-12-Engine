#include "..\Headers\Events\MouseEvents.h"

MouseEvent::MouseEvent() noexcept
	:
	t(MouseEvent::Type::Undefined),
	Pos(-1, -1)
{
}

MouseEvent::MouseEvent(Type Type, std::pair<short, short> Pos) noexcept
	:
	t(Type),
	Pos(Pos)
{
}

bool MouseEvent::L_Pressed() noexcept
{
	return t == Type::L_Pressed ? true : false;
}

bool MouseEvent::R_Pressed() noexcept
{
	return t == Type::R_Pressed ? true : false;
}

bool MouseEvent::WheelDown() noexcept
{
	return t == Type::WheelDown ? true : false;
}

bool MouseEvent::WheelUp() noexcept
{
	return t == Type::WheelUp ? true : false;
}

bool MouseEvent::WheelPressed() noexcept
{
	return t == Type::WheelPressed ? true : false;
}

std::pair<short, short> MouseEvent::GetPos() noexcept
{
	return Pos;
}

bool MouseEvent::operator==(Type t) noexcept
{
	return this->t == t;
}