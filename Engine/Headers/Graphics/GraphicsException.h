#pragma once
#ifndef GRAPHICS_EXCEPTION_HEADER
#define GRAPHICS_EXCEPTION_HEADER
#include <Windows.h>
#include "..\Header.h"
#include <string>


class GraphicsException
{
public:
	Engine_API GraphicsException(DWORD error, UINT line, const char* file);
	Engine_API std::string What() const;
	Engine_API const char* GetType() const;

private:
	DWORD ErrorCode;
	UINT line;
	const char* file;
};

#endif