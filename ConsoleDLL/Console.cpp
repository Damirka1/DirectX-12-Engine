#include "Console.h"
#include <Windows.h>


Console::Console(const wchar_t* Name)
	:
	ConsoleInput(nullptr),
	ConsoleOutput(nullptr)
{
	if (AllocConsole())
	{
		ConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
		ConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTitleW(Name);
		SetConsoleCtrlHandler(reinterpret_cast<PHANDLER_ROUTINE>(ConsoleHandler), TRUE);
	}

}

Console::~Console()
{
	FreeConsole();
}

void Console::Write(const char* str)
{
	WriteConsoleA(ConsoleOutput, str, static_cast<DWORD>(strlen(str)), nullptr, nullptr);
}

void Console::Write(const wchar_t* str)
{
	WriteConsoleW(ConsoleOutput, str, static_cast<DWORD>(wcslen(str)), nullptr, nullptr);
}

Console& Console::operator<<(const wchar_t* str)
{
	WriteConsoleW(ConsoleOutput, str, static_cast<DWORD>(wcslen(str)), nullptr, nullptr);
	return *this;
}

bool Console::ConsoleHandler(int fdwCtrlType)
{
    switch (fdwCtrlType)
    {
	default:
		return FALSE;
    }
}
