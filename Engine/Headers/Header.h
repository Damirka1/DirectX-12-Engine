#pragma once
#if DLL_EXPORT
#	define Engine_API __declspec(dllexport)
#else
#	define Engine_API __declspec(dllimport)
#endif