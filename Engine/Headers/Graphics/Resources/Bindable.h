#pragma once
#ifndef BINDABLE_HEADER
#define BINDABLE_HEADER
#include "..\..\Graphics.h"

class Bindable
{
public:
	virtual void Bind(Graphics* pGraphics) = 0;
	virtual ~Bindable()
	{

	}
};

#endif