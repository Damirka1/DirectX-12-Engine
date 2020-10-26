#pragma once
#ifndef GRAPHICS_EXCEPTION_HEADER
#define GRAPHICS_EXCEPTION_HEADER
#include <Windows.h>

class GraphicsException
{
public:
	GraphicsException(DWORD error, UINT line, const char* file);
	const char* What() const;
	const char* GetType() const;

private:
	DWORD ErrorCode;
	UINT line;
	const char* file;
};

#endif