#pragma once
#ifdef _DEBUG
#ifndef ERROR_CHECK_HEADER
#define ERROR_CHECK_HEADER
#include "GraphicsException.h"

#define Error_Check(x) if(x != S_OK) throw GraphicsException(GetLastError(), __LINE__, __FILE__);

#endif

#else
#define Error_Check(x) x;

#endif