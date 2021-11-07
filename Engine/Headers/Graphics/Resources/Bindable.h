#pragma once
#ifndef BINDABLE_HEADER
#define BINDABLE_HEADER
#include "..\..\Graphics.h"

class Bindable
{
	
public:
	virtual void Bind(Graphics* pGraphics) = 0;
	virtual void Initialize(Graphics* pGraphics, D3D12_CPU_DESCRIPTOR_HANDLE& pHandle)
	{}
	virtual void Initialize(Graphics* pGraphics)
	{}
	virtual ~Bindable()
	{}

protected:
	bool Initialized = false;
};

#endif