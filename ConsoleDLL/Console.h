#pragma once
#ifndef CONSOLE_HEADER
#define CONSOLE_HEADER

#include "..\Engine\Headers\Header.h"

class Console
{
public:
	Engine_API Console(const wchar_t* Name);
	Engine_API ~Console();
	Engine_API void Write(const char* str);
	Engine_API void Write(const wchar_t* str);

	Engine_API Console& operator<<(const wchar_t* str);

private:
	static bool ConsoleHandler(int fdwCtrlType);

private:
	void* ConsoleInput;
	void* ConsoleOutput;
};

#endif