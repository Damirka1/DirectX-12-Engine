#pragma once
#ifndef BINDABLE_HEADER
#define BINDABLE_HEADER
#include "..\..\Graphics.h"

class Bindable
{
	friend class ResourceManager;
public:
	virtual void Bind(Graphics* pGraphics) = 0;
	virtual void Initialize(Graphics* pGraphics, D3D12_CPU_DESCRIPTOR_HANDLE& pHandle)
	{}
	virtual void Initialize(Graphics* pGraphics)
	{}
	virtual ~Bindable()
	{}
	bool IsInitialized()
	{
		return Initialized;
	}
	
protected:
	void SetHeapIndex(UINT Table, UINT Range, UINT Index)
	{
		this->Table = Table;
		this->Range = Range;
		this->Index = Index;
	}

	bool Initialized = false;
	UINT Table = 0u, Range = 0u, Index = 0u;
};

#endif