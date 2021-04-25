#pragma once
#ifndef KEYBOARDEVENTS_HEADER
#define KEYBOARDEVENTS_HEADER
#include "..\Header.h"

class KeyEvent
{
public:
	enum class Type
	{
		Pressed,
		Released,
		Undefined
	};

	KeyEvent() noexcept;
	KeyEvent(unsigned char Code, Type type) noexcept;

	Engine_API bool operator==(const char Key)  noexcept;

private:
	unsigned char Code;
	Type t;
};

#endif