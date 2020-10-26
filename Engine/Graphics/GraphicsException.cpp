#include "..\Headers\Graphics\GraphicsException.h"
#ifdef _DEBUG
#include <sstream>

GraphicsException::GraphicsException(DWORD error, UINT line, const char* file)
	:
	ErrorCode(error),
	line(line),
	file(file)
{
}

const char* GraphicsException::What() const
{
	char* buffer = nullptr;
	DWORD m = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER, nullptr, ErrorCode, NULL, buffer, NULL, nullptr);

	std::ostringstream ss;
	ss << "[File]: " << file << "\n" << "[Line]: " << line << "\n";
	ss << "[Message]: " << buffer << "\n";

	return ss.str().c_str();
}

const char* GraphicsException::GetType() const
{
	return "GraphicsException";
}

#endif