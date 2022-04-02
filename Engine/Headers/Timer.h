#pragma once
#include <chrono>
#include "Header.h"

class Timer
{
public:
	Engine_API Timer() noexcept;
	Engine_API float Mark() noexcept;
	Engine_API float Peek() const noexcept;
private:
	std::chrono::steady_clock::time_point last;
};