#pragma once
#ifndef RESOURCE_HEADER
#define RESOURCE_HEADER
#include "..\..\Graphics.h"

class Resource
{
	friend class ResourceManager;

protected:
	virtual void Initialize(Graphics* pGraphics, D3D12_CPU_DESCRIPTOR_HANDLE& pHandle) = 0;

	virtual ~Resource(){}

	void SetHeapIndex(UINT Table, UINT Range, UINT Index) noexcept
	{
		this->Table = Table;
		this->Range = Range;
		this->Index = Index;
	}

	bool Initialized = false;
	UINT Table = 0u, Range = 0u, Index = 0u;
};

#endif