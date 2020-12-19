#pragma once
#include <math.h>

constexpr float PI = 3.14159265f;
constexpr double PI_D = 3.1415926535897932;


double wrap_angle(double theta) noexcept
{
	constexpr double twoPi = 2.0 * PI_D;
	const double mod = fmod(theta, twoPi);
	if (mod > PI_D)
	{
		return mod - twoPi;
	}
	else if (mod < -PI_D)
	{
		return mod + twoPi;
	}
	return mod;
}

float wrap_angle(float theta) noexcept
{
	constexpr float twoPi = 2.0f * PI;
	const float mod = (float)fmod(theta, twoPi);
	if (mod > PI)
	{
		return mod - twoPi;
	}
	else if (mod < -PI)
	{
		return mod + twoPi;
	}
	return mod;
}