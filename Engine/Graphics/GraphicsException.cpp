#ifdef _DEBUG
#include "..\Headers\Graphics\GraphicsException.h"
#include <sstream>

GraphicsException::GraphicsException(DWORD error, UINT line, const char* file)
	:
	ErrorCode(error),
	line(line),
	file(file)
{
}

std::string GraphicsException::What() const
{
	char* buffer = nullptr;
	DWORD m = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER, nullptr, ErrorCode, NULL, buffer, NULL, nullptr);

	std::ostringstream ss;
	ss << "[File]: " << file << "\n" << "[Line]: " << line << "\n";
	ss << "[Message]: " << (buffer ? buffer : "No description") << "\n" << "\0";

	return ss.str();
}

const char* GraphicsException::GetType() const
{
	return "GraphicsException";
}

#endif